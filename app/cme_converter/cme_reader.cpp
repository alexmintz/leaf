/*
*
*	� Copyright 2012 by TAF Co. All rights reserved.
*
*/

#include "cme_reader.h"
#include "leaf/LFMarketState.h"
#include "leaf/LFCommonData.h"
#include <iomanip>

int main(int argc, char* argv[])
{
	
	try {
		Z::add_command_line_option("d", "CLE directory for processing");
		Z::add_command_line_option("trades_only", "Process trades only");
		Z::init(argc, argv);

		std::string cle_dir = Z::get_setting("COMMAND:d");
		if(!cle_dir.empty())
			TCSettings::set_resource("BMTS:cle_dir", cle_dir);
		

		TCFiler::instance()->set_mode(ios::app);
		BMTS<LFDepthData>::instance()->_out_file = true;
		BMTS<LFTradeData>::instance()->_out_file = true;
		BMTS<LFVWAPData>::instance()->_out_file = true;

		BMTS<CMEDataPoint>::instance()->_out_file = true;

		BMModel::instance()->subscribe<LFDepthData>();
		BMModel::instance()->subscribe<LFTradeData>();
		BMModel::instance()->subscribe<LFVWAPData>();
		BMModel::instance()->open();
		LFMarketState::instance()->open();


		CMEReader rd(cle_dir, Z::get_setting_bool("COMMAND:trades_only"));
		rd.run();

		BMModel::instance()->close();
		LFMarketState::instance()->close();
		Z::fini();
	}
	catch(Z::Exception& e)
	{
		std::cout << e << std::endl;
		return -1;
	}
	return 0;
}

CMEReader::CMEReader(const std::string& dir_, bool trades_only_)
: BMSequenceReader(dir_, ".txt"),
  _worker(1, "worker"), _f_ind(0),
  _cme_fix_out_file(Z::get_setting_bool("BMTS:CMEDataPoint:out_file", false)),
  _trades_only(trades_only_),
  _repeat_bbo(TCSettings::get_bool("LFModel:repeat_bbo", false)), 
  _vwap_over(false)
{
	_back_trace.init(Z::get_setting("BMTS:CMEDataPoint:back_trace", "20150624_070000:2050624_100000"));
	init();
	_worker.hwm(100);
	_worker.lwm(10);
	_worker.open();
	_worker.register_handler (this, &CMEReader::process);
}

bool converter (std::string i, std::string j) 
{ 
	std::vector<std::string> iv;
	std::vector<std::string> jv;
	
	tc_token_vec(i, iv, '.');
	tc_token_vec(j, jv, '.');

	size_t is = iv.size();
	size_t js = jv.size();

	if(iv[is-1] == "txt")
		return true;
	if(jv[js-1] == "txt")
		return false;

	return iv[is-1] < jv[js-1];
}
void CMEReader::init()
{
	_v.clear();
		
	ACE_Dirent adr(_dir.c_str());
	dirent* dp = 0;
	std::vector<std::string> v;
	while( (dp = adr.read()) != 0 )
	{
		std::string tn = dp->d_name;
		tc_token_vec(tn, v, '.');
		size_t s = v.size();
		if(s == 0 || v[s-1] == "cfg")
			continue;
		_fvec.push_back(_dir + "/" + tn);
	}
	_f_ind = _fvec.size();
	sort(_fvec.begin(), _fvec.end(), converter);
	for(size_t i = 0; i < _fvec.size(); ++i)
		_v.push_back(new Reader(_fvec[i], *this));
}
CMEReader::~CMEReader()
{
	_worker.unregister_handler<ACE_Message_Block>();
}
void CMEReader::run()
{
	while(!step());
}

void CMEReader::process_hdr(ACE_Message_Block* mb_)
{
	process_rec(mb_);
}
void CMEReader::process_eof(bool full_)
{
	--_f_ind;
	if(_f_ind == 0)
	{
		_worker.close(false);
		_worker.wait();
	}
}
void CMEReader::process_group(int action_,
	const ACE_Time_Value& timestamp_,
	const std::string& sec_,
	std::map<int, std::string>& m,
	std::set<std::string>& depth_report_,
	std::set<std::string>& trade_report_,
	size_t group_left_)
{
	//entry type
	int side = 0;

	if (m[269] == "0")
		side = 0;
	else if (m[269] == "1")
		side = 1;
	else if (m[269] == "2")
		side = 2;
	else
		return;

	if (side == 2)
	{
		process_trade_group(action_, sec_, timestamp_, m, trade_report_);
		return;
	}

	//level
	int level = atoi(m[1023].c_str()) - 1;
	if (level == -1 || level > LFDepthDataReader::DEPTH_SIZE - 1)
		return;
	if (!_trades_only)
		process_quote_group(action_, sec_, side, level, timestamp_, m, depth_report_, group_left_);
}
void CMEReader::process_trade_group(int action_, const std::string& sec_, const ACE_Time_Value& tv_, std::map<int, std::string>& m, std::set<std::string>& report_)
{
	if (!LFSecurityMaster::instance()->set_cqg(m[107], sec_))
		return;


	double price = ACE_OS::strtod(m[270].c_str(), 0);
	price = price / LFSecurityMaster::instance()->get_price_factor();
	double qty = ACE_OS::strtod(m[271].c_str(), 0);

	LFTradeData& lftrade = _tm[m[107]];
	lftrade._t = tv_;

	if (lftrade._instr_key.empty())
		lftrade._instr_key = m[107];
	
	_cme_data.reset_b(tv_, action_, m[83], m[269], m[270], m[271], m[273], m[274], m[276], m[277], m[336], m[346], m[451], m[1023]);


	std::ostringstream os;
	double d = std::fabs(lftrade._trade._p - price);

	switch (action_)
	{
	case 0: // New Trade
		if (!m[277].empty())
			break;
		
		if (d > 0.1)
				os << "Trade price diff > 10c" << std::endl;

		lftrade._trade.reset(price, qty);
		break;

	case 1: // Change Trade

		break;

	case 2: // Delete Trade

		break;

	case 3: // Delete Trhough Trade
	case 4: // Delete From Trade
	case 5: // Delete Overlay Trade
	default:
		os << "UNEXPECTED Trade action " << action_;
	}
	if (_cme_fix_out_file && _back_trace.in_range(tv_))
		LF::model_send(_cme_data, os.str(), tv_, _cme_fix_out_file);

	TCDateTime tcdt(tv_);

	if (!_vwap_over && 
		tcdt.get_time().hour() == 14 &&
		tcdt.get_time().minute() > 27)
	{
		_vm[lftrade._instr_key][price] += qty;
		if (tcdt.get_time().minute() > 29)
		{
			//compute vwap
			for (VWAPMap::iterator it = _vm.begin(); it != _vm.end(); ++it)
			{
				double vwap = 0;
				double tot_qty = 0;
				PQPMap& pqp = (*it).second;
				for (PQPMap::iterator ki = pqp.begin(); ki != pqp.end(); ++ki)
				{
					tot_qty += ki->second;
					vwap += ki->first*ki->second;
				}
				if (tot_qty == 0)
					vwap = 0;
				else
					vwap /= tot_qty;
				// report vwap for lftrade._instr_key
				BMModel::instance()->send(new LFVWAPData((*it).first, tv_, vwap));
			}
			_vwap_over = true;
		}
	}
	LFMarketState::instance()->send(new LFTradeData(lftrade));
}

void CMEReader::process_quote_group(int action_, const std::string& sec_, int side, int level,
	const ACE_Time_Value& tv_, std::map<int, std::string>& m, std::set<std::string>& report_, size_t group_left_)
{
	// ignore implied prices for futures
	if (sec_ == m[107] && (m[276] == "K" || m[276] == "C"))
		return;

	// wrong security
	if (!LFSecurityMaster::instance()->set_cqg(m[107], sec_))
		return;


	double price = ACE_OS::strtod(m[270].c_str(), 0);
	price = price / LFSecurityMaster::instance()->get_price_factor();
	double qty = ACE_OS::strtod(m[271].c_str(), 0);

	// temporary storage of the group member;
	LFDepthDataReader& lfdepth = _dm[m[107]].second;

	if(lfdepth._instr_key.empty())
	{
		lfdepth._instr_key	= m[107];
		lfdepth._tick_size	= LFSecurityMaster::instance()->get_tick_size();
		lfdepth._tick_value = LFSecurityMaster::instance()->get_tick_value();
	}

	std::ostringstream os; 
	os << std::fixed << std::setprecision(6);

	switch (side)
	{
		case 0: //bid
		{
			switch (action_)
			{
			case 0: // New Bid

				//shift down
				if (lfdepth._bid[level].good())
				{ // no need to shift if slot is empty
					// find last nonNAN entry and shift until it
					size_t k = 0;
					for (; k < LFDepthDataReader::DEPTH_SIZE - 1 && lfdepth._bid[k].good(); ++k);

					for (size_t i = k; i > level; --i)
						lfdepth._bid[i] = lfdepth._bid[i - 1];
				}
				lfdepth._bid[level].reset(price, qty);

				// check if everything prior to level is valid
				for (int k = level - 1; k >= 0; --k)
				{
					if (!lfdepth._bid[level].good() || TCComparePrice::diff(price, lfdepth._bid[k]._p) >= 0)
						lfdepth._bid[k].reset();
				}
				break;

			case 1: // Change Bid
				//if (!lfdepth._bid[level].good() || TCComparePrice::diff(lfdepth._bid[level]._p, price) == 0)
					lfdepth._bid[level].reset(price, qty);
				break;

			case 2: //delete
				if (!lfdepth._bid[level + 1].good())
				{
					lfdepth._bid[level].reset();
					break;
				}
				for (int i = level; i < LFDepthDataReader::DEPTH_SIZE - 1; ++i)
					lfdepth._bid[i] = lfdepth._bid[i + 1];

				lfdepth._bid[LFDepthDataReader::DEPTH_SIZE - 1].reset();
				break;

			default:
				os << "UNEXPECTED BID action " << action_ << std::endl;
			}
			break;
		}
		case 1: //offer
		{
			switch (action_)
			{
			case 0: // New Ask

				//shift down
				if (lfdepth._ask[level].good())
				{ // no need to shift if slot is empty
					// find last nonNAN entry and shift until it
					size_t k = 0;
					for (k = 0; k < LFDepthDataReader::DEPTH_SIZE - 1 && lfdepth._ask[k].good(); ++k);
					for (size_t i = k; i > level; --i)
						lfdepth._ask[i] = lfdepth._ask[i - 1];
				}
				lfdepth._ask[level].reset(price, qty);
				// check if everything prior to level is valid
				for (int k = level - 1; k >= 0; --k)
				{
					if (TCComparePrice::diff(lfdepth._ask[k]._p, price) >= 0)
						lfdepth._ask[k].reset();
				}
				break;
			case 1: // Change Ask
				//if (!lfdepth._ask[level].good() || TCComparePrice::diff(lfdepth._ask[level]._p, price) == 0)
					lfdepth._ask[level].reset(price, qty);
				break;
			case 2: //delete
				if (!lfdepth._ask[level + 1].good())
				{
					lfdepth._ask[level].reset();
					break;
				}
				for (int i = level; i < LFDepthDataReader::DEPTH_SIZE - 1; ++i)
					lfdepth._ask[i] = lfdepth._ask[i + 1];
				lfdepth._ask[LFDepthDataReader::DEPTH_SIZE - 1].reset();
				break;

			default:
				os << "UNEXPECTED ASK action " << action_ << std::endl;
			}
			break;
		}
	}

	if (!os.str().empty())
		Z::report_error("%s", os.str().c_str());

	if (group_left_ > 0)
		return;

	// last member, we are done with transaction, make a record
	
	if (!lfdepth._bid[0].good() || !lfdepth._ask[0].good())
		return;

	if (TCComparePrice::diff(lfdepth._bid[0]._p, lfdepth._ask[0]._p) > 0)
	{
		Z::report_error("bid [%f] > ask [%f]", lfdepth._bid[0]._p, lfdepth._ask[0]._p);
		return;
	}

	// previously processed group;
	LFDepthData& lfdepth_prev = _dm[m[107]].first;


	if (TCComparePrice::diff(lfdepth_prev._bid[0]._p, lfdepth._bid[0]._p) != 0 ||
		TCComparePrice::diff(lfdepth_prev._ask[0]._p, lfdepth._ask[0]._p) != 0 ||
		_repeat_bbo)
	{
		
		if (lfdepth_prev._instr_key.empty())
		{
			lfdepth_prev._instr_key = lfdepth._instr_key;
			lfdepth_prev._cqg_InstrumentID = lfdepth._cqg_InstrumentID;
			lfdepth_prev._tick_size = lfdepth._tick_size;
			lfdepth_prev._tick_value = lfdepth._tick_value;
		}
		lfdepth_prev._ask[0] = lfdepth._ask[0];
		lfdepth_prev._bid[0] = lfdepth._bid[0];
		lfdepth_prev._t = tv_;
		report_.insert(lfdepth_prev._instr_key);
	}
}

ACE_Time_Value CMEReader::get_ace_time(const std::string& ts_, bool& same_day_)
{
	tm ttm;
	ttm.tm_mday = atoi(ts_.substr(6, 2).c_str());
	ttm.tm_mon	= atoi(ts_.substr(4, 2).c_str()) - 1;
	ttm.tm_year = atoi(ts_.substr(0, 4).c_str()) - 1900;
	ttm.tm_hour = atoi(ts_.substr(8, 2).c_str());
	ttm.tm_min	= atoi(ts_.substr(10,2).c_str());
	ttm.tm_sec	= atoi(ts_.substr(12,2).c_str());
	ttm.tm_isdst= -1;
	long usec = atoi(ts_.substr(14,3).c_str())*1000; 
	
	TCDateTime tcdt(ttm);
	
	//return tcdt.get_ace_from_gmt_time(usec);
	ACE_Time_Value ret_atv = tcdt.get_ace_from_gmt_time(usec);
	TCDateTime tcdt_converted(ret_atv);
	int hour = tcdt_converted.get_time().hour();
	same_day_ = (tcdt.get_date() == tcdt_converted.get_date() && hour < 17);
	return ret_atv;
}
void CMEReader::process_rec(ACE_Message_Block* mb_)
{
	_worker.send(mb_);
}
void CMEReader::process(ACE_Message_Block* mb_)
{
	auto_ptr<ACE_Message_Block> m(mb_);
	std::vector<const char*> v;
	tc_vec(v, mb_, "\x01");

	//Z::report_debug("processed sequence # %s", v[4]);
	// take only "X"
	
	if(strcmp(v[2],"35=X")!=0)
		return;

	std::string ts;

	// tag 52 timestamp
	ts = v[5]; 
	std::string timestamp = ts.substr(3);
	_cme_data.set_key(ts.substr(3, 8));

	bool same_day;
	ACE_Time_Value tv = get_ace_time(timestamp, same_day);

	if (!same_day)
		return;

	// tag 75 date 
	ts = v[6];
	std::string date = ts.substr(3);
	TCDate tcdt(atoi(date.substr(6).c_str()), atoi(date.substr(4, 2).c_str()), atoi(date.substr(0, 4).c_str()));
	std::string sec = LFSecurityMaster::instance()->get_cl(tcdt); //get the front month sec

	_cme_data.reset_h(timestamp, date);

	ts = v[7];
	size_t entries = atoi(ts.substr(4).c_str());

	size_t res;
	std::set<std::string> depth_report;
	std::set<std::string> trade_report;

	size_t group_size = v.size();
	
	for(size_t i = 8; i < group_size; ++i)
	{
		ts = v[i];

		// action
		res = ts.find("279=");
		if(res != std::string::npos)
		{
			
			while(i < group_size)
			{
				int action = atoi(ts.substr(4).c_str());
				std::map<int, std::string> m;

				for(i++; i < group_size; ++i)
				{
					ts = v[i];
					res = ts.find_first_of('=');
					if(res == std::string::npos)
						continue;
					int index = atoi(ts.substr(0, res).c_str());
					if(index == 279)
						break;
					m[index] = ts.substr(res+1);
				}
				process_group(action, tv, sec, m, depth_report, trade_report, group_size-i);
			}
		}
	}
	
	for (std::set<std::string>::iterator k = depth_report.begin(); k != depth_report.end(); ++k)
	{
		LFDepthData& lfdepth = _dm[*k].first;
		//BMModel::instance()->send(new LFDepthDataReader(lfdepth));
		LFMarketState::instance()->send(new LFDepthData(lfdepth));
	}
	/*
	for (std::set<std::string>::iterator l = trade_report.begin(); l != trade_report.end(); ++l)
	{
		LFTradeData& lftrade = _tm[*l];
		//BMModel::instance()->send(new LFTradeData(lftrade));
		LFMarketState::instance()->send(new LFTradeData(lftrade));
	}
	*/
}
//======================================================================


