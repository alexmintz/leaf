/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMarketModel.h"
//#include <gsl/gsl_statistics.h>
#include <cstdlib> 
#include <set>
#include <cfloat>

//======================================================================
LFMarketModel::LFMarketModel(const std::string& market_, const std::string& momentum_)
:_market(market_),
_momentum(momentum_)
{
	_name = _market + "_" + _momentum;
	_errors_key = _name + "_ERR";
	reset();
}
//======================================================================
LFMarketModel::~LFMarketModel() 
{
}
//======================================================================
void LFMarketModel::reset()
{
	_tick_timeout = ACE_Time_Value::zero;
	_tick_timeout_max = ACE_Time_Value::zero;
	_same_trade_timeout = ACE_Time_Value::zero;
	_spearman_corr = false;
	_use_depth_price = false;
	_smoothness_l = 0;
	_smoothness_min_l = 0;
	_smoothness_decay_factor = 0;
	_smoothness_weight_round = 0;
	_smoothness_activity_round = 0;
	_smoothness_corr_l = 0;
	_smoothness_vma_linear = false;
	_crl_decay_l_factor = 0;
	_crl_decay_v_factor = 0;
	_crl_split_charge = false;
	_crl_high = 0;
	_crl_low = 0;
	_crl_high_offset = 0;
	_crl_low_offset = 0;
	_crl_discrepancy = 0;
	_crl_box_sharp_slope.first =  _crl_box_sharp_slope.second = 0;
	_crl_box_sharp_min_v.first =  _crl_box_sharp_min_v.second = 0;
	_crl_box_sharp_slope_decline = 0;
	_crl_box_sharp_len = 0;
	_crl_box_sharp_scope_exact = false;
	_crl_box_sharp_scope_decline = 0;
	_crl_box_sharp_scope_decline_stop = false;
	_crl_box_sharp_scope_charge_etalon = false;
	_crl_box_sharp_scope_based = false;
	_crl_box_min_slope = 0;
	_crl_box_bounce_p_pct = 0;
	_crl_box_bounce_l_pct = 0;
	_crl_box_bounce_min_l = 0;
	_crl_continuous = 0;
	_crl_continuous_straight = 0;
	_crl_continuity_weighted = false;
	_crl_bias_full_action = false;
	_crl_zero_exp_on_prev = false;
	_crl_pin_joined = false;
	_crl_join_check_parrent = false;
	_crl_zero_exp_on_prev_charge = false;
	_crl_nojoin_on_bounce = false;
	_crl_in_exp_pct = 0;
	_crl_g0_end_on_sharp = false;
	_crl_g0_end_on_max_l = 0;
	_crl_g0_end_on_max_v = 0;
	_crl_g0_end_on_tip_s = 0;
	_crl_g0_end_on_tip_v = 0;

	_crl_bias_avg_size = 0;
	_crl_bias_use_density = false;
	_crl_tip = 0;
	_crl_tip_min_ticks = 0;
	_crl_tip_end_l = 0;

	_tick_size = 0;
	_tick_value = 0;
	_waves_on = false;
	_out_file = false;
	_out_file_boxes = false;
	_out_file_blossom = tc_range<long>(-1, -1);
	_out_file_boxes_trace = false;
	_out_file_datapoints = false;
	_out_file_points = false;
	_out_file_points_trace = false;
	_report_old_trades = false;
	_report_old_depth = false;
	_report_scope_sanity = false;

}
//======================================================================
void LFMarketModel::load()
{
	std::string base = "LFModel:" + _momentum + ":" + _market + ":";
	
	_waves_on = Z::get_setting_bool(base + "waves_on", true);
	_out_file = Z::get_setting_bool(base + "out_file", true);
	_out_file_boxes = Z::get_setting_bool(base + "out_file_boxes", false);
	_out_file_blossom = tc_range<long>(Z::get_setting(base + "out_file_blossom", "-1:-1"));
	_out_file_boxes_trace = Z::get_setting_bool(base + "out_file_boxes_trace", false);
	_out_file_datapoints = Z::get_setting_bool(base + "out_file_datapoints", false);
	_out_file_points = Z::get_setting_bool(base + "out_file_points", false);
	_out_file_points_trace = Z::get_setting_bool(base + "out_file_points_trace", false);
	_report_old_trades = Z::get_setting_bool(base + "report_old_trades", false);
	_report_old_depth = Z::get_setting_bool(base + "report_old_depth", false);
	_report_scope_sanity = Z::get_setting_bool(base + "report_scope_sanity", false);
	base = "LFModel:" + _market + ":";
	
	long tt_msec = Z::get_setting_integer(base + "tick_timeout_msec", 1000);
	long tt_secs = long(tt_msec / 1000);
	_tick_timeout = ACE_Time_Value(tt_secs, long((tt_msec - (tt_secs * 1000)) * 1000));
	
	_tick_timeout_max = ACE_Time_Value(Z::get_setting_integer(base + "tick_timeout_max_sec", 10), 0);

	tt_msec = Z::get_setting_integer(base + "same_trade_timeout_msec", 200);
	tt_secs = long(tt_msec / 1000);
	_same_trade_timeout = ACE_Time_Value(tt_secs, long((tt_msec - (tt_secs * 1000)) * 1000));

	_crl_decay_l_factor = Z::get_setting_double(base + "crl_decay_l_factor", 1);
	_crl_decay_v_factor = Z::get_setting_double(base + "crl_decay_v_factor", 1);

	_smoothness_l = Z::get_setting_double(base + "smoothness_l", 3);
	_smoothness_min_l = Z::get_setting_double(base + "smoothness_min_l", 0.33);
	_smoothness_decay_factor = Z::get_setting_double(base + "smoothness_decay_factor", 2);
	_smoothness_weight_round = Z::get_setting_double(base + "smoothness_weight_round", 0);
	_smoothness_activity_round = Z::get_setting_double(base + "smoothness_activity_round", 0);
	_smoothness_corr_l = Z::get_setting_double(base + "smoothness_corr_l", _smoothness_l);
	_smoothness_vma_linear = Z::get_setting_bool(base + "smoothness_vma_linear", false);
	_crl_split_charge = Z::get_setting_bool(base + "crl_split_charge", false);
	
	_crl_high = Z::get_setting_double(base + "crl_high", 0.7);
	_crl_low = Z::get_setting_double(base + "crl_low", 0.3);
	_crl_high_offset = Z::get_setting_double(base + "crl_high_offset", 0.2);
	_crl_low_offset = Z::get_setting_double(base + "crl_low_offset", 0.2);

	_crl_discrepancy = Z::get_setting_double(base + "crl_discrepancy", -1);
	_crl_split_negative = Z::get_setting_double(base + "crl_split_negative", -1);
	_crl_split_offset = Z::get_setting_double(base + "crl_split_offset", 1000);
	_crl_split_min = Z::get_setting_double(base + "crl_split_min", 1000);
	_crl_split_side_charge = Z::get_setting_bool(base + "crl_split_side_charge", false);
	_crl_split_side_min= Z::get_setting_double(base + "crl_split_side_min", 3);
	_crl_tip = Z::get_setting_double(base + "crl_tip", 0.5);
	_crl_tip_min_ticks = Z::get_setting_double(base + "crl_tip_min_ticks", 11);
	_crl_tip_end_l = Z::get_setting_double(base + "crl_tip_end_l", 1);

	
	_crl_box_sharp_slope = tc_range<double>(Z::get_setting(base + "crl_box_sharp_slope", "1000:1000"));
	_crl_box_sharp_min_v = tc_range<double>(Z::get_setting(base + "crl_box_sharp_min_v", "0:0"));
	_crl_box_sharp_slope_decline = Z::get_setting_double(base + "crl_box_sharp_slope_decline", 5);
	_crl_box_sharp_scope_decline = Z::get_setting_double(base + "crl_box_sharp_scope_decline", 0);
	_crl_box_sharp_len = Z::get_setting_double(base + "crl_box_sharp_len", 60);
	_crl_box_sharp_scope_exact = Z::get_setting_bool(base + "crl_box_sharp_scope_exact", false);
	_crl_box_sharp_scope_decline_stop = Z::get_setting_bool(base + "crl_box_sharp_scope_decline_stop", false);
	_crl_box_sharp_scope_charge_etalon = Z::get_setting_bool(base + "crl_box_sharp_scope_charge_etalon", false);

	_crl_box_sharp_scope_based = Z::get_setting_bool(base + "crl_box_sharp_scope_based", false);

	_crl_box_min_slope = Z::get_setting_double(base + "crl_box_min_slope", 7);
	_crl_box_bounce_p_pct = Z::get_setting_double(base + "crl_box_bounce_p_pct", 7);
	_crl_box_bounce_l_pct = Z::get_setting_double(base + "crl_box_bounce_l_pct", 1);
	_crl_box_bounce_min_l = Z::get_setting_double(base + "crl_box_bounce_min_l", 0.02);
	_crl_continuous = Z::get_setting_double(base + "crl_continuous", 0.6);
	_crl_continuous_straight = Z::get_setting_double(base + "crl_continuous_straight", 0.9);
	_crl_continuity_weighted = Z::get_setting_bool(base + "crl_continuity_weighted", false);
	_crl_bias_full_action = Z::get_setting_bool(base + "crl_bias_full_action", false);
	_crl_zero_exp_on_prev = Z::get_setting_bool(base + "crl_zero_exp_on_prev", false);
	_crl_pin_joined =  Z::get_setting_bool(base + "crl_pin_joined", false);
	_crl_join_check_parrent = Z::get_setting_bool(base + "crl_join_check_parrent", false);
	_crl_zero_exp_on_prev_charge = Z::get_setting_bool(base + "crl_zero_exp_on_prev_charge", false);
	_crl_nojoin_on_bounce = Z::get_setting_bool(base + "crl_nojoin_on_bounce", false);
	_crl_in_exp_pct = Z::get_setting_double(base + "crl_in_exp_pct", 0);
	_crl_g0_end_on_sharp = Z::get_setting_bool(base + "crl_g0_end_on_sharp", false);
	_crl_g0_end_on_max_l = Z::get_setting_double(base + "crl_g0_end_on_max_l", 1001);
	_crl_g0_end_on_max_v = Z::get_setting_double(base + "crl_g0_end_on_max_v", 1001);
	_crl_g0_end_on_tip_s = Z::get_setting_double(base + "crl_g0_end_on_tip_s", 1001);
	_crl_g0_end_on_tip_v = Z::get_setting_double(base + "crl_g0_end_on_tip_v", 1001);

	_crl_bias_avg_size = Z::get_setting_integer(base + "crl_bias_avg_size", 1);
	_crl_bias_use_density = Z::get_setting_bool(base + "crl_bias_use_density", false);
	
	_back_trace.init(Z::get_setting(base + "back_trace", "20111111_111111:20111111_111111"));
	_box_trace.init(Z::get_setting(base + "box_trace", "20111111_111111:20111111_111111"));

	_spearman_corr = Z::get_setting_bool(base + "spearman_corr", false);
	_use_depth_price = Z::get_setting_bool(base + "use_depth_price", false);

}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFMarketModel& d_)
{
	os << "," << d_._market;
	os << "," << d_._momentum;
	return os;
}
//======================================================================
