/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_SIGNAL_HIGHLOW_H
#define LF_SIGNAL_HIGHLOW_H

#include "znet/ZNet.h"
#include "leaf/LFSignal.h"

//======================================================================
class LEAF_Export LFSignal_HighLow : public LFSignal_Market
{
public:
	LFSignal_HighLow(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_ = "HighLow");
	virtual ~LFSignal_HighLow();

	virtual void trading();

	virtual void process_request(const LFStrategyRequest& r_);

protected:
	double		_pre_avg_ticks;

	double		_box_E_charge_dir_roll;
	double		_box_E_charge_dir_bp;
	tc_vector<double>	_strong_qty_set;
	tc_vector<double>	_crl_box_charge_sets;
	bool				_crl_box_charge_sets_inter;
	double				_crl_box_min_charge;

	bool				_repeater;
	double				_repeater_adjust_factor;
	double				_repeater_adjust_limit_factor;

	bool		_box_E_on_250;
	bool		_box_E_on_sharp;
	tc_range<double>	_box_E_min_p_charge;
	double		_box_E_repeat_p_slope;
	double		_box_E_repeat_p_slope_tip;
	double		_box_E_repeat_p_slope_child;
	double		_box_E_repeat_child;
	double		_box_E_repeat_vol;
	double		_box_E_repeat_p_slope_peak;
	bool 		_box_E_repeat_p_slope_peak_confirm;
	bool 		_box_E_repeat_p_slope_peak_no_enter;
	double		_box_E_repeat_p_slope_shadow;
	double		_box_E_repeat_p_slope_stitch;
	bool		_box_E_report_shadow;
	bool		_box_E_report_stitch;
	bool		_box_E_report_slope_peak;
	double		_box_E_repeat_charge;
	bool		_box_E_repeat_stitch;
	tc_vector<double>	_box_E_charge_exp_bounce_avg;
	
	tc_range<double> _box_E_diff_p_charge;
	tc_range<double> _box_E_diff_t_charge;
	double			 _box_E_diff_p_charge_micro;
	tc_vector<double>  _box_E_diff_t_charge_micro;

	tc_range<double> _box_E_repeat_slope_diff;
	double			 _box_E_repeat_slope_diff_t;
	double			 _box_E_repeat_slope_diff_p;
	tc_range<double> _box_E_repeat_slope_diff_valid;

	bool		_box_E_repeat_child_reverse;
	bool		_box_E_report_failed_matches;

	double		_box_E_b_len_charge_micro;

	tc_vector<double>	_box_E_charge_q_multiplier;

	double		_box_X_Micro_pct;
	double		_box_X_Scope_pct;
	double		_box_X_MicroConf_bpct;
	double		_box_X_MicroConf_pct;
	double		_box_X_Accel_pct;
	tc_vector<double>		_box_X_Micro_ticks;
	double		_box_X_MicroConf_ticks;
	bool		_box_X_contra_always;
	bool		_box_X_BounceExact;

	bool		_box_E_ignore_loss;
	bool		_box_E_ignore_loss_outside;
	double		_box_E_avg_only_limit;
	double		_box_E_avg_rr_factor;
	tc_vector<double>	_box_E_cover_loss_pos_limit;
	tc_vector<bool>		_box_E_old_average;
	bool		_box_E_pias_allied;
	double		_box_E_min_density;
	bool		_box_E_pias_contra;
	bool		_box_E_pias_shadow;
	bool		_box_E_pias_shadow_stitch;

	bool		_trends_out_file;
	bool		_tnt_out_file;


	double		_exit_trend_end_pct;
	bool		_box_E_repeat_force;
	bool		_button_entry;

	long		_enters_count;

	double		_request_qty;

	LFBox	_check_box;

	LFBox	_enter_sharp_chargemicro;
	LFBox	_sharp_charge;
	LFBox	_sharp_charge_conf;
	LFBox	_sharp_charge_bounce;

	LFBox	_enter_sharp_charge;
	LFBoxPair _enter_sharp_prev;

	LFInfo_Position		_m_data;

	void process_repeater();
	double repeater_enter_qty(double requested_q_, double existed_q_);
	void process_box();
	void process_250();

	void set_sharp_micro(const LFBox& bc_, const LFBounce& b_, const std::string& action_);
	void enter_sharp_micro();
	void enter_sharp();
	void exit_sharp_bounce();
	void process_request_sharp(const LFStrategyRequest& r_);

	void process_sharp();

	void report_sharp_trigger(bool active_);

};

//======================================================================
class LEAF_Export LFSignal_Repeater : public LFSignal_Market
{
public:
	LFSignal_Repeater(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_ = "Repeater");
	virtual ~LFSignal_Repeater();

	virtual void trading();

	virtual void process_request(const LFStrategyRequest& r_) {}

protected:
	LFInfo_Position		_m_data;

};

//======================================================================
#endif /*LF_SIGNAL_HIGHLOW_H*/
