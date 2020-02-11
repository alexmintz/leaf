/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_DATA_H
#define LF_DATA_H

#include "znet/ZNet.h"
#include "leaf/LFMarketData.h"
#include "leaf/LFMomentumData.h"
#include "leaf/LEAF_Export.h"

//======================================================================
typedef	std::vector<LFMomentumData*> LFMomentumDataPVec;

//======================================================================
struct LEAF_Export LFStrategyState
{
	LFStrategyState();
	virtual ~LFStrategyState();

	void reset()
	{
		_fume = 0;
		_exit = 0;
		_cur_profit_ticks = 0;
		_max_profit_ticks = 0;
		_i_count = 0;

		_l_1.reset();
		_f_1.reset();
		_exec.reset();
	}
	void clear_just_flags()
	{
		_l_1.clear_just_flags();
		_f_1.clear_just_flags();
	}

	ACE_Time_Value _t;
	std::string _strategy_key;

	double _fume;
	double _exit;
	double _cur_profit_ticks;
	double _max_profit_ticks;
	long	_i_count;


	LFMomentumData _l_1;
	LFMomentumData _f_1;

	LFMomentumDataPVec _m_vec;

	LFTeaser	_exec;
};

//======================================================================
struct LEAF_Export LFStrategyData  : public Z::Point
{
	LFStrategyData();
	LFStrategyData(const LFStrategyState& s_);
	virtual ~LFStrategyData();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFStrategyData& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFStrategyData& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFStrategyData& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	void reset()
	{
		_fume = 0;
		_exit = 0;

		_l_1.reset();
		_f_1.reset();
		_exec.reset();
	}

	std::string _strategy_key;

	double _fume;
	double _exit;

	LFMomentumData_P _l_1;
	LFMomentumData_P _f_1;

	LFTeaser	_exec;
	static const char* header();
};
typedef Z::smart_ptr<LFStrategyData> LFStrategyDataPtr;

Z_EVENT_DECLARE_MODEL(LEAF, LFStrategyData)

//======================================================================
// Order placing data
//======================================================================
//======================================================================
struct LEAF_Export LFNewOrder  : public Z::Point
{
	LFNewOrder();
	virtual ~LFNewOrder();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFNewOrder& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFNewOrder& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFNewOrder& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	std::string _strategy_key;		// _strategy_id
	long		_order_id;
	long		_strategy_id;
	long		_sub_strategy_id; // boss, etc
	std::string _instr;

	long		_cqg_InstrumentID;
	long		_cqg_AccountID;
	LF::OrderType _order_type;
	LF::Side _order_side;
	long		_q;
	double		_limit_p;
	double		_stop_p;

	static const char* header();
};
typedef Z::smart_ptr<LFNewOrder> LFNewOrderPtr;

Z_EVENT_DECLARE_MODEL(LEAF, LFNewOrder)

//======================================================================
struct LEAF_Export LFCancelOrder  : public Z::Point
{
	LFCancelOrder();
	virtual ~LFCancelOrder();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFCancelOrder& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFCancelOrder& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFCancelOrder& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	std::string _strategy_key;		// _strategy_id
	long		_order_id;
	long		_strategy_id;
	long		_sub_strategy_id; // boss, etc
	std::string _instr;

	long		_cqg_InstrumentID;
	long		_cqg_AccountID;
	std::string	_cqg_OriginalOrderId;
	std::string _cqg_GWOrderID;

	LF::Side _order_side;

	static const char* header();
};
typedef Z::smart_ptr<LFCancelOrder> LFCancelOrderPtr;

Z_EVENT_DECLARE_MODEL(LEAF, LFCancelOrder)

struct LFOrderUpdate;
//======================================================================
struct LEAF_Export LFOrderFill  : public Z::Point
{
	LFOrderFill();
	LFOrderFill(const LFOrderUpdate&);
	virtual ~LFOrderFill();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFOrderFill& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFOrderFill& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFOrderFill& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	std::string _strategy_key;		// _strategy_id
	long		_order_id;
	long		_strategy_id;
	long		_sub_strategy_id; // boss, etc

	std::string _instr;

	long			_cqg_InstrumentID;
	long			_cqg_AccountID;
	std::string		_cqg_OriginalOrderId;
	std::string		_cqg_GWOrderID;

	TC_pqp			_fill;
	std::string		_fill_id;
	LF::Side	_side;
	LF::FillStatus	_status;

	static const char* header();
};
typedef Z::smart_ptr<LFOrderFill> LFOrderFillPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFOrderFill)

//======================================================================
struct LEAF_Export LFOrderUpdate  : public Z::Point
{
	LFOrderUpdate();
	LFOrderUpdate(const LFNewOrder& req_);
	virtual ~LFOrderUpdate();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFOrderUpdate& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFOrderUpdate& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFOrderUpdate& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	std::string _strategy_key;		// _strategy_id
	long		_order_id;
	long		_strategy_id;
	long		_sub_strategy_id; // boss, etc
	std::string _instr;

	long		_cqg_InstrumentID;
	long		_cqg_AccountID;
	std::string	_cqg_OriginalOrderId;
	std::string	_cqg_GWOrderID;

	LF::OrderType _order_type;
	LF::Side _order_side;
	long		_q;
	double		_limit_p;
	double		_stop_p;
	LF::OrderStatus _order_status;
	long		_filled_q;
	double		_avg_fill_p;
	long		_remain_q;
	std::string	_user_name;
	long		_trader_id;
	long		_err_code;
	std::string	_err_desc;

	std::vector<LFOrderFill> _fills;

	static const char* header();
};
typedef Z::smart_ptr<LFOrderUpdate> LFOrderUpdatePtr;
typedef std::map<long, LFOrderUpdatePtr> LFOrderMap;

Z_EVENT_DECLARE_MODEL(LEAF, LFOrderUpdate)

//======================================================================
struct LEAF_Export LFPositionData  : public Z::Point
{
	LFPositionData();
	LFPositionData(long strategy_id_, const std::string& instr_, long sub_strategy_id_ = 0);
	virtual ~LFPositionData();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFPositionData& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFPositionData& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFPositionData& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	void reset();
	void reset_user_input();

	std::string as_string() const;

	std::string		_strategy_key;
	long			_strategy_id;
	long			_sub_strategy_id;
	std::string		_instr;
	double			_tick_size;
	double			_tick_value;

	LF::Side		_side;
	TC_pqp			_open;
	TC_pqp			_opening;			//  co-sided open orders
	TC_pqp			_closing;			//  contra-sided open orders

	// totals
	TC_pqp			_p_open;			// _open + _opening
	double			_p_remain_q;		// _open - _closing
	double			_unrealized_ticks;

	TC_pqp			_last_fill;
	std::string		_last_fill_id;
	LF::Side		_last_fill_side;

	double			_realized_pnl;
	double			_unrealized_pnl;
	double			_pnl;
	
	//=== strategy fields ===
	LF::PositionStatus	_status; // active, suspended
	LF::PositionAction	_action;
	std::string _signal;
	std::string _last_exec;

	// average
	double		_q_A;
	double		_q_A_i; // user input
	double		_q_A_s; // strategy
	// scratch
	double		_q_S;
	double		_q_S_i; // user input
	// profit
	double		_q_P;
	double		_q_P_i; // user input

	//=========================
	LFBoxTrends	_box_trends;
	LFBoxTrends	_lcy_trends;


	double		_to_trend;
	LF::Side	_trend_side;
	bool		_in_trend;
	double		_trend_hard_p;
	double		_trend_soft_p;

	LFBox	_bias_bc;	// Sharp Charge
	LFBox	_bias_bx;	// Sharp Box
	//======================================================================
	enum BoxChangedBit
	{
		bc_TRENDS,
		bc_LCY,
		bc_BP,
		bc_BX,
		bc_LAST
	};

	class LEAF_Export BoxChanged : public std::bitset<bc_LAST>
	{
	public:
		typedef std::bitset<bc_LAST> Papa;
		BoxChanged(unsigned long from_ = 0) : Papa(from_) {}

		std::string to_alfa() const;
	};

	BoxChanged _box_changed;

	bool is_strategy_equal(const LFPositionData& s_);
	bool is_position_equal(const LFPositionData& s_);
	void add_fill(const LFOrderFill& of_);

	double project_avg_p(const LFSide* s_, const TC_pqp& f_);

	// updates _unrealized_pnl and _pnl
	void mark_to_market(const LFDepthData& md_);
	void strat_to_market(const LFDepthData& md_);

	static const char* header();
};
typedef Z::smart_ptr<LFPositionData>	LFPositionDataPtr;

//======================================================================
struct LEAF_Export LFInfo_Position : public Z::Point
{
	LFInfo_Position();
	LFInfo_Position(long strategy_id_, const std::string& instr_);
	LFInfo_Position(const LFPositionData& pdata_);
	virtual ~LFInfo_Position();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) { _strategy_key = k_; }

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFInfo_Position& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFInfo_Position& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFInfo_Position& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	void reset();

	std::string		_strategy_key;
	long			_strategy_id;
	std::string		_instr;

	LF::Side		_side;
	TC_pqp			_open;

	std::string		_comment;

	static const char* header();

};
typedef Z::smart_ptr<LFInfo_Position>	LFInfo_PositionPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFInfo_Position)

//======================================================================
struct LEAF_Export LFEntry : public Z::Point
{
	LFEntry(const std::string& key_ = "");
	virtual ~LFEntry();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) {_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFEntry& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFEntry& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFEntry& d_);
	virtual void body_to_stream(std::ostream& os) const;

	void reset(const LFSide* side_ = 0, const ACE_Time_Value& t_ = ACE_Time_Value::zero);
	void reset(const LFSide* side_, const ACE_Time_Value& t_, const LFPositionData& pd_, double realized_, const LFDepthData& md_, size_t seq_ = 0);

	virtual bool valid() const { return _side != 0 && _t != ACE_Time_Value::zero; }

	void mark_to_market(const LFDepthData& md_);

	void enter(double q_, double q_limit_, const LFDepthData& md_, double expected_ticks_, double tp_pct_);
	// if rr_factor_ > 0 - average using risk/reward factor
	void avg(double expected_ticks_, double risk_p_, double rr_factor_, double q_limit_, const LFDepthData& md_, double tp_pct_);

	void set_exit(double q_, const LFDepthData& md_);
	void set_enter(double p_, double q_, const LFDepthData& md_);


	void exit_pct(double q_pct_, const LFDepthData& md_);
	void exit_cutloss(const LFDepthData& md_);
	void exit_takeprofit(const LFDepthData& md_);
	// true if changed
	bool set_cutloss(double p_, double q_pct_);
	bool set_takeprofit(double p_, double q_pct_);

	std::string _key;
	const LFSide* _side;
	ACE_Time_Value _s_t;
	std::string _s_t_str;

	double		_max_q;
	size_t		_seq;
	size_t		_trend_scale;
	TC_pqp		_open;
	TC_pqp		_enter;
	TC_pqp		_exit;

	// _q in percent of _max_q
	TC_pqp		_cutloss;
	TC_pqp		_takeprofit;
	double		_cutloss_ticks;
	bool		_old_average_logic;


	double		_last_ticks;
	double		_unrealized_ticks;
	double		_realized_pnl;
	double		_pre_realized_pnl;
	bool		_side_confirmed;
	double		_unrealized_pnl;
	double		_pnl;
	std::string	_comment;

	static const char* header();
};
typedef Z::smart_ptr<LFEntry> LFEntryPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFEntry)

//======================================================================
struct LEAF_Export LFTradeStats : public Z::Point
{
	LFTradeStats();
	virtual ~LFTradeStats();

	virtual std::string get_key() const						{ return _run_key; }
	virtual void		set_key(const std::string& k_)		{ _run_key = k_; }

	void reset(const ACE_Time_Value& t_ = ACE_Time_Value::zero, double pre_realized_pnl = 0);
	void update(const LFPositionData& pos_data_);

	bool started() const { return _s_t != ACE_Time_Value::zero; }
	bool ended() const { return _end_t != ACE_Time_Value::zero; }

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFTradeStats& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFTradeStats& d_);

	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFTradeStats& d_);

	std::string		_run_key;
	ACE_Time_Value _s_t;
	std::string _s_t_str;
	LF::Side	_side;
	double		_pre_realized_pnl;
	ACE_Time_Value _end_t;
	std::string _end_t_str;
	double	_open_max;
	double	_num_buys;
	double	_num_sells;
	double	_min_realized_pnl;
	double	_max_realized_pnl;
	double	_min_unrealized_pnl;
	double	_max_unrealized_pnl;
	double	_min_pnl;
	double	_max_pnl;

	double _unrealized_pnl;
	double _realized_pnl;
	double _pnl;
	double _open;
	std::string _last_exec;

	static const char* header();

};

typedef Z::smart_ptr<LFTradeStats>	LFTradeStatsPtr;
typedef std::vector<LFTradeStats>	LFTradeStatsVec;

Z_EVENT_DECLARE_MODEL(LEAF, LFTradeStats)

//======================================================================
struct LEAF_Export LFRunStats : public Z::Point
{
	LFRunStats();

	virtual ~LFRunStats();

	virtual std::string get_key() const						{ return _run_key; }
	virtual void		set_key(const std::string& k_)	{_run_key = k_;}
	void				update(const LFPositionData& pos_data_);
	void				report();
	std::string			as_string() const;

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFRunStats& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFRunStats& d_);
	
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFRunStats& d_);
	void from_csv_vec(const std::vector<const char*>& v_);
	
	//---------------------------------------

	std::string		_run_key;
	

	LFTradeStats				_c_data;
	LFTradeStats				_t_data;
	LFTradeStatsVec				_t_history;
	std::string					_instr;

	static const char* header();
};
//======================================================================
typedef std::pair<std::string, std::string>	ResourceString;
typedef std::vector< ResourceString >		RunDataVec;
struct LEAF_Export LFRunData
{
	LFRunData(const std::string& model_resource_ = "LFModel", 
			  const std::string& strategy_resource_ = "LFStrategy");
	~LFRunData(){};

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFRunData& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFRunData& d_);
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFRunData& d_);

	TCBusinessTimeRange init(bool marked_only_ = false);
	void set();
	std::string as_string(bool full_ = false) const;

	RunDataVec	_model;
	RunDataVec	_strategy;

	std::string _m_base;
	std::string _s_base;
};
inline bool operator<<(ACE_OutputCDR& strm, const LFRunData& s)
{
	return	(strm << s._model) && 
			(strm << s._strategy) && 
			(strm << s._m_base) && 
			(strm << s._s_base);
}
inline bool operator>>(ACE_InputCDR& strm, LFRunData& s)
{
	return	(strm >> s._model) && 
			(strm >> s._strategy) && 
			(strm >> s._m_base) && 
			(strm >> s._s_base);
}

struct LEAF_Export LFRunEvent: public Z::Point
{
	LFRunEvent();
	virtual ~LFRunEvent(){}

	virtual std::string get_key() const						{ return _run_stats.get_key(); }
	virtual void		set_key(const std::string& k_)	{_run_stats.set_key(k_);}
	
	void init_backtest(std::vector<LFRunEvent*>& run_events_);
	void init_date();

	void parse(const RunDataVec& v_, size_t index_, std::vector<LFRunEvent*>& run_events_);

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFRunEvent& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFRunEvent& d_);
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFRunEvent& d_);
	std::string as_string() const;

	LFRunData		_run_data;
	LFRunStats		_run_stats;	
	std::string		_date;
	LFMatchVec		_teasers;

	static const char* header();
};
//======================================================================
typedef Z::smart_ptr<LFRunEvent>	LFRunEventPtr;

Z_EVENT_DECLARE_MODEL(LEAF, LFRunStats)
Z_EVENT_DECLARE_MODEL(LEAF, LFRunEvent)
Z_EVENT_DECLARE_MODEL(LEAF, LFPositionData)

//======================================================================
struct LEAF_Export LFTotalEvent : public LFRunEvent
{
	LFTotalEvent() {}
	LFTotalEvent(const LFRunEvent& e_)
		:LFRunEvent(e_)
	{}

	virtual ~LFTotalEvent(){}

	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFTotalEvent& d_);
	static const char* header();
};

typedef Z::smart_ptr<LFTotalEvent>	LFTotalEventPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFTotalEvent)
//======================================================================
struct LEAF_Export LFDrawDown : public Z::Point
{
	LFDrawDown();
	virtual ~LFDrawDown();

	virtual std::string get_key() const						{ return _run_key; }
	virtual void		set_key(const std::string& k_)		{ _run_key = k_; }

	void reset()
	{
		_start_date = 0;
		_end_date = 0;
		_max_date = 0;
		_max_down = 0;
	}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFDrawDown& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFDrawDown& d_);

	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFDrawDown& d_);

	std::string		_run_key;

	long	_start_date;
	long	_end_date;
	long	_max_date;

	double	_max_down;

	static const char* header();

};

typedef Z::smart_ptr<LFDrawDown>	LFDrawDownPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFDrawDown)

//======================================================================
// Strategy Requests
//======================================================================
struct LEAF_Export LFStrategyRequest  : public Z::Point
{
	LFStrategyRequest(long strategy_id_ = 0);
	virtual ~LFStrategyRequest();

	virtual std::string get_key() const { return _strategy_key; }
	virtual void set_key(const std::string& k_) {_strategy_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFStrategyRequest& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFStrategyRequest& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFStrategyRequest& d_);
	///Reads data in csv format
	void from_csv_vec(const std::vector<const char*>& v_);

	std::string as_string() const;

	std::string			_strategy_key;
	long				_strategy_id;

	LF::OrderType		_order_type;
	LF::Side			_order_side;
	LF::PositionStatus	_position_status;

	long				_cancel_id;
	long				_q;
	double				_limit_p;
	double				_stop_p;
	double				_q_A_i;
	double				_q_S_i;
	double				_q_P_i;
	double				_q_L_i;

	bool				_booking;

	LF::StrategyActionType	_st_action;
	bool				_st_active;

	static const char* header();
};
typedef Z::smart_ptr<LFStrategyRequest> LFStrategyRequestPtr;

Z_EVENT_DECLARE_MODEL(LEAF, LFStrategyRequest)

//======================================================================
struct LEAF_Export CMEDataPoint : public Z::Point
{
	CMEDataPoint(const std::string& key_ = "");
	virtual ~CMEDataPoint();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) {_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const CMEDataPoint& d_){return false;}
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, CMEDataPoint& d_) {return false;}

	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const CMEDataPoint& d_);
	virtual void body_to_stream(std::ostream& os) const;

	void reset_h(const std::string&		SendingTime,	// 52
				 const std::string&		TradeDate);		// 75

	void reset_b(const ACE_Time_Value&	s_t_,				// local time as computed
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
				const std::string&		MDPriceLevel);		// 1023

	std::string		_key;
	std::string		_s_t;				// local time as computed

	std::string		_SendingTime;		//52
	std::string		_TradeDate;			//75
	int	   		    _MDUpdateAction;	//279
	std::string		_RptSeq;			//83
	std::string		_MDEntryType;		//269
	std::string		_MDEntryPx;			//270
	std::string		_MDEntrysize;		//271
	std::string		_MDEntryTime;		//273
	std::string		_TickDirection;		//274
	std::string		_QuoteCondition;	//276
	std::string		_TradeCondition;	//277
	std::string		_TradingSessionID;	//336
	std::string		_NumberOfOrders;	//346
	std::string		_NetChgPrevDay;		//451
	std::string		_MDPriceLevel;		//1023

	std::string	_comment;
	static const char* header();
};
typedef Z::smart_ptr<CMEDataPoint> CMEDataPointPtr;
Z_EVENT_DECLARE_MODEL(LEAF, CMEDataPoint)
//======================================================================

#endif /*LF_DATA_H*/
