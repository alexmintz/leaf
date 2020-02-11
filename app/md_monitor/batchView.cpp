#include "stdafx.h"
#include "p_format.h"
#include "batchView.h"
#include "leaf/Version.h"
#include "znet/TCDateTime.h"

void CALLBACK EXPORT batch_timer_proc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	Z::process_ready_results();
}
IMPLEMENT_DYNCREATE(Cp_BatchView, ETSLayoutFormView)

BEGIN_MESSAGE_MAP(Cp_BatchView, ETSLayoutFormView)
	ON_BN_CLICKED(IDBATCH,	OnBnClickedBatch)
END_MESSAGE_MAP()

//	return ",R_KEY,RUN_ID,INSTR,OPEN_MAX,BUYS,SELLS,MIN_REALIZED, MAX_REALIZED,MIN_UNREALIZED,MAX_UNREALIZED,MIN_PNL,MAX_PNL";


//======================================================================
Cp_BatchView::Ctrl::Ctrl()
	:_writer("BATCH_STATS_" + std::string(LEAF_VERSION)),
	_writer_tot("BATCH_STATS_TOTAL_" + std::string(LEAF_VERSION)),
	_trades_writer("TRADE_STATS_" + std::string(LEAF_VERSION)),
	_trades_writer_tot("TRADE_STATS_TOTAL_" + std::string(LEAF_VERSION))
{
	_risk_free_rate = TCSettings::get_double("LFModel:risk_free_rate", 0.91); 
	_risk_free_rate /= 253; //daily rate
	_initial_margin = TCSettings::get_double("LFModel:initial_margin", 2700);
	_initial_margin *= TCSettings::get_double("LFModel:position_limit", 250);

	std::ostringstream os; 
	os  << std::fixed << std::setprecision(0) 
		<<	"Sharpe/Return on $" 
		<< _initial_margin 
		<< " initial margin";

	add_column("Run",				f_loc(&Record::_run));
	add_column("Instr",				f_loc(&Record::_run_stats, &LFRunStats::_instr));
	add_column("Date",				f_loc(&Record::_date));

	add_column("Open Max",			f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_open_max));
	add_column("Total Buys",		f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_num_buys));
	add_column("Total Sells",		f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_num_sells));

	add_column_c("Min Realized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_min_realized_pnl), P_COLOR::neg_red());
	add_column_c("Max Realized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_max_realized_pnl), P_COLOR::neg_red());
	add_column_c("Min UnRealized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_min_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Max UnRealized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_max_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Min PnL",			f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_min_pnl), P_COLOR::neg_red());
	add_column_c("Max PnL",			f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_max_pnl), P_COLOR::neg_red());

	add_column_c("Last Realized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_realized_pnl), P_COLOR::neg_red());
	add_column_c("Last Unrealized",	f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Last PnL",		f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_pnl), P_COLOR::neg_red());
	add_column("Last Open",			f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_open));
	add_column("DrawDown", f_loc(&Record::_run_stats, &LFRunStats::_c_data, &LFTradeStats::_last_exec));

	add_column(os.str(),			f_loc(&Record::_run_data, &LFRunData::_s_base));

}
//======================================================================
Cp_BatchView::Ctrl1::Ctrl1()
{
	add_column("Start", f_loc(&Record::_s_t_str));
	add_column("End", f_loc(&Record::_end_t_str));
	add_column("Side", f_loc(&Record::_side), P_FORMAT::side());

	add_column("Open Max", f_loc(&Record::_open_max));
	add_column("Total Buys", f_loc(&Record::_num_buys));
	add_column("Total Sells", f_loc(&Record::_num_sells));

	add_column_c("Min Realized", f_loc(&Record::_min_realized_pnl), P_COLOR::neg_red());
	add_column_c("Max Realized", f_loc(&Record::_max_realized_pnl), P_COLOR::neg_red());
	add_column_c("Min UnRealized", f_loc(&Record::_min_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Max UnRealized", f_loc(&Record::_max_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Min PnL", f_loc(&Record::_min_pnl), P_COLOR::neg_red());
	add_column_c("Max PnL", f_loc(&Record::_max_pnl), P_COLOR::neg_red());

	add_column_c("Last Realized", f_loc(&Record::_realized_pnl), P_COLOR::neg_red());
	add_column_c("Last Unrealized", f_loc(&Record::_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Last PnL", f_loc(&Record::_pnl), P_COLOR::neg_red());
	add_column("Last Open", f_loc(&Record::_open));
	add_column("Last_Exec", f_loc(&Record::_last_exec));
}
//======================================================================
Cp_BatchView::Cp_BatchView()
: ETSLayoutFormView(IDD_BATCH),
_ctrl(new Ctrl),
_ctrl1(new Ctrl1)
{ 
}
//======================================================================
bool Cp_BatchView::Ctrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{

	clrText_ = 0xFF000000;
	clrTextBk_ = 0xFF000000;
	if(r_->_date.empty())
	{
		clrTextBk_ = TC_COL::Yellow;
	};
	return true;
}
//======================================================================
bool Cp_BatchView::Ctrl1::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{

	clrText_ = 0xFF000000;
	clrTextBk_ = 0xFF000000;
	if(r_->_s_t == ACE_Time_Value::zero)
	{
		clrTextBk_ = TC_COL::Yellow;
	};
	return true;
}
//======================================================================
Cp_BatchView::~Cp_BatchView()
{
}
//======================================================================
void Cp_BatchView::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BATCH, *_ctrl);
	DDX_Control(pDX, IDC_LIST_SLOPES, *_ctrl1);
}
//======================================================================
void Cp_BatchView::OnInitialUpdate()
{
	ETSLayoutFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);

	GetParentFrame()->ShowWindow( SW_SHOWMINIMIZED ) ;
	update_layout();
}
//======================================================================
void Cp_BatchView::update_layout()
{
	UpdateLayout(
	pane(VERTICAL)<<
	(pane(HORIZONTAL)
		<< (pane(VERTICAL) << item(IDC_LIST_BATCH)))
	<< 
	(pane(HORIZONTAL)
		<< (pane(VERTICAL) << item(IDC_LIST_SLOPES)))
	<< (pane(HORIZONTAL, ABSOLUTE_VERT) 
		<< item( paneNull, GREEDY)
		<< item(IDBATCH, NORESIZE | ALIGN_CENTER)
		<< item( paneNull, GREEDY))
	);
}
//======================================================================
void Cp_BatchView::process_result(LFRunEvent* result_, const Z::Param& param_)
{
	bool last = Z::in_call() == 0;
	_ctrl->process_result(result_, param_, last);
	if(last)
	{
		GetDlgItem(IDBATCH)->EnableWindow(TRUE);
		::KillTimer(0,_timer);
	}
}
//======================================================================
void  Cp_BatchView::Ctrl::make_tot_dates(std::string& month_, std::string& quarter_, std::string& year_, const std::string& date_)
{
	TCDate d(date_);
	std::ostringstream m_os;
	std::ostringstream q_os;
	std::ostringstream y_os;
	m_os << d.year() << "_M";
	q_os << d.year() << "_Q";
	y_os << d.year() << "_Y";
	unsigned char m = d.month();
	if (m < 10)
		m_os << "_0";
	else
		m_os << "_";
	m_os << (int)m;
	if (m <= 3)
		q_os << "_1";
	else if (m <= 6)
		q_os << "_2";
	else if (m <= 9)
		q_os << "_3";
	else
		q_os << "_4";

	month_ = m_os.str();
	quarter_ = q_os.str();
	year_ = y_os.str();
}
//======================================================================
void Cp_BatchView::Ctrl::update_totals(LFTradeStats& tdata_, const LFTradeStats& pdata_)
{
	tdata_._min_realized_pnl = std::min(tdata_._min_realized_pnl, pdata_._min_realized_pnl);
	tdata_._max_realized_pnl = std::max(tdata_._max_realized_pnl, pdata_._max_realized_pnl);
	tdata_._min_unrealized_pnl = std::min(tdata_._min_unrealized_pnl, pdata_._min_unrealized_pnl);
	tdata_._max_unrealized_pnl = std::max(tdata_._max_unrealized_pnl, pdata_._max_unrealized_pnl);
	tdata_._min_pnl = std::min(tdata_._min_pnl, pdata_._min_pnl);
	tdata_._max_pnl = std::max(tdata_._max_pnl, pdata_._max_pnl);

	tdata_._open_max++;

	if (pdata_._pnl > 0)
	{
		tdata_._realized_pnl += pdata_._pnl;
		tdata_._num_buys++;
	}
	else if (pdata_._pnl < 0)
	{
		tdata_._unrealized_pnl += pdata_._pnl;
		tdata_._num_buys++;
		tdata_._num_sells--;
	}
	else if (pdata_._open_max != 0)
		tdata_._num_buys++;

	tdata_._pnl += pdata_._pnl;
	double t_pnl = tdata_._realized_pnl - tdata_._unrealized_pnl;
	tdata_._open = t_pnl <= 0.? 0. : tdata_._realized_pnl*100. / t_pnl;
}
//======================================================================
void Cp_BatchView::Ctrl::update_sub_totals(const std::string& sub_date_, const LFTradeStats& pdata_)
{
	LFRunEventPtr& sub_tot = _sub_tot[sub_date_];
	if (sub_tot.get() == 0)
	{
		sub_tot.reset(new LFRunEvent());
		sub_tot->_date = sub_date_;
		sub_tot->_t = ACE_OS::gettimeofday();
	}
	update_totals(sub_tot->_run_stats._c_data, pdata_);
}
//======================================================================
void Cp_BatchView::Ctrl::process_result(LFRunEvent* result_, const Z::Param& param_, bool last_)
{
	auto_ptr<LFRunEvent> r(result_);
	BASE::R_Ptr p( new LFRunEvent(*result_) );
	
	p->_run = r->_run_stats._run;
	p->_run_data._s_base = r->_run_data.as_string();
	p->init_date();
	result_->_date = p->_date;

	LFRunEvent& tot = *_tot;
	LFTradeStats& tdata = tot._run_stats._c_data;
	LFTradeStats& pdata = p->_run_stats._c_data;
	
	update_totals(tdata, pdata);

	// Daily Sharpe computation
	tot._run_data._s_base = sharpe(pdata); 

	// store daily PnL
	_daily_pnl.push_back(LFDayPnL(TCDate(p->_date).get_ymd(), pdata._pnl));
	if (last_)
		tdata._last_exec = drawdown();

	// sub totals computations
	std::string m_date;
	std::string q_date;
	std::string y_date;
	make_tot_dates(m_date, q_date, y_date, p->_date);
	
	update_sub_totals(m_date, pdata);
	update_sub_totals(q_date, pdata);
	update_sub_totals(y_date, pdata);

	BASE::R_Vec r_vec(2, _tot);
	r_vec[1] = p;
	
	update_data(r_vec);
	r->_t = ACE_OS::gettimeofday();
	for(size_t i = 0; i < r->_teasers.size(); ++i)
	{
		auto_ptr<LFMatch> sl(new LFMatch(r->_teasers[i]));
		sl->_t = r->_t;
		sl->_run = 0;
		//if(sl->_is_enter && (_E_match.empty() || sl->_m_name == _E_match) )
		//	_matches->process_result(sl.get());
		SlopeWriterPtr& w = _slope_writers[sl->get_key()];
		if(w.get() == 0)
			w.reset(new BMPointWriter<LFMatch>(sl->get_key()));
		w->send(sl.release());
	}
	for (size_t i = 0; i < r->_run_stats._t_history.size(); ++i)
	{
		auto_ptr<LFTradeStats> ts(new LFTradeStats(r->_run_stats._t_history[i]));
		ts->_t = r->_t;
		ts->_run = 0;
		_trades->process_result(ts.get());
		_trades_writer.send(ts.release());
	}
	_writer.send(r.release());
	if(last_)
	{
		_writer.close_os();
		for (LFRunEventPtrMap::iterator ii = _sub_tot.begin(); ii != _sub_tot.end(); ++ii)
			_writer_tot.send(new LFTotalEvent(*(*ii).second));
		auto_ptr<LFTotalEvent> tev(new LFTotalEvent(*_tot));
		tev->_date = "TOTAL";
		_writer_tot.send(tev.release());
		_writer_tot.close_os();
		for (SlopeWriterMap::iterator i = _slope_writers.begin(); i != _slope_writers.end(); ++i)
			(*i).second->close_os();
		_trades_writer.close_os();
		_trades_writer_tot.send(new LFTradeStats(*_trades->_tot));
		_trades_writer_tot.close_os();
	}
}
bool day_pnl_less(const LFDayPnL& p1_, const LFDayPnL& p2_)
{
	return p1_.first < p2_.first;
}
//======================================================================
bool draw_down_less(const LFDrawDown& p1_, const LFDrawDown& p2_)
{
	return p1_._max_down < p2_._max_down;
}
//======================================================================
std::string Cp_BatchView::Ctrl::drawdown()
{
	if (_daily_pnl.size() < 2)
		return "";

	std::sort(_daily_pnl.begin(), _daily_pnl.end(), day_pnl_less);


	std::vector<LFDrawDown> down_vec;

	LFDrawDown down;

	double tot_pnl = 0;
	double max_pnl = 0;
	for (size_t i = 0; i < _daily_pnl.size(); ++i)
	{
		LFDayPnL& d = _daily_pnl[i];
		tot_pnl += d.second;
		if (tot_pnl < max_pnl)
		{
			double ddown = max_pnl - tot_pnl;
			if (down._end_date != 0)
			{
				down_vec.push_back(down);
				down.reset();
			}
			if (down._start_date == 0)
			{
				down._start_date = d.first;
				down._max_down = ddown;
				down._max_date = d.first;
			}
			else if (ddown > down._max_down)
			{
				down._max_down = ddown;
				down._max_date = d.first;
			}
		}
		else
		{
			max_pnl = tot_pnl;
			if (down._start_date != 0)
			{
				down._end_date = d.first;
				down_vec.push_back(down);
				down.reset();
			}
		}
	}

	if (down._start_date != 0)
		down_vec.push_back(down);

	if (down_vec.size() == 0)
		return "";

	BMPointWriter<LFDrawDown>	writer_dd("DRAWDOWN_" + std::string(LEAF_VERSION));
	ACE_Time_Value t = ACE_OS::gettimeofday();

	for (size_t ii = 0; ii < down_vec.size(); ++ii)
	{
		down_vec[ii]._t = t;
		writer_dd.send(new LFDrawDown(down_vec[ii]));
	}
	writer_dd.close_os();

	std::sort(down_vec.begin(), down_vec.end(), draw_down_less);

	LFDrawDown& dworst = *down_vec.rbegin();
	std::ostringstream os;
	os << dworst._max_down << " @ " << dworst._max_date << " [" << dworst._start_date << " : " << dworst._end_date << "]";
	return os.str();
}
//======================================================================
std::string Cp_BatchView::Ctrl::sharpe(const LFTradeStats& _p_data)
{
	LFRunEvent& tot = *_tot;
	LFTradeStats& tdata = tot._run_stats._c_data;

	if(tdata._pnl == 0)
		return "N/A";

	double tot_avg_return = tdata._pnl/(_initial_margin/100);
	double avg_return = tot_avg_return/tdata._open_max;
	_daily_returns_v.push_back(_p_data._pnl/(_initial_margin/100));

	// standard deviation
	double s_dev = 0;

	for(size_t i = 1; i < _daily_returns_v.size(); ++i)
		s_dev += pow((avg_return - _daily_returns_v[i]), 2);

	s_dev /= tdata._open_max;
	s_dev = sqrt(s_dev);

	double sharpe = (avg_return - _risk_free_rate)/s_dev;
	std::ostringstream os;
	os	<< std::fixed << std::setprecision(2) 
		<< sharpe*sqrt(253.0) 
		<< " Tot "	<< tot_avg_return 
		<< "% "		<< avg_return*252 << "%/year";
	return os.str();
}
//======================================================================
void Cp_BatchView::Ctrl::init(Ctrl1* matches_)
{
	_E_match = Z::get_setting("LFModel:Batch:E_match", "");
	_trades = matches_;
	_trades->init();
	_daily_returns_v.clear();
	_daily_pnl.clear();
	mark_for_delete();
	do_delete();

	_tot.reset( new LFRunEvent );
	_tot->_run_stats._run = 0;
	_tot->_run = 0;
	_tot->_t = ACE_OS::gettimeofday();
	_sub_tot.clear();
	BASE::R_Vec r_vec(1, _tot);
	update_data(r_vec);
	_writer.close_os();
	_trades_writer.close_os();
	_writer_tot.close_os();
	_trades_writer_tot.close_os();
	for (SlopeWriterMap::iterator i = _slope_writers.begin(); i != _slope_writers.end(); ++i)
		(*i).second->close_os();
}
//======================================================================
void Cp_BatchView::Ctrl1::process_result(LFTradeStats* result_)
{
	BASE::R_Ptr p(new LFTradeStats(*result_));

	LFTradeStats& tdata = *_tot;
	LFTradeStats& pdata = *p;

	tdata._min_realized_pnl = std::min(tdata._min_realized_pnl, pdata._min_realized_pnl);
	tdata._max_realized_pnl = std::max(tdata._max_realized_pnl, pdata._max_realized_pnl);
	tdata._min_unrealized_pnl = std::min(tdata._min_unrealized_pnl, pdata._min_unrealized_pnl);
	tdata._max_unrealized_pnl = std::max(tdata._max_unrealized_pnl, pdata._max_unrealized_pnl);
	tdata._min_pnl = std::min(tdata._min_pnl, pdata._pnl);
	tdata._max_pnl = std::max(tdata._max_pnl, pdata._pnl);

	tdata._open_max++;
	if (pdata._pnl > 0)
	{
		tdata._realized_pnl += pdata._pnl;
	}
	else if (pdata._pnl < 0)
	{
		tdata._unrealized_pnl += pdata._pnl;
		tdata._num_sells--;
	}

	tdata._num_buys = (tdata._open_max + tdata._num_sells)*100. / tdata._open_max;

	tdata._pnl += pdata._pnl;
	tdata._open = tdata._realized_pnl*100. / (tdata._realized_pnl - tdata._unrealized_pnl);

	BASE::R_Vec r_vec(2, _tot);
	r_vec[1] = p;

	update_data(r_vec);
}
//======================================================================
void Cp_BatchView::Ctrl1::init()
{
	mark_for_delete();
	do_delete();
	_tot.reset(new LFTradeStats);
	_tot->_t = ACE_OS::gettimeofday();
	_tot->_s_t_str.clear();
	BASE::R_Vec r_vec(1, _tot);
	update_data(r_vec);

}
//======================================================================
void Cp_BatchView::OnBnClickedBatch()
{
	LFRunEvent rd;
	rd._run_stats._run = 0;
	std::vector<LFRunEvent*> rdv;
	rd.init_backtest(rdv);

	if(rdv.size() == 0)
		return;

	_ctrl->init(_ctrl1.get());
	_timer = ::SetTimer(0,0, 1000, batch_timer_proc);

	BMTS<LFRunEvent>::instance()->_table = TCDateTime::now().as_string() + "_" + Z::get_app_name();
	BMTS<LFTotalEvent>::instance()->_table = TCDateTime::now().as_string() + "_" + Z::get_app_name();
	BMTS<LFMatch>::instance()->_table = BMTS<LFRunEvent>::instance()->_table;
	BMTS<LFTradeStats>::instance()->_table = BMTS<LFRunEvent>::instance()->_table;
	BMTS<LFDrawDown>::instance()->_table = BMTS<LFRunEvent>::instance()->_table;

	std::string dir = TCFiler::get_dir(
				TCFiler::get_run_dir(BMTS<LFRunEvent>::instance()->_run_dir, 0, true),
				BMTS<LFRunEvent>::instance()->_table, true);

	TCResourceMap	mmap;
	std::string base = "LFModel:";
	TCSettings::get_resource_match(base + "*", mmap);
	
	std::string fname = dir + "/model.cfg";
	std::ofstream l_stream(fname.c_str());
	if(l_stream.good())
	{
		for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
			l_stream << base << (*i).second->get_sub_name(1) << "\t" << (*i).second->get_value() << std::endl;
	}


	Z::register_reply_handler(this, &Cp_BatchView::process_result);
	for(size_t i = 0; i < rdv.size(); ++i)
	{
		Z::report_debug("zcall -- LFRunData[%d]", i);
		Z::z_call(rdv[i]);
	}
	GetDlgItem(IDBATCH)->EnableWindow(FALSE);
}
//======================================================================
