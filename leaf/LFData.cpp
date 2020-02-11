/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFData.h"
#include <cstdlib> 
#include <set>
#include <cfloat>
#include <iomanip>
//======================================================================
//======================================================================
LFStrategyState::LFStrategyState()
	:
	_fume(0),
	_exit(0),
	_cur_profit_ticks(0),
	_max_profit_ticks(0),
	_i_count(0),
	_l_1("leader", "1"),
	_f_1("follower", "1")

{
	_m_vec.push_back(&_l_1);
	_m_vec.push_back(&_f_1);
}
//======================================================================
LFStrategyState::~LFStrategyState()
{
}

//======================================================================
//======================================================================
const char* LFStrategyData::header()
{
	return ",S_KEY,FUME,EXIT,MIDP,\
F1_T,F1_P,F1_M,F1_ENG,F1_M_ID,F1_M_SIDE,F1_M_M,F1_M_V,F1_M_ENGV,F1_ENDP,\
L1_T,L1_P,L1_M,L1_ENG,L1_M_ID,L1_M_SIDE,L1_M_M,L1_M_V,L1_M_ENGV,L1_ENDP";
}


//======================================================================
std::ostream& operator<<(std::ostream& os, const LFStrategyData& d_)
{
	d_.to_stream(os);

	os	<< "," << d_._strategy_key;

	os << "," << d_._fume ;
	os << "," << d_._exit ;

	os << "," << d_._f_1;
	os << "," << d_._l_1;
	return os;
}


//======================================================================
void LFStrategyData::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 38)
		z_throw("LFStrategyData::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_fume = atof(v_[ind++]);
	_exit = atof(v_[ind++]);
	_l_1.from_csv_vec(ind, v_);
	_f_1.from_csv_vec(ind, v_);
}
//======================================================================
LFStrategyData::LFStrategyData()
:
_fume(0),
_exit(0)

{
}
//======================================================================
LFStrategyData::LFStrategyData(const LFStrategyState& s_)
	:
	_strategy_key(s_._strategy_key),
	_fume(s_._fume),
	_exit(s_._exit),
	_f_1(s_._f_1),
	_l_1(s_._l_1),
	_exec(s_._exec)

{
	_t = s_._t;
}
//======================================================================
LFStrategyData::~LFStrategyData()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFStrategyData& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._fume)) 
	  return false;
	if (!(strm << d_._exit)) 
	  return false;
	if (!(strm << d_._l_1)) 
	  return false;
	if (!(strm << d_._f_1)) 
	  return false;

	 return true;
}
 
bool operator>>(ACE_InputCDR& strm, LFStrategyData& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._fume)) 
	  return false;
	if (!(strm >> d_._exit)) 
	  return false;
	if (!(strm >> d_._l_1)) 
	  return false;
	if (!(strm >> d_._f_1)) 
	  return false;

	return true;
}

std::string LFStrategyData::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_key << " >" << _fume;
	return os.str();
}

//======================================================================
//======================================================================
//======================================================================
std::string LF::make_strategy_key(long strategy_id_, const std::string& instr_key_)
{
	std::ostringstream os;
	os << strategy_id_ << "_" << instr_key_;
	return os.str();
}
std::string LF::make_sub_strategy_key(long strategy_id_, long sub_strategy_id_, const std::string& instr_key_)
{
	std::ostringstream os;
	os << strategy_id_ << "_" << sub_strategy_id_ << "_"<< instr_key_;
	return os.str();
}
std::string LF::make_strategy_key(const std::string& strategy_id_, const std::string& instr_key_)
{
	std::string s;
	s = strategy_id_ + "_" + instr_key_;
	return s;
}
std::string LF::make_sub_strategy_key(const std::string& strategy_id_, long sub_strategy_id_, const std::string& instr_key_)
{
	std::ostringstream os;
	os << strategy_id_ << "_" << sub_strategy_id_ << "_"<< instr_key_;
	return os.str();
}
//======================================================================
//======================================================================
const char* LFNewOrder::header()
{
	return ",S_KEY,ORDER_ID,STRATEGY_ID,SUBSTRATEGY_ID,INSTR,INSTR_ID,ACCOUNT_ID,TYPE,SIDE,Q,LIMIT_P,STOP_P";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFNewOrder& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._order_id
		 << "," << d_._strategy_id
		 << "," << d_._sub_strategy_id
		 << "," << d_._instr
		 << "," << d_._cqg_InstrumentID
		 << "," << d_._cqg_AccountID;
		 
	os  << "," << (long)d_._order_type
		 << "," << (long)d_._order_side
		 << "," << d_._q
		 << "," << d_._limit_p
		 << "," << d_._stop_p;

	return os;
}
//======================================================================
void LFNewOrder::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 11)
		z_throw("LFNewOrder::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_order_id = atol(v_[ind++]);
	_strategy_id = atol(v_[ind++]);
	_sub_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_cqg_InstrumentID = atol(v_[ind++]);
	_cqg_AccountID = atol(v_[ind++]);

	_order_type = LF::OrderType(atol(v_[ind++]));
	_order_side = LF::Side(atol(v_[ind++]));
	_q = atol(v_[ind++]);

	_limit_p = atof(v_[ind++]);
	_stop_p = atof(v_[ind++]);
}
//======================================================================
LFNewOrder::LFNewOrder()
:	_order_id(0),
	_strategy_id(0),
	_sub_strategy_id(0),
	_cqg_InstrumentID(-1),
	_cqg_AccountID(-1),
	_order_type(LF::otUndefined),
	_order_side(LF::s_UNKNOWN),
	_q(0),
	_limit_p(0),
	_stop_p(0)
{
}
//======================================================================
LFNewOrder::~LFNewOrder()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFNewOrder& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._order_id)) 
	  return false;
	if (!(strm << d_._strategy_id)) 
	  return false;
	if (!(strm << d_._sub_strategy_id)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm << d_._cqg_AccountID)) 
	  return false;

	if(!(strm << (long)d_._order_type))
		return false;
	if(!(strm << (long)d_._order_side))
		return false;
	if(!(strm << d_._q))
		return false;
	if(!(strm << d_._limit_p))
		return false;
	if(!(strm << d_._stop_p))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFNewOrder& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._order_id)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	if (!(strm >> d_._sub_strategy_id)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm >> d_._cqg_AccountID)) 
	  return false;
	long ot = 0;
	if(!(strm >> ot))
		return false;
	d_._order_type = LF::OrderType(ot);
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._order_side = LF::Side(os);
	if(!(strm >> d_._q))
		return false;
	if(!(strm >> d_._limit_p))
		return false;
	if(!(strm >> d_._stop_p))
		return false;
	return true;
}
std::string LFNewOrder::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id << "_" << _order_id
		<< "(" << _instr << ") (" << _order_type << ") " 
		<<  _order_side << " " << _q << "@" << _limit_p << "(" << _stop_p << ")";
	return os.str();
}
//======================================================================
//======================================================================
//======================================================================
const char* LFCancelOrder::header()
{
	return ",S_KEY,ORDER_ID,STRATEGY_ID,SUBSTRATEGY_ID,INSTR,INSTR_ID,ACCOUNT_ID,ORIG_ORDER_ID,GW_ORDER_ID,SIDE";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFCancelOrder& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._order_id
		 << "," << d_._strategy_id
		 << "," << d_._sub_strategy_id
		 << "," << d_._instr
		 << "," << d_._cqg_InstrumentID
		 << "," << d_._cqg_AccountID
		 << "," << d_._cqg_OriginalOrderId
		 << "," << d_._cqg_GWOrderID;
		 
	os   << "," << (long)d_._order_side;

	return os;
}
//======================================================================
void LFCancelOrder::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 7)
		z_throw("LFCancelOrder::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_order_id = atol(v_[ind++]);
	_strategy_id = atol(v_[ind++]);
	_sub_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_cqg_InstrumentID = atol(v_[ind++]);
	_cqg_AccountID = atol(v_[ind++]);
	_cqg_OriginalOrderId = v_[ind++];
	_cqg_GWOrderID = v_[ind++];

	_order_side = LF::Side(atol(v_[ind++]));
}
//======================================================================
LFCancelOrder::LFCancelOrder()
:	_order_id(0),
	_strategy_id(0),
	_sub_strategy_id(0),
	_cqg_InstrumentID(-1),
	_cqg_AccountID(-1),
	_order_side(LF::s_UNKNOWN)
{
}
//======================================================================
LFCancelOrder::~LFCancelOrder()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFCancelOrder& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._order_id)) 
	  return false;
	if (!(strm << d_._strategy_id)) 
	  return false;
	if (!(strm << d_._sub_strategy_id)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm << d_._cqg_AccountID)) 
	  return false;
	if (!(strm << d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm << d_._cqg_GWOrderID)) 
	  return false;

	if(!(strm << (long)d_._order_side))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFCancelOrder& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._order_id)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	if (!(strm >> d_._sub_strategy_id)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm >> d_._cqg_AccountID)) 
	  return false;
	if (!(strm >> d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm >> d_._cqg_GWOrderID)) 
	  return false;
	
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._order_side = LF::Side(os);
	return true;
}
std::string LFCancelOrder::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id << "_" << _order_id << "(" << _instr << ")";
	return os.str();
}
//======================================================================
//======================================================================
//======================================================================
const char* LFOrderUpdate::header()
{
	return ",S_KEY,ORDER_ID,STRATEGY_ID,SUBSTRATEGY_ID,INSTR,INSTR_ID,ACCOUNT_ID,ORIG_ORDER_ID,GW_ORDER_ID,TYPE,SIDE,Q,LIMIT_P,STOP_P,STATUS,FILLED_Q,AVG_P,\
		   REMAIN_Q,USER,TRADER,ERR_CODE,ERR_DESC,FILLS";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFOrderUpdate& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._order_id
		 << "," << d_._strategy_id
		 << "," << d_._sub_strategy_id
		 << "," << d_._instr
		 << "," << d_._cqg_InstrumentID
		 << "," << d_._cqg_AccountID
		 << "," << d_._cqg_OriginalOrderId
		 << "," << d_._cqg_GWOrderID;

	os  << "," << (long)d_._order_type
		 << "," << (long)d_._order_side
		 << "," << d_._q
		 << "," << d_._limit_p
		 << "," << d_._stop_p
		 << "," << (long)d_._order_status
		 << "," << d_._filled_q
		 << std::fixed << std::setprecision(LFSecurityMaster::instance()->get_iprecision())
		 << "," << d_._avg_fill_p
		 << "," << d_._remain_q
		 << "," << d_._user_name
		 << "," << d_._trader_id
		 << "," << d_._err_code
		 << "," << d_._err_desc;

	os  << "," << d_._fills.size();

	return os;
}
//======================================================================
void LFOrderUpdate::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 19)
		z_throw("LFOrderUpdate::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_order_id = atol(v_[ind++]);
	_strategy_id = atol(v_[ind++]);
	_sub_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_cqg_InstrumentID = atol(v_[ind++]);
	_cqg_AccountID = atol(v_[ind++]);
	_cqg_OriginalOrderId = v_[ind++];
	_cqg_GWOrderID = v_[ind++];

	_order_type = LF::OrderType(atol(v_[ind++]));
	_order_side = LF::Side(atol(v_[ind++]));
	_q = atol(v_[ind++]);

	_limit_p = atof(v_[ind++]);
	_stop_p = atof(v_[ind++]);
	_order_status = LF::OrderStatus(atol(v_[ind++]));
	_filled_q = atol(v_[ind++]);
	_avg_fill_p = atof(v_[ind++]);
	_remain_q = atol(v_[ind++]);
	_user_name = v_[ind++];
	_trader_id = atol(v_[ind++]);
	_err_code = atol(v_[ind++]);
	_err_desc = v_[ind++];
}
//======================================================================
LFOrderUpdate::LFOrderUpdate()
:	_order_id(0),
	_strategy_id(0),
	_sub_strategy_id(0),
	_cqg_InstrumentID(-1),
	_cqg_AccountID(-1),
	_order_type(LF::otUndefined),
	_order_side(LF::s_UNKNOWN),
	_q(0),
	_limit_p(0),
	_stop_p(0),
	_order_status(LF::osNotSent),
	_filled_q(0),
	_avg_fill_p(0),
	_remain_q(0),
	_trader_id(0),
	_err_code(0)
{
}
LFOrderUpdate::LFOrderUpdate(const LFNewOrder& req_)
:	Z::Point(req_._t),
	_order_id(req_._order_id),
	_strategy_id(req_._strategy_id),
	_sub_strategy_id(req_._sub_strategy_id),
	_strategy_key(req_._strategy_key),
	_instr(req_._instr),
	_cqg_InstrumentID(req_._cqg_InstrumentID),
	_cqg_AccountID(req_._cqg_AccountID),
	_order_type(req_._order_type),
	_order_side(req_._order_side),
	_q(req_._q),
	_limit_p(req_._limit_p),
	_stop_p(req_._stop_p),
	_order_status(LF::osNotSent),
	_filled_q(0),
	_avg_fill_p(0),
	_remain_q(req_._q),
	_trader_id(0),
	_err_code(0)
{
}
//======================================================================
LFOrderUpdate::~LFOrderUpdate()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFOrderUpdate& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._order_id)) 
	  return false;
	if (!(strm << d_._strategy_id)) 
	  return false;
	if (!(strm << d_._sub_strategy_id)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm << d_._cqg_AccountID)) 
	  return false;
	if (!(strm << d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm << d_._cqg_GWOrderID)) 
	  return false;

	if(!(strm << (long)d_._order_type))
		return false;
	if(!(strm << (long)d_._order_side))
		return false;
	if(!(strm << d_._q))
		return false;
	if(!(strm << d_._limit_p))
		return false;
	if(!(strm << d_._stop_p))
		return false;
	if(!(strm << (long)d_._order_status))
		return false;
	if(!(strm << d_._filled_q))
		return false;
	if(!(strm << d_._avg_fill_p))
		return false;
	if(!(strm << d_._remain_q))
		return false;
	if(!(strm << d_._user_name))
		return false;
	if(!(strm << d_._trader_id))
		return false;
	if(!(strm << d_._err_code))
		return false;
	if(!(strm << d_._err_desc))
		return false;
	if(!(strm << d_._fills))
		return false;


	return true;
}
bool operator>>(ACE_InputCDR& strm, LFOrderUpdate& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._order_id)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	if (!(strm >> d_._sub_strategy_id)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm >> d_._cqg_AccountID)) 
	  return false;
	if (!(strm >> d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm >> d_._cqg_GWOrderID)) 
	  return false;

	long ot = 0;
	if(!(strm >> ot))
		return false;
	d_._order_type = LF::OrderType(ot);
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._order_side = LF::Side(os);
	if(!(strm >> d_._q))
		return false;
	if(!(strm >> d_._limit_p))
		return false;
	if(!(strm >> d_._stop_p))
		return false;
	long ost = 0;
	if(!(strm >> ost))
		return false;
	d_._order_status = LF::OrderStatus(ost);
	if(!(strm >> d_._filled_q))
		return false;
	if(!(strm >> d_._avg_fill_p))
		return false;
	if(!(strm >> d_._remain_q))
		return false;
	if(!(strm >> d_._user_name))
		return false;
	if(!(strm >> d_._trader_id))
		return false;
	if(!(strm >> d_._err_code))
		return false;
	if(!(strm >> d_._err_desc))
		return false;
	if(!(strm >> d_._fills))
		return false;
	return true;
}
std::string LFOrderUpdate::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id << "_" << _order_id << " (" << _instr
		<< ") " 
		<< 	_q << "@" << _limit_p << " [" << _filled_q << "@" << _avg_fill_p 
		<< "] status=" << _order_status << "err=" << _err_code << " " << _err_desc;
	return os.str();

}
//======================================================================
//======================================================================
//======================================================================
const char* LFOrderFill::header()
{
	return ",S_KEY,ORDER_ID,STRATEGY_ID,SUBSTRATEGY_ID,INSTR,INSTR_ID,ACCT,ORIG_ORDER_ID, GW_ORDER_ID, FILL_P,FILL_Q,FILL_ID,SIDE,STATUS";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFOrderFill& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._order_id
		 << "," << d_._strategy_id
		 << "," << d_._sub_strategy_id
		 << "," << d_._instr
		 << "," << d_._cqg_InstrumentID
		 << "," << d_._cqg_AccountID
		 << "," << d_._cqg_OriginalOrderId
		 << "," << d_._cqg_GWOrderID
		 << "," << d_._fill.p_p() << "," << d_._fill._q;
		 
	os	 << "," << d_._fill_id
		 << "," << (long)d_._side
		<< "," << (long)d_._status;

	return os;
}
//======================================================================
void LFOrderFill::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 10)
		z_throw("LFOrderFill::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_order_id = atol(v_[ind++]);
	_strategy_id = atol(v_[ind++]);
	_sub_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_cqg_InstrumentID = atol(v_[ind++]);
	_cqg_AccountID = atol(v_[ind++]);
	_cqg_OriginalOrderId = v_[ind++];
	_cqg_GWOrderID = v_[ind++];
	_fill.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_fill_id = v_[ind++];
	_side = LF::Side(atol(v_[ind++]));
	_status = LF::FillStatus(atol(v_[ind++]));

}
//======================================================================
LFOrderFill::LFOrderFill()
:	_order_id(0),
	_strategy_id(0),
	_sub_strategy_id(0),
	_cqg_InstrumentID(-1),
	_cqg_AccountID(-1),
	_side(LF::s_UNKNOWN),
	_status(LF::fsNormal)
{
}
//======================================================================
LFOrderFill::LFOrderFill(const LFOrderUpdate& req_)
:	Z::Point(req_._t),
	_order_id(req_._order_id),
	_strategy_id(req_._strategy_id),
	_sub_strategy_id(req_._sub_strategy_id),
	_strategy_key(req_._strategy_key),
	_instr(req_._instr),
	_cqg_InstrumentID(req_._cqg_InstrumentID),
	_cqg_AccountID(req_._cqg_AccountID),
	_cqg_OriginalOrderId(req_._cqg_OriginalOrderId),
	_cqg_GWOrderID(req_._cqg_GWOrderID),
	_side(req_._order_side)
{}
//======================================================================
LFOrderFill::~LFOrderFill()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFOrderFill& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._order_id)) 
	  return false;
	if (!(strm << d_._strategy_id)) 
	  return false;
	if (!(strm << d_._sub_strategy_id)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm << d_._cqg_AccountID)) 
	  return false;
	if (!(strm << d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm << d_._cqg_GWOrderID)) 
	  return false;
	if (!(strm << d_._fill)) 
	  return false;
	if (!(strm << d_._fill_id)) 
	  return false;
	if(!(strm << (long)d_._side))
		return false;
	if(!(strm << (long)d_._status))
		return false;
	return true;

}
bool operator>>(ACE_InputCDR& strm, LFOrderFill& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._order_id)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	if (!(strm >> d_._sub_strategy_id)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._cqg_InstrumentID)) 
	  return false;
	if (!(strm >> d_._cqg_AccountID)) 
	  return false;
	if (!(strm >> d_._cqg_OriginalOrderId)) 
	  return false;
	if (!(strm >> d_._cqg_GWOrderID)) 
	  return false;
	if (!(strm >> d_._fill)) 
	  return false;
	if (!(strm >> d_._fill_id)) 
	  return false;
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._side = LF::Side(os);
	long fs = 0;
	if(!(strm >> fs))
		return false;
	d_._status = LF::FillStatus(fs);
	return true;
}
std::string LFOrderFill::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id << "_" << _order_id
		<< " (" << _instr << ") " << _side << " " << _fill;
	return os.str();
}
//======================================================================
//======================================================================
//======================================================================
const char* LFPositionData::header()
{
	return ",S_KEY,STRATEGY_ID,SUBSTRATEGY_ID,INSTR,TICK_SIZE,TICK_VAL,\
SIDE,OPEN_P,OPEN_Q,OPENING_P,OPENING_Q,CLOSING_P,CLOSING_Q,\
P_OPEN_P,P_OPEN_Q,REMAIN_Q,UNREALIZED_TICKS,\
LAST_FILL_P,LAST_FILL_Q,LAST_FILL_ID,LAST_FILL_SIDE,\
REALIZED,UNREALIZED,PNL,STATUS,ACTION,SIGNAL,\
TREND_SIDE,IN_TREND,HARD_P,SOFT_P,\
Q_A,Q_A_I,Q_A_S,Q_S,Q_S_I,Q_P,Q_P_I,\
BOX_CHANGED";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFPositionData& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._strategy_id
		 << "," << d_._sub_strategy_id
		 << "," << d_._instr
		 << "," << d_._tick_size
		 << "," << d_._tick_value
		 << "," << (long)d_._side;
	os 	<< "," << d_._open.p_p() << "," << d_._open._q;
	os	<< "," << d_._opening.p_p() << "," << d_._opening._q;
	os	<< "," << d_._closing.p_p() << "," << d_._closing._q;
	os 	<< "," << d_._p_open.p_p() << "," << d_._p_open._q;
	os 	<< "," << d_._p_remain_q;
	os 	<< "," << d_._unrealized_ticks;
		 
	os	<< "," << d_._last_fill.p_p() << "," << d_._last_fill._q
 		<< "," << d_._last_fill_id
		 << "," << (long)d_._last_fill_side;

	os	<< "," << d_._realized_pnl;
	os	<< "," << d_._unrealized_pnl;
	os	<< "," << d_._pnl;
	os	<< "," << (long)d_._status;
	os	<< "," << (long)d_._action;
	os << "," << d_._signal ;
	os << "," << (long)d_._trend_side;
	os << "," << d_._in_trend?1:0;
	os << "," << d_._trend_hard_p;
	os << "," << d_._trend_soft_p;
	os << "," << d_._q_A;
	os	<< "," << d_._q_A_i;
	os	<< "," << d_._q_A_s;
	os	<< "," << d_._q_S;
	os	<< "," << d_._q_S_i;
	os	<< "," << d_._q_P;
	os	<< "," << d_._q_P_i;
	os << "," << d_._box_changed.to_alfa();
	return os;
}

//======================================================================
void LFPositionData::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 39)
		z_throw("LFPositionData::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_strategy_id = atol(v_[ind++]);
	_sub_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_tick_size = atof(v_[ind++]);
	_tick_value = atof(v_[ind++]);
	_side = LF::Side(atol(v_[ind++]));

	_open.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_opening.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_closing.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_p_open.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_p_remain_q = atof(v_[ind++]);
	_unrealized_ticks = atof(v_[ind++]);

	_last_fill.reset(atof(v_[ind]), atof(v_[ind+1]));
	ind +=2;
	_last_fill_id = v_[ind++];
	_last_fill_side = LF::Side(atol(v_[ind++]));
	_realized_pnl = atof(v_[ind++]);
	_unrealized_pnl = atof(v_[ind++]);
	_pnl = atof(v_[ind++]);
	_status = LF::PositionStatus(atol(v_[ind++]));
	_action = LF::PositionAction(atoi(v_[ind++]));
	_signal = v_[ind++];
	_q_A = atof(v_[ind++]);
	_q_A_i = atof(v_[ind++]);
	_q_A_s = atof(v_[ind++]);
	_q_S = atof(v_[ind++]);
	_q_S_i = atof(v_[ind++]);
	_q_P = atof(v_[ind++]);
	_q_P_i = atof(v_[ind++]);
	_to_trend = atof(v_[ind++]);

}
//======================================================================
LFPositionData::LFPositionData()
:	_strategy_id(0),
	_sub_strategy_id(0),
	_tick_size(0),
	_tick_value(0),
	_side(LF::s_UNKNOWN),
	_p_remain_q(0),
	_unrealized_ticks(0),
	_last_fill_side(LF::s_UNKNOWN),
	_realized_pnl(0),
	_unrealized_pnl(0),
	_pnl(0),
	_status(LF::psActive),
	_action(LF::paUndefined),
	_q_A(0),
	_q_A_i(-1),
	_q_A_s(0),
	_q_S(0),
	_q_S_i(-1),
	_q_P(0),
	_q_P_i(-1),
	_to_trend(0),
	_trend_side(LF::s_UNKNOWN),
	_in_trend(false),
	_trend_hard_p(0),
	_trend_soft_p(0)

{
}
//======================================================================
LFPositionData::LFPositionData(long strategy_id_, const std::string& instr_, long sub_strategy_id_)
:	_strategy_id(strategy_id_),
	_sub_strategy_id(sub_strategy_id_),
	_instr(instr_),
	_tick_size(0),
	_tick_value(0),
	_side(LF::s_UNKNOWN),
	_p_remain_q(0),
	_unrealized_ticks(0),
	_last_fill_side(LF::s_UNKNOWN),
	_realized_pnl(0),
	_unrealized_pnl(0),
	_pnl(0),
	_status(LF::psActive),
	_action(LF::paUndefined),
	_q_A(0),
	_q_A_i(0),
	_q_A_s(0),
	_q_S(0),
	_q_S_i(0),
	_q_P(0),
	_q_P_i(0),
	_to_trend(0),
	_trend_side(LF::s_UNKNOWN),
	_in_trend(false),
	_trend_hard_p(0),
	_trend_soft_p(0)
{
	if(_sub_strategy_id == 0)
		_strategy_key = LF::make_strategy_key(_strategy_id, _instr);
	else
		_strategy_key = LF::make_sub_strategy_key(_strategy_id, _sub_strategy_id, _instr);
}
//======================================================================
LFPositionData::~LFPositionData()
{
}
//======================================================================
void LFPositionData::reset()
{
	_t = ACE_Time_Value::zero;
	_run = 0;
	_tick_size = 0;
	_tick_value = 0;
	_side = LF::s_UNKNOWN;
	_open.reset();
	_opening.reset();
	_closing.reset();
	_p_open.reset();
	_p_remain_q = 0;
	_unrealized_ticks = 0;
	_last_fill.reset();
	_last_fill_id = "";
	_last_fill_side = LF::s_UNKNOWN;
	_realized_pnl = 0;
	_unrealized_pnl = 0;
	_pnl = 0;
	_status = LF::psActive;
	_action = LF::paUndefined;
	_signal.clear();
	_last_exec.clear();
	_q_A = 0;
	_q_A_s = 0;
	_q_S = 0;
	_q_P = 0;
	_to_trend = 0;
	_box_trends.reset();
	_lcy_trends.reset();
	_trend_side = LF::s_UNKNOWN;
	_in_trend = false;
	_trend_hard_p = 0;
	_trend_soft_p = 0;

	_bias_bc.reset();
	_bias_bx.reset();

	reset_user_input();
	_box_changed.reset();
}
void LFPositionData::reset_user_input()
{
	_q_A_i = 0;
	_q_S_i = 0;
	_q_P_i = 0;
}
bool LFPositionData::is_position_equal(const LFPositionData& s_)
{
	return	_side == s_._side &&
			_p_open == s_._p_open &&
			_realized_pnl == s_._realized_pnl;

}
//======================================================================
std::string LFPositionData::BoxChanged::to_alfa() const
{
	std::string s;

	s += test(LFPositionData::bc_TRENDS) ? 'T' : '_';
	s += test(LFPositionData::bc_LCY) ? 'L' : '_';
	s += test(LFPositionData::bc_BP) ? 'B' : '_';
	s += test(LFPositionData::bc_BX) ? 'X' : '_';
	return s;
}
//======================================================================
bool LFPositionData::is_strategy_equal(const LFPositionData& s_)
{
	_box_changed.reset();
	bool is_equal = 
		_tick_size == s_._tick_size &&
		_status == s_._status &&
		_action == s_._action &&
		_signal == s_._signal &&
		_q_A == s_._q_A &&
		_q_A_i == s_._q_A_i &&
		_q_A_s == s_._q_A_s &&
		_q_S == s_._q_S &&
		_q_S_i == s_._q_S_i &&
		_q_P == s_._q_P &&
		_q_P_i == s_._q_P_i
		&& _trend_side == s_._trend_side
		&& _in_trend == s_._in_trend
		&& _trend_hard_p == s_._trend_hard_p
		&& _trend_soft_p == s_._trend_soft_p
		;
	_box_changed.set(bc_TRENDS, !(_box_trends == s_._box_trends));
	_box_changed.set(bc_LCY, !(_lcy_trends == s_._lcy_trends));
	_box_changed.set(bc_BP, !_bias_bc.t_equal(s_._bias_bc) || _bias_bc._comment != s_._bias_bc._comment);
	_box_changed.set(bc_BX, !_bias_bx.t_equal(s_._bias_bx));
	return is_equal && _box_changed.none();

}

//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFPositionData& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._strategy_key)) 
	  return false;
	if (!(strm << d_._strategy_id)) 
	  return false;
	if (!(strm << d_._sub_strategy_id)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._tick_size)) 
	  return false;
	if (!(strm << d_._tick_value)) 
	  return false;
	if(!(strm << (long)d_._side))
		return false;
	if (!(strm << d_._open)) 
	  return false;
	if (!(strm << d_._opening)) 
	  return false;
	if (!(strm << d_._closing)) 
	  return false;
	if (!(strm << d_._p_open)) 
	  return false;
	if (!(strm << d_._p_remain_q)) 
	  return false;
	if (!(strm << d_._unrealized_ticks)) 
	  return false;
	if (!(strm << d_._last_fill)) 
	  return false;
	if (!(strm << d_._last_fill_id)) 
	  return false;
	if(!(strm << (long)d_._last_fill_side))
		return false;
	if(!(strm << d_._realized_pnl))
		return false;
	if(!(strm << d_._unrealized_pnl))
		return false;
	if(!(strm << d_._pnl))
		return false;
	if(!(strm << (long)d_._status))
		return false;
	if(!(strm << (long)d_._action))
		return false;
	if(!(strm << d_._signal))
		return false;
	if (!(strm << d_._last_exec))
		return false;
	if (!(strm << d_._q_A))
		return false;
	if(!(strm << d_._q_A_i))
		return false;
	if(!(strm << d_._q_A_s))
		return false;
	if(!(strm << d_._q_S))
		return false;
	if(!(strm << d_._q_S_i))
		return false;
	if(!(strm << d_._q_P))
		return false;
	if(!(strm << d_._q_P_i))
		return false;
	if(!(strm << d_._to_trend))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFPositionData& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	if (!(strm >> d_._sub_strategy_id)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._tick_size)) 
	  return false;
	if (!(strm >> d_._tick_value)) 
	  return false;
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._side = LF::Side(os);
	if (!(strm >> d_._open)) 
	  return false;
	if (!(strm >> d_._opening)) 
	  return false;
	if (!(strm >> d_._closing)) 
	  return false;
	if (!(strm >> d_._p_open)) 
	  return false;
	if (!(strm >> d_._p_remain_q)) 
	  return false;
	if (!(strm >> d_._unrealized_ticks)) 
	  return false;
	if (!(strm >> d_._last_fill)) 
	  return false;
	if (!(strm >> d_._last_fill_id)) 
	  return false;
	long fs = 0;
	if(!(strm >> fs))
		return false;
	d_._last_fill_side = LF::Side(fs);
	if (!(strm >> d_._realized_pnl)) 
	  return false;
	if (!(strm >> d_._unrealized_pnl)) 
	  return false;
	if (!(strm >> d_._pnl)) 
	  return false;
	long ps = 0;
	if(!(strm >> ps))
		return false;
	d_._status = LF::PositionStatus(ps);
	long pa = 0;
	if (!(strm >> pa)) 
	  return false;
	d_._action = LF::PositionAction(pa);
	if (!(strm >> d_._signal)) 
	  return false;
	if (!(strm >> d_._last_exec))
		return false;
	if (!(strm >> d_._q_A))
	  return false;
	if (!(strm >> d_._q_A_i)) 
	  return false;
	if (!(strm >> d_._q_A_s)) 
	  return false;
	if (!(strm >> d_._q_S)) 
	  return false;
	if (!(strm >> d_._q_S_i)) 
	  return false;
	if (!(strm >> d_._q_P)) 
	  return false;
	if (!(strm >> d_._q_P_i)) 
	  return false;
	if (!(strm >> d_._to_trend)) 
	  return false;
	return true;
}

std::string LFPositionData::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id 
		<< " (" << _instr << ") " << _side << " " << _open 
		<< "pnl=[" << _realized_pnl << ", " << _unrealized_pnl  << ", " << _pnl << "]";
	return os.str();
}
//======================================================================
void LFPositionData::add_fill(const LFOrderFill& of_)
{
	if(!of_._fill.good())
		return;
	_t = of_.get_t();
	_last_fill_id = of_._fill_id;
	_last_fill = of_._fill;
	_last_fill_side = of_._side;

	if(_side == LF::s_UNKNOWN || _open._q == 0)
	{
		_side = _last_fill_side;
		_open = _last_fill;
	}
	else if(_side == _last_fill_side)
	{
		_open += _last_fill;
	}
	else
	{
		double q_m = std::min(_open._q, _last_fill._q);
		if(_tick_size == 0)
			z_throw("LFPositionData::add_fill -- tick size is not set");
		_realized_pnl += q_m * LFSide::s(_side)->b_diff(_open._p, _last_fill._p) * _tick_value / _tick_size;
		_realized_pnl = TCCompare_10000::z(_realized_pnl);
		if(_open._q > _last_fill._q)
		{
			_open._q -= _last_fill._q;
		}
		else if(_open._q < _last_fill._q)
		{
			_open.reset(_last_fill._p, _last_fill._q  - _open._q);
			_side = _last_fill_side;
		}
		else
		{
			_open.reset();
			_side = LF::s_UNKNOWN;
		}
	}

	// invalidate pnl
	_unrealized_ticks = 0;
	_unrealized_pnl = 0;
	_pnl = 0;
}
//======================================================================
double LFPositionData::project_avg_p(const LFSide* s_, const TC_pqp& f_)
{
	if(_side == LF::s_UNKNOWN || _open._q == 0)
		return f_._p;
	if(_side != s_->value())
		return _open._p;
	return (_open + f_)._p;
}

//======================================================================
void LFPositionData::mark_to_market(const LFDepthData& md_)
{
	if(_tick_size == 0)
	{
		_tick_size = md_._tick_size;
		_tick_value = md_._tick_value;
	}
	if(_t < md_.get_t())
		_t = md_.get_t();
	_unrealized_pnl = 0;
	_unrealized_ticks = 0;
	_pnl = 0;
	if(_side != LF::s_UNKNOWN && _open._q > 0)
	{
		const LFSide* s = LFSide::s(_side);
		_unrealized_ticks =s->b_diff(_open._p, s->same_p(md_)) / _tick_size;
		_unrealized_pnl = _open._q * _unrealized_ticks * _tick_value;
		_unrealized_pnl = TCCompare_10000::z(_unrealized_pnl);
	}
	_pnl = _realized_pnl + _unrealized_pnl;

}
//======================================================================
void LFPositionData::strat_to_market(const LFDepthData& md_)
{
	_to_trend = 0;
	const LFSide* s = LFSide::s(_trend_side);
	if (s != 0)
	{
		if(_in_trend && _trend_soft_p != 0)
			_to_trend = s->v(_trend_soft_p, s->same_p(md_), _tick_size);
		else
			_to_trend = s->v(_trend_hard_p, s->same_p(md_), _tick_size);
	}

}
//======================================================================
//======================================================================
//======================================================================
const char* LFInfo_Position::header()
{
	return ",S_KEY,STRATEGY_ID,INSTR,SIDE,OPEN_P,OPEN_Q,COM";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFInfo_Position& d_)
{
	d_.to_stream(os);

	os << "," << d_._strategy_key;
	os << "," << d_._strategy_id;
	os << "," << d_._instr;
	os << "," << (long)d_._side;
	os << "," << d_._open.p_p() << "," << d_._open._q;
	os << "," << d_._comment;
	return os;
}

//======================================================================
void LFInfo_Position::from_csv_vec(const std::vector<const char*>& v_)
{
	if (v_.size() < Z::Point::TIME_FIELDS + 7)
		z_throw("LFInfo_Position::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_strategy_id = atol(v_[ind++]);
	_instr = v_[ind++];
	_side = LF::Side(atol(v_[ind++]));

	_open.reset(atof(v_[ind]), atof(v_[ind + 1]));
	ind += 2;
	_comment = v_[ind++];
}
//======================================================================
LFInfo_Position::LFInfo_Position()
	: _strategy_id(0),
	_side(LF::s_UNKNOWN)
{
	reset();
}
//======================================================================
LFInfo_Position::LFInfo_Position(long strategy_id_, const std::string& instr_)
	: _strategy_id(strategy_id_),
	_instr(instr_),
	_side(LF::s_UNKNOWN)
{
	_strategy_key = LF::make_strategy_key(_strategy_id, _instr);
	reset();
}
//======================================================================
LFInfo_Position::LFInfo_Position(const LFPositionData& pdata_)
	:_strategy_key(pdata_._strategy_key),
	_strategy_id(pdata_._strategy_id),
	_instr(pdata_._instr),
	_side(pdata_._side),
	_open(pdata_._open)
{
	_run = BMModel::instance()->run_num();
	_t = pdata_._t;
}

//======================================================================
LFInfo_Position::~LFInfo_Position()
{
}
//======================================================================
void LFInfo_Position::reset()
{
	_t = ACE_Time_Value::zero;
	_run = BMModel::instance()->run_num();
	_side = LF::s_UNKNOWN;
	_open.reset();
	_comment.clear();
}
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFInfo_Position& d_)
{
	if (!d_.to_cdr(strm))
		return false;
	if (!(strm << d_._strategy_key))
		return false;
	if (!(strm << d_._strategy_id))
		return false;
	if (!(strm << d_._instr))
		return false;
	if (!(strm << (long)d_._side))
		return false;
	if (!(strm << d_._open))
		return false;
	if (!(strm << d_._comment))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFInfo_Position& d_)
{
	if (!d_.from_cdr(strm))
		return false;
	if (!(strm >> d_._strategy_key))
		return false;
	if (!(strm >> d_._strategy_id))
		return false;
	if (!(strm >> d_._instr))
		return false;
	long os = 0;
	if (!(strm >> os))
		return false;
	d_._side = LF::Side(os);
	if (!(strm >> d_._open))
		return false;
	if (!(strm >> d_._comment))
		return false;
	return true;
}
//======================================================================
//======================================================================
const char* LFEntry::header()
{
	return ",S_KEY,S_T,COMMENT,SIDE,SEQ,MAX_Q,\
OPEN_P,OPEN_Q,E_P,E_Q,X_P,X_Q,L_TICKS,U_TICKS,R_PNL,PRE_R_PNL,S_CONF,U_PNL,PNL,CL_P,CL_Q,CL_TICKS,OLD_AVG,TP_P,TP_Q";
}

//======================================================================
void LFEntry::body_to_stream(std::ostream& os) const
{
	os << "," << _key;
	os << "," << _s_t_str;
	os << "," << _comment ;
	os << "," << _side ;
	os << "," << _seq;	
	os << "," << _max_q;	
	os << "," << _open.p_p();
	os << "," << _open._q;
	os << "," << _enter.p_p();
	os << "," << _enter._q;
	os << "," << _exit.p_p();
	os << "," << _exit._q;
	os << "," << _last_ticks;
	os << "," << _unrealized_ticks;
	os << "," << _realized_pnl;
	os << "," << _pre_realized_pnl;
	os << "," << _side_confirmed?1:0;
	os << "," << _unrealized_pnl;
	os << "," << _pnl;
	os << "," << _cutloss.p_p();
	os << "," << _cutloss._q;
	os << "," << _cutloss_ticks;
	os << "," << _old_average_logic?1:0;
	os << "," << _takeprofit.p_p();
	os << "," << _takeprofit._q;
		
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFEntry& d_)
{

	d_.to_stream(os);
	d_.body_to_stream(os);

	return os;

}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFEntry& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;

	if (!(strm << d_._key)) 
	  return false;
	if (!(strm << d_._side)) 
	  return false;
	if (!(strm << d_._s_t)) 
	  return false;
	if (!(strm << d_._s_t_str)) 
	  return false;
	if (!(strm << d_._seq))
		return false;
	if (!(strm << d_._trend_scale))
		return false;
	if (!(strm << d_._max_q)) 
	  return false;
	if (!(strm << d_._open)) 
	  return false;
	if (!(strm << d_._enter)) 
	  return false;
	if (!(strm << d_._exit)) 
	  return false;
	if (!(strm << d_._cutloss)) 
	  return false;
	if (!(strm << d_._takeprofit)) 
	  return false;
	if (!(strm << d_._cutloss_ticks)) 
	  return false;
	if (!(strm << d_._last_ticks)) 
	  return false;
	if (!(strm << d_._unrealized_ticks)) 
	  return false;
	if (!(strm << d_._realized_pnl)) 
	  return false;
	if (!(strm << d_._pre_realized_pnl)) 
	  return false;
	if (!(strm << d_._side_confirmed)) 
	  return false;
	if (!(strm << d_._unrealized_pnl)) 
	  return false;
	if (!(strm << d_._pnl)) 
	  return false;
	if (!(strm << d_._comment)) 
	  return false;

	return true;
}
bool operator>>(ACE_InputCDR& strm, LFEntry& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._key)) 
	  return false;
	if (!(strm >> d_._side)) 
	  return false;
	if (!(strm >> d_._s_t)) 
	  return false;
	if (!(strm >> d_._s_t_str)) 
	  return false;
	if (!(strm >> d_._seq))
		return false;
	if (!(strm >> d_._trend_scale))
		return false;
	if (!(strm >> d_._max_q)) 
	  return false;
	if (!(strm >> d_._open)) 
	  return false;
	if (!(strm >> d_._enter)) 
	  return false;
	if (!(strm >> d_._exit)) 
	  return false;
	if (!(strm >> d_._cutloss)) 
	  return false;
	if (!(strm >> d_._takeprofit)) 
	  return false;
	if (!(strm >> d_._cutloss_ticks)) 
	  return false;
	if (!(strm >> d_._last_ticks)) 
	  return false;
	if (!(strm >> d_._unrealized_ticks)) 
	  return false;
	if (!(strm >> d_._realized_pnl)) 
	  return false;
	if (!(strm >> d_._pre_realized_pnl)) 
	  return false;
	if (!(strm >> d_._side_confirmed)) 
	  return false;
	if (!(strm >> d_._unrealized_pnl)) 
	  return false;
	if (!(strm >> d_._pnl)) 
	  return false;
	if (!(strm >> d_._comment)) 
	  return false;
	return true;
}
//======================================================================
LFEntry::LFEntry(const std::string& key_)
 : _key(key_),
	_seq(0),
	_trend_scale(LFScopeStore::NUMBER_OF_SCOPES),
	_max_q(0),
	_last_ticks(0),
	_unrealized_ticks(0),
	_realized_pnl(0),
	_pre_realized_pnl(0),
	_side_confirmed(false),
	_unrealized_pnl(0),
	_pnl(0),
	_cutloss_ticks(0),
	_old_average_logic(false)
{
}
//======================================================================
LFEntry::~LFEntry() 
{
}
//======================================================================
//======================================================================
void LFEntry::reset(const LFSide* side_, const ACE_Time_Value& t_)
{
	_side = side_;
	_s_t = t_;
	_s_t_str = TCTimestamp(_s_t).c_str();
	_t = t_;
	_run = BMModel::instance()->run_num();

	_seq = 0;
	_trend_scale = LFScopeStore::NUMBER_OF_SCOPES;
	_max_q = 0;
	_open.reset();
	_enter.reset();
	_exit.reset();
	_cutloss.reset();
	_takeprofit.reset();
	_last_ticks = 0;
	_unrealized_ticks = 0;
	_realized_pnl = 0;
	_pre_realized_pnl = 0;
	_side_confirmed = false;
	_unrealized_pnl = 0;
	_pnl = 0;
	_comment.clear();
}
//======================================================================
void LFEntry::reset(const LFSide* side_, const ACE_Time_Value& t_, const LFPositionData& pd_, double realized_, const LFDepthData& md_, size_t seq_)
{
	reset(side_, t_);
	_seq = seq_;
	_realized_pnl = realized_;
	if(_side->value() == pd_._side && pd_._p_open._q > 0)
	{
		_open = pd_._p_open;
		_max_q = _open._q;
		_pre_realized_pnl = pd_._realized_pnl - _realized_pnl;
		_side_confirmed = true;
	}
	else
	{
		_pre_realized_pnl = pd_._pnl - _realized_pnl;
		_side_confirmed = false;
	}
	mark_to_market(md_);
}
//======================================================================
void LFEntry::mark_to_market(const LFDepthData& md_)
{
	_unrealized_pnl = 0;
	_last_ticks = 0;
	_unrealized_ticks = 0;
	_pnl = 0;
	if(valid() && md_.valid() && _open._q > 0)
	{
		if(_enter.good())
			_last_ticks =_side->b_diff(_enter._p, _side->same_p(md_)) / md_._tick_size;
		_unrealized_ticks =_side->b_diff(_open._p, _side->same_p(md_)) / md_._tick_size;
		_unrealized_pnl = _open._q * _unrealized_ticks * md_._tick_value;
		_unrealized_pnl = TCCompare_10000::z(_unrealized_pnl);
	}
	_pnl = _realized_pnl + _unrealized_pnl;

}
//======================================================================
void LFEntry::enter(double q_, double q_limit_, const LFDepthData& md_, double expected_ticks_, double tp_pct_)
{
	_enter.reset();
	if(!valid() || !md_.valid())
		return;
	double enter_p = _side->contra_p(md_);
	double enter_q = q_;
	if(enter_q + _open._q > q_limit_)
		enter_q = q_limit_ - _open._q;

	set_enter(enter_p, enter_q, md_);
	if(expected_ticks_ <= 0)
		return;
	set_takeprofit(enter_p + _side->sign()*expected_ticks_*md_._tick_size, tp_pct_);

}
//======================================================================
void LFEntry::avg(double expected_ticks_, double risk_p_, double rr_factor_, double q_limit_, const LFDepthData& md_, double tp_pct_)
{
	_enter.reset();
	if(!valid() || !md_.valid() || _pnl >= 0)
		return;
	double enter_p = _side->contra_p(md_);
	double enter_q = 0;
	double risk_v = _side->v(risk_p_, enter_p, md_._tick_size);
	double pnl = _pnl;
	if (_unrealized_ticks >= 0)
		pnl = _realized_pnl;
	if (rr_factor_ < 0)
	{
		// reward_v / pos_v = new_q / tot_q
		// reward = reward_v * pos_q = pos_v * pos_q * new_q / tot_q
		// risk = new_q * risk_v
		// risk = rr_factor * reward
		// risk_v = rr_factor * pos_v * pos_q / tot_q
		//
		// tot_q = rr_factor * pos_q * pos_v / risk_v

		double rrf = -rr_factor_;
		if (risk_v > 0 && pnl < 0)
			enter_q = (long)(-pnl * rrf / risk_v / md_._tick_value + 0.5) - _open._q;

		//double pos_v = _open.good() ? _side->v(enter_p, _open._p, md_._tick_size) : 0;
		//if (risk_v > 0 && pos_v > 0)
		//	enter_q = (long)((rr_factor_ * pos_v / risk_v - 1.) * _open._q + 0.5);
	}
	else if	(expected_ticks_ > 0)
	{
		if (expected_ticks_ >= -_unrealized_ticks)
			pnl = _realized_pnl;

		enter_q = (long)(-pnl / expected_ticks_ / md_._tick_value + 0.5);
		if (!_old_average_logic)
			enter_q -= _open._q;
		if (rr_factor_ > 0 && risk_v > 0 && pnl < 0)
		{
			enter_q = std::min((long)(-pnl * rr_factor_ / risk_v / md_._tick_value + 0.5) - _open._q, enter_q);
		}
	}
	else if (rr_factor_ > 0 && risk_v > 0 && pnl < 0)
	{
		enter_q = (long)(-pnl * rr_factor_ / risk_v / md_._tick_value + 0.5) - _open._q;
	}

	if (enter_q + _open._q > q_limit_)
		enter_q = q_limit_ - _open._q;
	set_enter(enter_p, enter_q, md_);
	if (expected_ticks_ > 0)
		set_takeprofit(enter_p + _side->sign()*expected_ticks_*md_._tick_size, tp_pct_);
}
//======================================================================
void LFEntry::set_enter(double p_, double q_, const LFDepthData& md_)
{
	_enter.reset(p_, q_);
	_open += _enter;
	_max_q = std::max(_max_q, _open._q);
	mark_to_market(md_);
}

void LFEntry::set_exit(double q_, const LFDepthData& md_)
{
	_exit.reset();
	if(!valid() || !_open.good() || !md_.valid())
		return;

	_exit.reset(_side->same_p(md_), std::min(_open._q, q_));

	_realized_pnl += _exit._q * _side->b_diff(_open._p, _exit._p) * md_._tick_value / md_._tick_size;
	_realized_pnl = TCCompare_10000::z(_realized_pnl);
	if(_open._q > q_)
	{
		_open._q -= q_;
	}
	else
	{
		q_ -= _open._q;
		_open.reset();
	}

	mark_to_market(md_);
}

//======================================================================
void LFEntry::exit_pct(double q_pct_, const LFDepthData& md_)
{
	_exit.reset();
	if(!valid() || !_open.good() || !md_.valid())
		return;

	double q = _open._q - (long)(_max_q*(100. - q_pct_)/100. + 0.5);
	if(q > 0)
		set_exit(q, md_);

}
//======================================================================
void LFEntry::exit_cutloss(const LFDepthData& md_)
{
	_exit.reset();
	if(!valid() || !_open.good() || !md_.valid() || !_cutloss.good())
		return;

	if(_side->b_diff(_cutloss._p, _side->same_p(md_))/md_._tick_size >= -_cutloss_ticks)
		return;
	exit_pct(_cutloss._q, md_);

}
//======================================================================
void LFEntry::exit_takeprofit(const LFDepthData& md_)
{
	_exit.reset();
	if(!valid() || !_open.good() || !md_.valid() || !_takeprofit.good())
		return;

	if(_side->b_diff(_takeprofit._p, _side->same_p(md_)) < 0)
		return;
	exit_pct(_takeprofit._q, md_);
}
//======================================================================
bool LFEntry::set_cutloss(double p_, double q_pct_)
{
	TC_pqp old_cl = _cutloss;
	_cutloss.reset(p_, q_pct_);
	if(_cutloss._q > 100)
		_cutloss._q = 100;
	return _cutloss != old_cl;
}
//======================================================================
bool LFEntry::set_takeprofit(double p_, double q_pct_)
{
	TC_pqp old_tp = _takeprofit;
	_takeprofit.reset(p_, q_pct_);
	if(_takeprofit._q > 100)
		_takeprofit._q = 100;
	return _takeprofit != old_tp;
}

//======================================================================
//======================================================================
//======================== LFTradeStats ===================================
//======================================================================
LFTradeStats::LFTradeStats()
{
	reset();
	_run = BMModel::instance()->run_num();
	_run_key = Z::get_setting("LFStrategy:strategy_id") + "_trades";
}
//======================================================================
void LFTradeStats::reset(const ACE_Time_Value& t_, double pre_realized_pnl_)
{
	_t = t_;
	_s_t = t_;
	_s_t_str = TCTimestamp(_s_t).c_str();
	_pre_realized_pnl = pre_realized_pnl_;
	_side = LF::s_UNKNOWN;
	_end_t = ACE_Time_Value::zero;
	_end_t_str.clear();
	_open_max = 0;
	_num_buys = 0;
	_num_sells = 0;
	_min_realized_pnl = 0;
	_max_realized_pnl = 0;
	_min_unrealized_pnl = 0;
	_max_unrealized_pnl = 0;
	_min_pnl = 0;
	_max_pnl = 0;
	_unrealized_pnl = 0;
	_realized_pnl = 0;
	_pnl = 0;
	_open = 0;
	_last_exec.clear();


}
//======================================================================
void LFTradeStats::update(const LFPositionData& pos_data_)
{
	double q_diff = 0;
	if (_side == LF::s_UNKNOWN
		|| (_side == pos_data_._side && pos_data_._open._q > 0))
	{
		_side = pos_data_._side;
		_unrealized_pnl = pos_data_._unrealized_pnl;
		_realized_pnl = pos_data_._realized_pnl - _pre_realized_pnl;
		_pnl = pos_data_._pnl - _pre_realized_pnl;
		q_diff = TCCompareQty::diff(pos_data_._open._q, _open);
		_open = pos_data_._open._q;
	}
	else
	{
		if (started())
		{
			_unrealized_pnl = 0;
			_realized_pnl = pos_data_._realized_pnl - _pre_realized_pnl;
			_pnl = _realized_pnl;
			_end_t = pos_data_._t;
			_end_t_str = TCTimestamp(_end_t).c_str();
			q_diff = TCCompareQty::diff(0, _open);
			_open = 0;
			_last_exec = pos_data_._last_exec;
		}
		else
		{
			_unrealized_pnl = pos_data_._unrealized_pnl;
			_realized_pnl = pos_data_._realized_pnl - _pre_realized_pnl;
			_pnl = pos_data_._pnl - _pre_realized_pnl;
			q_diff = (_side == pos_data_._side) ? TCCompareQty::diff(pos_data_._open._q, _open) : pos_data_._open._q;
			_side = pos_data_._side;
			_open = pos_data_._open._q;
		}
	}

	if (q_diff > 0)
	{
		if (_side == LF::s_BUY)
			_num_buys++;
		else if (_side == LF::s_SELL)
			_num_sells++;
	}
	else if (q_diff < 0)
	{
		if (_side == LF::s_BUY)
			_num_sells++;
		else if (_side == LF::s_SELL)
			_num_buys++;
	}


	if (_open > _open_max) _open_max = _open;
	if (_realized_pnl < _min_realized_pnl)
		_min_realized_pnl = _realized_pnl;
	if (_realized_pnl > _max_realized_pnl)
		_max_realized_pnl = _realized_pnl;

	if (_unrealized_pnl < _min_unrealized_pnl)
		_min_unrealized_pnl = _unrealized_pnl;
	if (_unrealized_pnl > _max_unrealized_pnl)
		_max_unrealized_pnl = _unrealized_pnl;

	if (_pnl < _min_pnl)
		_min_pnl = _pnl;
	if (_pnl > _max_pnl)
		_max_pnl = _pnl;
}
//======================================================================
const char* LFTradeStats::header()
{
	return ",R_KEY,START,END,SIDE,PRE_REALIZED,OPEN_MAX,BUYS,SELLS,MIN_REALIZED,MAX_REALIZED,\
		   		   MIN_UNREALIZED,MAX_UNREALIZED,MIN_PNL,MAX_PNL,\
				   		   L_REALIZED,L_UNREALIZED,L_PNL,L_OPEN,LAST_EXEC";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFTradeStats& d_)
{
	d_.to_stream(os);

	os << "," << d_._run_key
		<< "," << d_._s_t_str
		<< "," << d_._end_t_str
		<< "," << (long)d_._side
		<< "," << d_._pre_realized_pnl
		<< "," << d_._open_max
		<< "," << d_._num_buys
		<< "," << d_._num_sells
		<< "," << d_._min_realized_pnl
		<< "," << d_._max_realized_pnl
		<< "," << d_._min_unrealized_pnl
		<< "," << d_._max_unrealized_pnl
		<< "," << d_._min_pnl
		<< "," << d_._max_pnl
		<< "," << d_._realized_pnl
		<< "," << d_._unrealized_pnl
		<< "," << d_._pnl
		<< "," << d_._open
		<< "," << d_._last_exec;

	return os;
}
//======================================================================
LFTradeStats::~LFTradeStats()
{
}
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFTradeStats& d_)
{
	if (!d_.to_cdr(strm))
		return false;
	if (!(strm << d_._run_key))
		return false;
	if (!(strm << d_._s_t))
		return false;
	if (!(strm << d_._s_t_str))
		return false;
	if (!(strm << (long)d_._side))
		return false;
	if (!(strm << d_._pre_realized_pnl))
		return false;
	if (!(strm << d_._end_t))
		return false;
	if (!(strm << d_._end_t_str))
		return false;
	if (!(strm << d_._open_max))
		return false;
	if (!(strm << d_._num_buys))
		return false;
	if (!(strm << d_._num_sells))
		return false;
	if (!(strm << d_._min_realized_pnl))
		return false;
	if (!(strm << d_._max_realized_pnl))
		return false;
	if (!(strm << d_._min_unrealized_pnl))
		return false;
	if (!(strm << d_._max_unrealized_pnl))
		return false;
	if (!(strm << d_._min_pnl))
		return false;
	if (!(strm << d_._max_pnl))
		return false;
	if (!(strm << d_._realized_pnl))
		return false;
	if (!(strm << d_._unrealized_pnl))
		return false;
	if (!(strm << d_._pnl))
		return false;
	if (!(strm << d_._open))
		return false;
	if (!(strm << d_._last_exec))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFTradeStats& d_)
{
	if (!d_.from_cdr(strm))
		return false;
	if (!(strm >> d_._run_key))
		return false;
	if (!(strm >> d_._s_t))
		return false;
	if (!(strm >> d_._s_t_str))
		return false;
	long s = 0;
	if (!(strm >> s))
		return false;
	d_._side = LF::Side(s);
	if (!(strm >> d_._pre_realized_pnl))
		return false;
	if (!(strm >> d_._end_t))
		return false;
	if (!(strm >> d_._end_t_str))
		return false;
	if (!(strm >> d_._open_max))
		return false;
	if (!(strm >> d_._num_buys))
		return false;
	if (!(strm >> d_._num_sells))
		return false;
	if (!(strm >> d_._min_realized_pnl))
		return false;
	if (!(strm >> d_._max_realized_pnl))
		return false;
	if (!(strm >> d_._min_unrealized_pnl))
		return false;
	if (!(strm >> d_._max_unrealized_pnl))
		return false;
	if (!(strm >> d_._min_pnl))
		return false;
	if (!(strm >> d_._max_pnl))
		return false;
	if (!(strm >> d_._realized_pnl))
		return false;
	if (!(strm >> d_._unrealized_pnl))
		return false;
	if (!(strm >> d_._pnl))
		return false;
	if (!(strm >> d_._open))
		return false;
	if (!(strm >> d_._last_exec))
		return false;
	return true;
}
//======================================================================
//======================================================================
//======================== LFRunStats ===================================
//======================================================================
LFRunStats::LFRunStats()
{
	_run = BMModel::instance()->run_num();
	_instr = Z::get_setting("LFStrategy:follower");
	_run_key = Z::get_setting("LFStrategy:strategy_id");
}
//======================================================================
const char* LFRunStats::header()
{
	return ",R_KEY,RUN_ID,INSTR,OPEN_MAX,BUYS,SELLS,MIN_REALIZED,MAX_REALIZED,\
MIN_UNREALIZED,MAX_UNREALIZED,MIN_PNL,MAX_PNL\
L_REALIZED,L_UNREALIZED,L_PNL,L_OPEN";
}
//======================================================================
void LFRunStats::update(const LFPositionData& pos_data_)
{
	_c_data.update(pos_data_);
	if (_t_data.started())
	{
		_t_data.update(pos_data_);
	}
	else if (pos_data_._open._q > 0)
	{
		_t_data.reset(pos_data_._t, pos_data_._realized_pnl);
		_t_data.update(pos_data_);
	}
	if (_t_data.ended())
	{
		_t_history.push_back(_t_data);
		_t_data.reset();
		if (pos_data_._open._q > 0)
		{
			_t_data.reset(pos_data_._t, pos_data_._realized_pnl);
			_t_data.update(pos_data_);
		}
	}
}
//======================================================================
void LFRunStats::report() 
{
	_t = TCTimestamp::stamp();
	
	TCResourceMap	mmap;
	std::string base = "LFModel:";

	TCSettings::get_resource_match(base + "*", mmap);
	
	std::string fname = TCFiler::get_run_dir(Z::get_setting("BMTS:run_dir", Z::get_setting("LOG:dir", ".")),_run, false);
	fname += "/model.cfg";
	std::ofstream l_stream(fname.c_str());
	if (l_stream.good())
		for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
			l_stream << base << (*i).second->get_sub_name(1) << "\t" << (*i).second->get_value() << std::endl;

}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFRunStats& d_)
{
	d_.to_stream(os);

	os	<< "," << d_._run_key
		<< "," << d_._run
		<< "," << d_._instr
		<< "," << d_._c_data._open_max
		<< "," << d_._c_data._num_buys
		<< "," << d_._c_data._num_sells 
		<< "," << d_._c_data._min_realized_pnl
		<< "," << d_._c_data._max_realized_pnl
		<< "," << d_._c_data._min_unrealized_pnl
		<< "," << d_._c_data._max_unrealized_pnl
		<< "," << d_._c_data._min_pnl
		<< "," << d_._c_data._max_pnl
		<< "," << d_._c_data._realized_pnl
		<< "," << d_._c_data._unrealized_pnl
		<< "," << d_._c_data._pnl
		<< "," << d_._c_data._open;

	return os;
}
//======================================================================
void LFRunStats::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 13)
		z_throw("LFPositionData::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_run_key			= v_[ind++];
	_run				= atol(v_[ind++]);
	ind++;
	_c_data._open_max			= atof(v_[ind++]);
	_c_data._num_buys			= atof(v_[ind++]);
	_c_data._num_sells			= atof(v_[ind++]);
	_c_data._min_realized_pnl	= atof(v_[ind++]);
	_c_data._max_realized_pnl	= atof(v_[ind++]);
	_c_data._min_unrealized_pnl	= atof(v_[ind++]);
	_c_data._min_unrealized_pnl	= atof(v_[ind++]);
	_c_data._min_pnl			= atof(v_[ind++]);
	_c_data._max_pnl			= atof(v_[ind++]);
	_c_data._realized_pnl		= atof(v_[ind++]);
	_c_data._unrealized_pnl	= atof(v_[ind++]);
	_c_data._pnl				= atof(v_[ind++]);
	_c_data._open				= atof(v_[ind++]);
}
//======================================================================
LFRunStats::~LFRunStats()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFRunStats& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._run_key)) 
	  return false;
	if (!(strm << d_._run)) 
	  return false;
	if (!(strm << d_._instr)) 
	  return false;
	if (!(strm << d_._c_data)) 
	  return false;
	if (!(strm << d_._t_data)) 
	  return false;
	if (!(strm << d_._t_history)) 
	  return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFRunStats& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;

	if (!(strm >> d_._run_key)) 
	  return false;
	if (!(strm >> d_._run)) 
	  return false;
	if (!(strm >> d_._instr)) 
	  return false;
	if (!(strm >> d_._c_data)) 
	  return false;
	if (!(strm >> d_._t_data)) 
	  return false;
	if (!(strm >> d_._t_history)) 
	  return false;
	return true;
}
std::string LFRunStats::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _run << " (" << _instr << ")"	<< "\n" 
		<< "max open (" << _c_data._open_max << ") "	
		<< "sells (" << _c_data._num_sells << ") buys (" << _c_data._num_buys << ") "	
		<< "upnl ("  << _c_data._min_unrealized_pnl	<< " "	<< _c_data._max_unrealized_pnl << ") "											<< " "
		<< "rpnl ("  << _c_data._min_realized_pnl	<< " "	<< _c_data._max_realized_pnl << ") "												<< " "
		<< "pnl ("  << _c_data._min_pnl	<< " " << _c_data._max_pnl << ")" 
		<< "l pnl(" << _c_data._realized_pnl << " " << _c_data._unrealized_pnl << " " << _c_data._pnl << ")" 
		<< "l open(" << _c_data._open	<< std::endl;
	return os.str();
}
//======================================================================
LFRunData::LFRunData(const std::string& model_resource_, const std::string& strategy_resource_)
: _m_base(model_resource_), _s_base(strategy_resource_)
{
}
//======================================================================
std::string LFRunData::as_string(bool full_) const
{
	TCResourceMap	mmap;
	std::string br = _m_base + ":backtest:*";
	TCSettings::get_resource_match(br, mmap);
	std::set<std::string> s;

	for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
		s.insert((*i).second->get_sub_name(2));

	std::string r;
	for (size_t i = 0; i < _model.size(); ++i)
	{
		if(s.find(_model[i].first) != s.end())
			if(full_)
				r += _model[i].first + "(" + _model[i].second + "),";
			else
			{
				std::vector<std::string> v;
				tc_token_vec(_model[i].first,	v, '_');
				for(size_t k = 0; k < v.size(); ++k)
					r+= v[k][0] + '_';
				if(r.size() > 2)
					r.erase(r.end()-1, r.end());
				r += "(" + _model[i].second + "),";
			}
	}
	if(!r.empty())
		r.erase(r.end()-1, r.end());
	return r;
}
//======================================================================
TCBusinessTimeRange LFRunData::init(bool marked_only_)
{
//---------------------LFStrategy settings
	TCResourceMap	smap;
	std::string br = _s_base + ":*";
	TCSettings::get_resource_match(br, smap);

	for(TCResourceMap::iterator j = smap.begin(); j != smap.end(); ++j)
	{
		if(!marked_only_ || (*j).second->get_marked())
			_strategy.push_back(ResourceString((*j).second->get_sub_name(1), (*j).second->get_value()));
	}

	std::string range = Z::get_setting("LFModel:date_range");
	TCBusinessTimeRange	btr;
	if(range.empty())
	{
		std::string start_t = Z::get_setting("LFModel:start_time",	"09:15:00");
		std::string stop_t = Z::get_setting("LFModel:stop_time", "14:15:00");
		std::string day_t = Z::get_setting("LFModel:date");
		if(day_t.empty())
			btr.init(TCDateTime(TCTime(	start_t)).get_ace_time(), TCDateTime(TCTime(stop_t)).get_ace_time());
		else
			btr.init(TCDateTime(TCDate(day_t), TCTime(	start_t)).get_ace_time(), TCDateTime(TCDate(day_t), TCTime(stop_t)).get_ace_time());
		Z::add_setting("LFModel:date_range", btr.as_string());
	}
	else
		btr.init(range);
//---------------------LFModel settings
	TCResourceMap	mmap;
	br = _m_base + ":*";
	TCSettings::get_resource_match(br, mmap);

	for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
	{
		//if((*i).second->key().size() == 2)
		{
			const std::string& s = (*i).second->get_sub_name(1);
			if(s == "start_time")
			{
				TCTime t(btr.from());
				if(!marked_only_ || (*i).second->get_marked())
					_model.push_back(ResourceString(s, t.as_string()));
			}
			else if(s == "stop_time")
			{
				TCTime t(btr.to());
				if(!marked_only_ || (*i).second->get_marked())
					_model.push_back(ResourceString(s, t.as_string()));
			}
			else
			{
				if(!marked_only_ || (*i).second->get_marked())
					_model.push_back(ResourceString(s, (*i).second->get_value()));
			}
		}
	}
	return btr;
}
void LFRunData::set()
{
//	Z::report_info("LFRunData::set -- adding resources");
	bool verbose = Z::get_setting_bool("back_test:out_file", false);
	std::string r;
	for(size_t i = 0; i < _model.size(); ++i)
	{
		r = _m_base+":"+_model[i].first;
		Z::add_setting(r, _model[i].second);
		if (verbose)
			Z::report_info("%s %s", r.c_str(), _model[i].second.c_str());
	}
	for(size_t k = 0; k < _strategy.size(); ++k)
	{
		r = _s_base+":"+_strategy[k].first;
		Z::add_setting(r, _strategy[k].second);
		if (verbose)
			Z::report_info("%s %s", r.c_str(), _strategy[k].second.c_str());
	}
}
//======================================================================
const char* LFRunEvent::header()
{
	return ",R_KEY,RUN_ID,INSTR,OPEN_MAX,BUYS,SELLS,MIN_REALIZED,MAX_REALIZED,\
MIN_UNREALIZED,MAX_UNREALIZED,MIN_PNL,MAX_PNL,L_REALIZED,L_UNREALIZED,L_PNL,L_OPEN,DATE,L_PARAMS";
}
//======================================================================
void LFRunEvent::init_backtest(std::vector<LFRunEvent*>& run_events_)
{
	TCBusinessTimeRange btr = _run_data.init();
	_t = TCTimestamp::stamp();
	
	RunDataVec v;
	TCResourceMap	mmap;
	std::string br = _run_data._m_base + ":backtest:*";
	TCSettings::get_resource_match(br, mmap);
	for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
		v.push_back(ResourceString((*i).second->get_sub_name(2), (*i).second->get_value()));

	run_events_.push_back(new LFRunEvent(*this));
	parse(v, 0, run_events_);
	//run_events_.pop_back();
	TCDateVec dv = btr.vec();
	v.clear();
	tc_vector<long> weekdays(Z::get_setting("LFModel:backtest_days"), ',');
	for(size_t i = 0; i < dv.size(); ++i)
	{
		if(!weekdays.empty())
		{
			TCJulianDate jd(dv[i]);
			unsigned wd = jd.week_day();
			if(std::find(weekdays.begin(), weekdays.end(), wd)!=weekdays.end())
				v.push_back(ResourceString("date", dv[i].as_string()));
		}
		else
			v.push_back(ResourceString("date", dv[i].as_string()));
	}
	parse(v, 0, run_events_);
	run_events_.pop_back();
	std::cout << "produced " << run_events_.size() << " sets of data" << std::endl;
}
//======================================================================
void LFRunEvent::parse(const RunDataVec& vec_, size_t index_, std::vector<LFRunEvent*>& run_events_)
{
	if(index_ >= vec_.size())
		return;

	std::vector<std::string> v;
	tc_token_vec(vec_[index_].second, v, ',');

	for(size_t k = 0; k < v.size(); ++k)
	{
		RunDataVec::iterator it;
		for(it = run_events_[run_events_.size()-1]->_run_data._model.begin(); it != run_events_[run_events_.size()-1]->_run_data._model.end(); ++it)
		{	
			if (it->first == vec_[index_].first)	
			{
				it->second = v[k];
				break;
			}
		}
		if(it == run_events_[run_events_.size()-1]->_run_data._model.end())
			run_events_[run_events_.size()-1]->_run_data._model.push_back(ResourceString(vec_[index_].first, v[k]));
		
		if(index_ == vec_.size()-1) 
		{
			LFRunEvent* rd = run_events_[run_events_.size()-1];
			run_events_.push_back(new LFRunEvent(*rd));
		}
		else
		{
			if(v.size() == 1)
			{
				LFRunEvent* rd = run_events_[run_events_.size()-1];
				run_events_.push_back(new LFRunEvent(*rd));
			}
			parse(vec_, index_+1, run_events_);
		}
	}
}
//======================================================================
void LFRunEvent::init_date()
{
	RunDataVec& m = _run_data._model;
	for(size_t i = 0; i < m.size(); ++i)
	{
		if(m[i].first == "date")
		{
			_date = m[i].second;
			break;
		}
	}
}
//======================================================================
LFRunEvent::LFRunEvent()
{
}
//----------------------------------------------------------------------
std::string LFRunEvent::as_string() const
{
	return _run_stats.as_string() + _run_data.as_string();
}
//----------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const LFRunData& d_)
{
	for (size_t i = 0; i < d_._model.size(); ++i)
		os << ":" << d_._model[i].second;
	return os;
}
//----------------------------------------------------------------------
bool operator<<(ACE_OutputCDR& strm, const LFRunEvent& d_)
{
	return	(strm << d_._run_stats) && 
			(strm << d_._run_data) &&
			(strm << d_._teasers);
}
//----------------------------------------------------------------------
bool operator>>(ACE_InputCDR& strm, LFRunEvent& d_)
{
	return	(strm >> d_._run_stats) && 
			(strm >> d_._run_data) &&
			(strm >> d_._teasers);
}
//----------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const LFRunEvent& d_)
{
	os << d_._run_stats;
	os << "," << d_._date;
	os << "," << d_._run_data;

	return os;
}
//----------------------------------------------------------------------

//======================================================================
const char* LFTotalEvent::header()
{
	return ",PERIOD,DAYS,TRADED_DAYS,LOSS_DAYS,MIN_REALIZED,MAX_REALIZED,\
		   		   MIN_UNREALIZED,MAX_UNREALIZED,MIN_PNL,MAX_PNL,GAIN,LOSS,PNL,SUCCESS_PCT";
}
//----------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const LFTotalEvent& d_)
{
	d_.to_stream(os);
	os << "," << d_._date
		<< "," << d_._run_stats._c_data._open_max
		<< "," << d_._run_stats._c_data._num_buys
		<< "," << d_._run_stats._c_data._num_sells
		<< "," << d_._run_stats._c_data._min_realized_pnl
		<< "," << d_._run_stats._c_data._max_realized_pnl
		<< "," << d_._run_stats._c_data._min_unrealized_pnl
		<< "," << d_._run_stats._c_data._max_unrealized_pnl
		<< "," << d_._run_stats._c_data._min_pnl
		<< "," << d_._run_stats._c_data._max_pnl
		<< "," << d_._run_stats._c_data._realized_pnl
		<< "," << d_._run_stats._c_data._unrealized_pnl
		<< "," << d_._run_stats._c_data._pnl
		<< "," << d_._run_stats._c_data._open;

	return os;

}
//======================================================================
//======================================================================
//======================================================================
const char* LFDrawDown::header()
{
	return ",START_DATE,END_DATE,MAX_DATE,MAX_DOWN";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFDrawDown& d_)
{
	d_.to_stream(os);

	os << "," << d_._start_date;
	os << "," << d_._end_date;
	os << "," << d_._max_date;
	os << "," << d_._max_down;
	return os;
}
//======================================================================
LFDrawDown::LFDrawDown()
	: _start_date(0),
	_end_date(0),
	_max_date(0),
	_max_down(0)
{
}
//======================================================================
LFDrawDown::~LFDrawDown()
{
}
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFDrawDown& d_)
{
	if (!d_.to_cdr(strm))
		return false;
	if (!(strm << d_._run_key))
		return false;
	if (!(strm << d_._start_date))
		return false;
	if (!(strm << d_._end_date))
		return false;
	if (!(strm << d_._max_date))
		return false;
	if (!(strm << d_._max_down))
		return false;
	return true;
}
//======================================================================
bool operator>>(ACE_InputCDR& strm, LFDrawDown& d_)
{
	if (!d_.from_cdr(strm))
		return false;
	if (!(strm >> d_._run_key))
		return false;
	if (!(strm >> d_._start_date))
		return false;
	if (!(strm >> d_._end_date))
		return false;
	if (!(strm >> d_._max_date))
		return false;
	if (!(strm >> d_._max_down))
		return false;
	return true;
}


//======================================================================
//======================================================================
//======================================================================
const char* LFStrategyRequest::header()
{
	return ",S_KEY,STRATEGY_ID,TYPE,SIDE,STATUS,CANCEL_ID,Q,LIMIT_P,STOP_P,Q_A,Q_S,Q_P,Q_L,BOOK,S_TYPE,S_ACTIVE";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFStrategyRequest& d_)
{
	d_.to_stream(os);

	os	 << "," << d_._strategy_key
		 << "," << d_._strategy_id
	     << "," << (long)d_._order_type
	     << "," << (long)d_._order_side
		 << "," << (long)d_._position_status
		 << "," << d_._cancel_id
		 << "," << d_._q
		 << "," << d_._limit_p
		 << "," << d_._stop_p;
	os << "," << d_._q_A_i;
	os << "," << d_._q_S_i;
	os << "," << d_._q_P_i;
	os << "," << d_._q_L_i;
	os << "," << d_._booking?"1":"0";
	os << "," << d_._st_action;
	os << "," << d_._st_active?"1":"0";
	return os;
}
//======================================================================
void LFStrategyRequest::from_csv_vec(const std::vector<const char*>& v_)
{
	if(v_.size() < Z::Point::TIME_FIELDS + 11)
		z_throw("LFStrategyRequest::from_csv_vec -- bad format.");

	Z::Point::from_csv_vec(v_);

	int ind = Z::Point::TIME_FIELDS;

	_strategy_key = v_[ind++];
	_strategy_id = atol(v_[ind++]);
	_order_type = LF::OrderType(atol(v_[ind++]));
	_order_side = LF::Side(atol(v_[ind++]));
	_position_status = LF::PositionStatus(atol(v_[ind++]));
	_cancel_id = atol(v_[ind++]);
	_q = atol(v_[ind++]);
	_limit_p = atof(v_[ind++]);
	_stop_p = atof(v_[ind++]);
	_q_A_i = atof(v_[ind++]);
	_q_S_i = atof(v_[ind++]);
	_q_P_i = atof(v_[ind++]);
	_q_L_i = atof(v_[ind++]);
}
//======================================================================
LFStrategyRequest::LFStrategyRequest(long strategy_id_)
:	_strategy_id(strategy_id_),
	_order_type(LF::otUndefined),
	_order_side(LF::s_UNKNOWN),
	_position_status(LF::psUndefined),
	_cancel_id(-1),
	_q(0),
	_limit_p(0),
	_stop_p(0),
	_q_A_i(-1),
	_q_S_i(-1),
	_q_P_i(-1),
	_q_L_i(-1),
	_booking(false),
	_st_action(LF::satUNKNOWN),
	_st_active(false)
{
	std::ostringstream os;
	os << strategy_id_;
	_strategy_key = os.str();
}
//======================================================================
LFStrategyRequest::~LFStrategyRequest()
{
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFStrategyRequest& d_)
{
	if (!d_.to_cdr(strm)) 
		return false;
	if (!(strm << d_._strategy_key)) 
		return false;
	if (!(strm << d_._strategy_id)) 
		return false;
	if(!(strm << (long)d_._order_type))
		return false;
	if(!(strm << (long)d_._order_side))
		return false;
	if(!(strm << (long)d_._position_status))
		return false;
	if(!(strm << d_._cancel_id))
		return false;
	if(!(strm << d_._q))
		return false;
	if(!(strm << d_._limit_p))
		return false;
	if(!(strm << d_._stop_p))
		return false;
	if(!(strm << d_._q_A_i))
		return false;
	if(!(strm << d_._q_S_i))
		return false;
	if(!(strm << d_._q_P_i))
		return false;
	if(!(strm << d_._q_L_i))
		return false;
	if(!(strm << d_._booking))
		return false;
	if(!(strm << (long)d_._st_action))
		return false;
	if(!(strm << d_._st_active))
		return false;
	return true;
}
//======================================================================
bool operator>>(ACE_InputCDR& strm, LFStrategyRequest& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._strategy_key)) 
	  return false;
	if (!(strm >> d_._strategy_id)) 
	  return false;
	long ot = 0;
	if(!(strm >> ot))
		return false;
	d_._order_type = LF::OrderType(ot);
	long os = 0;
	if(!(strm >> os))
		return false;
	d_._order_side = LF::Side(os);
	long ps = 0;
	if(!(strm >> ps))
		return false;
	d_._position_status = LF::PositionStatus(ps);
	if(!(strm >> d_._cancel_id))
		return false;
	if(!(strm >> d_._q))
		return false;
	if(!(strm >> d_._limit_p))
		return false;
	if(!(strm >> d_._stop_p))
		return false;
	if(!(strm >> d_._q_A_i))
		return false;
	if(!(strm >> d_._q_S_i))
		return false;
	if(!(strm >> d_._q_P_i))
		return false;
	if(!(strm >> d_._q_L_i))
		return false;
	if(!(strm >> d_._booking))
		return false;
	long ac = 0;
	if(!(strm >> ac))
		return false;
	d_._st_action = LF::StrategyActionType(ac);
	if(!(strm >> d_._st_active))
		return false;
	return true;
}
//======================================================================
std::string LFStrategyRequest::as_string() const
{
	std::ostringstream os;
	os << TCTimestamp(_t).c_str() << " " << _strategy_id <<  " " << _order_type 
		<<  " " << _order_side << " " << _position_status << " " 
		<< _cancel_id << " " << _q  << " " << _limit_p << " " << _stop_p 
		<< " " << _booking << " " << _st_action << " " << _st_active;
	return os.str();
}

//======================================================================
//======================================================================
//======================================================================
const char* CMEDataPoint::header()
{
	return ",KEY,S_T,52,75,279,83,269,PRICE,SIZE,273,274,276,277,336,346,451,1023,COMMENT";
}

//======================================================================
void CMEDataPoint::body_to_stream(std::ostream& os) const
{
	os << "," << _key;
	os << "," << _s_t;				// local time as computed

	os << "," << _SendingTime;		//52
	os << "," << _TradeDate;		//75
	os << "," << _MDUpdateAction;	//279
	os << "," << _RptSeq;			//83
	os << "," << _MDEntryType;		//269
	os << "," << _MDEntryPx;		//270
	os << "," << _MDEntrysize;		//271
	os << "," << _MDEntryTime;		//273
	os << "," << _TickDirection;	//274
	os << "," << _QuoteCondition;	//276
	os << "," << _TradeCondition;	//277
	os << "," << _TradingSessionID;	//336
	os << "," << _NumberOfOrders;	//346
	os << "," << _NetChgPrevDay;	//451
	os << "," << _MDPriceLevel;		//1023

	os << "," << _comment;
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const CMEDataPoint& d_)
{

	d_.to_stream(os);
	d_.body_to_stream(os);
	return os;

}
//======================================================================
CMEDataPoint::CMEDataPoint(const std::string& key_)
 : _key(key_)
{
}
//======================================================================
CMEDataPoint::~CMEDataPoint() 
{
}
//======================================================================
void CMEDataPoint::reset_h(const std::string&		SendingTime,	
		  				   const std::string&		TradeDate)
{
	_SendingTime		= SendingTime;		
	_TradeDate			= TradeDate;
}

//======================================================================
void CMEDataPoint::reset_b(const ACE_Time_Value&	s_t_,	// local time as computed
					  int				MDUpdateAction,		//279
				const std::string&		RptSeq,				//83
				const std::string&		MDEntryType,		//269
				const std::string&		MDEntryPx,			//270
				const std::string&		MDEntrysize,		//271
				const std::string&		MDEntryTime,		//273
				const std::string&		TickDirection,		//274
				const std::string&		QuoteCondition,		//276
				const std::string&		TradeCondition,		//277
				const std::string&		TradingSessionID,	//336
				const std::string&		NumberOfOrders,		//346
				const std::string&		NetChgPrevDay,		//451
				const std::string&		MDPriceLevel)		// 1023
{
	_s_t = TCTimestamp(s_t_).c_str();

	_MDUpdateAction		= MDUpdateAction;
	_RptSeq				= RptSeq;
	_MDEntryType		= MDEntryType;
	_MDEntryPx			= MDEntryPx;		
	_MDEntrysize		= MDEntrysize;		
	_MDEntryTime		= MDEntryTime;	
	_TickDirection		= TickDirection;
	_QuoteCondition		= QuoteCondition;
	_TradeCondition		= TradeCondition;
	_TradingSessionID	= TradingSessionID;	
	_NumberOfOrders		= NumberOfOrders;
	_NetChgPrevDay		= NetChgPrevDay;
	_MDPriceLevel		= MDPriceLevel;
}
//======================================================================

