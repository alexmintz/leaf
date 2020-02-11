/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MOMENTUM_DATA_H
#define LF_MOMENTUM_DATA_H

#include "znet/ZNet.h"
#include "znet/TCTradingTime.h"
#include "leaf/LFScopeStore.h"

//======================================================================
struct LEAF_Export LFTradePoint
{
	LFTradePoint(const ACE_Time_Value& t_ = ACE_Time_Value::zero, const LFSide* side_ = 0, const TC_pqp& trade_ = TC_pqp(), long tran_ = 0)
		:_t(t_), _side(side_), _trade(trade_), _tran(tran_)
	{}

	void reset(const ACE_Time_Value& t_ = ACE_Time_Value::zero, const LFSide* side_ = 0, const TC_pqp& trade_ = TC_pqp(), long tran_ = 0)
	{
		_t = t_;
		_side = side_;
		_trade = trade_;
		_tran = tran_;
	}

	bool valid() const { return _t != ACE_Time_Value::zero;	}

	ACE_Time_Value	_t;
	const LFSide*	_side;
	TC_pqp			_trade;
	long			_tran;
};


//======================================================================
struct LEAF_Export LFMomentumData_P
{
	LFMomentumData_P(LFMarketModel* model_ = 0);
	virtual ~LFMomentumData_P();

	virtual void reset();

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFMomentumData_P& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFMomentumData_P& d_);
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFMomentumData_P& d_);
	void from_csv_vec(int& ind_, const std::vector<const char*>& v_);

	LFDepthData	_md;
	double	_p;
	double  _obv;
	double  _vma;
	double  _mfi;

	double  _u_vol;
	double  _d_vol;
	double  _u_tran;
	double  _d_tran;
	ACE_Time_Value _t;


	double _last_close;
	//------------
	double	_p_activity;
	double	_v_activity;
	double	_corr;
	double	_corr_dyn_high;
	double	_corr_dyn_low;

	//------------ box
	LFBox	_box;
	LFBoxTrends _box_trends;
	LFBoxTrends _lcy_trends;
	LFBoxVec	_lcy;
	LFBox	_latter_box;
	LFBox	_former_box;

	LFBoxTrend _charge_trend;

	LFBox	_sharp_box;

	//------------ trend 
	const LFSide*	_trend_side;
	bool			_in_trend;
	double			_trend_hard_p;
	double			_trend_soft_p;

	//------------

	LFBox	_bias_bc;	// Sharp Charge
	LFBox	_bias_bx;	// Sharp Box
};

//======================================================================
struct LEAF_Export LFMomentumData : public LFMomentumData_P
{
	LFMomentumData(const std::string& market_, const std::string& momentum_);
	virtual ~LFMomentumData();

	virtual void reset();
	void load_and_reset();

	bool process_missed_tick(const ACE_Time_Value& t_);
	bool process_depth(const LFDepthData& md_);
	bool check_process_last_trade(const ACE_Time_Value& t_);
	bool process_last_trade(const ACE_Time_Value& t_);
	bool process_trade(const LFTradeData& td_);

	void update(const ACE_Time_Value& t_, double p_, bool trade_);
	void fill_trade(LFTradePoint& tp_, const ACE_Time_Value& t_, const TC_pqp& pqp_);

	void set_wave(LFBox& b_, const LFPoint& from_);
	void update_scopes(bool force_0_end_);
	void reset_bias(bool add_);
	void reset_local_bias(bool force_overwrite_ = false);

	double calc_exp_contra_p(const LFBox& b_, double p_charge_1_, double exp_contra_pias_) const;

	void calc_point(const std::string& comment_);

	void fill_sharp_roll(LFBox& sb_, LFCluster& etalon_, std::string& comment_,
		tc_range<double>& pslope_, tc_range<double>& min_v_,
		double pslope_decline_, double max_l_);

	void fill_sharp_box(LFBox& sb_, LFCluster& etalon_, std::string& comment_,
		tc_range<double>& pslope_, tc_range<double>& min_v_, 
		double pslope_decline_, double max_l_);

	void clear_just_flags();

	void init_merge(LFBox& to_) const;

	void set_scope(LFBox& b_) const;

	void set_bounce(LFBounce& bnc_,  const LFCluster& peak_, const LFDataPointPtr& dp_);

	void set_limit_t(const ACE_Time_Value& from_t_, bool from_trade_);

	const LFDataPointPtr& dp() const { return _p_store->dp(); }
	const LFDataPointPtr& prev_dp() const { return _p_store->prev_dp(); }

	LFScopeStore& scopes() { return *_s_store; }
	const LFScopeStore& scopes() const { return *_s_store; }

	LFMarketModel _model;

	std::string	_levels_key;
	std::string	_box_key;
	std::string	_datapoint_key;


	auto_ptr<LFPointStore>	_p_store;
	auto_ptr<LFScopeStore>	_s_store;

	ACE_Time_Value		_tick_limit_tv;
	bool				_tick_limit_from_trade;
	const LFSide*		_last_trade_side;
	LFTradePoint		_last_trade;
	long				_i_count;

private:	
	LFMomentumData(const LFMomentumData&);
	LFMomentumData& operator=(const LFMomentumData&);
};

//======================================================================
#endif /*LF_MOMENTUM_DATA_H*/
