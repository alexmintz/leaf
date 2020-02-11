/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MARKET_MODEL_H
#define LF_MARKET_MODEL_H

#include "znet/ZNet.h"
#include "leaf/LFCommonData.h"

//======================================================================
struct LEAF_Export LFMarketModel
{
	LFMarketModel(const std::string& market_, const std::string& momentum_);
	virtual ~LFMarketModel();

	void reset();
	void load();
	static double psign(double m_)
	{
		return -LF::sign(m_);
	}

	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFMarketModel& d_);

	std::string _market;
	std::string _momentum;
	std::string _name;
	std::string _errors_key;

	ACE_Time_Value	_tick_timeout;
	ACE_Time_Value	_tick_timeout_max;
	ACE_Time_Value	_same_trade_timeout;

	bool	_spearman_corr;
	bool	_use_depth_price;

	double	_smoothness_l;
	double	_smoothness_min_l;
	double	_smoothness_decay_factor;
	double	_smoothness_weight_round;
	double	_smoothness_activity_round;
	double	_smoothness_corr_l;
	bool	_smoothness_vma_linear;

	double	_crl_decay_l_factor;
	double	_crl_decay_v_factor;
	bool	_crl_split_charge;
	double	_crl_high;
	double	_crl_low;
	double	_crl_high_offset;
	double	_crl_low_offset;
	double	_crl_discrepancy;
	double	_crl_split_negative;
	double	_crl_split_offset;
	double	_crl_split_min;
	double	_crl_split_side_charge;
	double	_crl_split_side_min;
	double	_crl_tip;
	double	_crl_tip_min_ticks;
	double	_crl_tip_end_l;

	tc_range<double>	_crl_box_sharp_slope;
	tc_range<double>	_crl_box_sharp_min_v;
	double	_crl_box_sharp_slope_decline;
	double	_crl_box_sharp_len;
	bool	_crl_box_sharp_scope_exact;
	double	_crl_box_sharp_scope_decline;
	bool	_crl_box_sharp_scope_decline_stop;
	bool	_crl_box_sharp_scope_charge_etalon;
	bool	_crl_box_sharp_scope_based;

	double	_crl_box_min_slope;
	double	_crl_box_bounce_p_pct;
	double	_crl_box_bounce_l_pct;
	double	_crl_box_bounce_min_l;
	double	_crl_continuous;
	double	_crl_continuous_straight;
	bool	_crl_continuity_weighted;

	bool				_crl_bias_full_action;
	bool				_crl_zero_exp_on_prev;
	bool				_crl_pin_joined;
	bool				_crl_join_check_parrent;
	bool				_crl_zero_exp_on_prev_charge;
	size_t				_crl_bias_avg_size;
	bool				_crl_bias_use_density;

	bool				_crl_nojoin_on_bounce;
	double				_crl_in_exp_pct;
	bool				_crl_g0_end_on_sharp;
	double				_crl_g0_end_on_max_l;
	double				_crl_g0_end_on_max_v;
	double				_crl_g0_end_on_tip_s;
	double				_crl_g0_end_on_tip_v;

	double	_tick_size;
	double	_tick_value;

	bool	_waves_on;
	bool	_out_file;
	bool	_out_file_boxes;
	tc_range<long>	_out_file_blossom;
	bool	_out_file_boxes_trace;
	bool	_out_file_datapoints;
	bool	_out_file_points;
	bool	_out_file_points_trace;
	bool	_report_old_trades;
	bool	_report_old_depth;
	bool	_report_scope_sanity;

	TCBusinessTimeRange _box_trace;

	TCBusinessTimeRange _back_trace;

	
	void set_md(const LFDepthData& md_)
	{
		_tick_size = md_._tick_size;
		_tick_value = md_._tick_value;
	}

};
//======================================================================
#endif /*LF_MARKET_MODEL_H*/
