/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFSignal_HighLow.h"
#include "leaf/LFStrategy.h"

#include <sstream>
#include <cmath>
#include <iomanip>

//======================================================================
LFSignal_HighLow::LFSignal_HighLow(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_)
	:LFSignal_Market(strat_, d_, type_),
	_button_entry(false),
	_enters_count(0),
	_request_qty(0)

{
	if (_on)
	{
		_strong_qty_set = tc_vector<double>(Z::get_setting(_resource_base + "strong_qty_set", "30"));

		_crl_box_charge_sets = tc_vector<double>(Z::get_setting(_resource_base + "crl_box_charge_sets", "75.1"));
		_crl_box_charge_sets_inter = Z::get_setting_bool(_resource_base + "crl_box_charge_sets_inter", false);
		_crl_box_min_charge = Z::get_setting_double(_resource_base + "crl_box_min_charge", 75.1);

		
		_repeater = Z::get_setting_bool(_resource_base + "repeater", false);
		_repeater_adjust_factor = Z::get_setting_double(_resource_base + "repeater_adjust_factor", 0);
		_repeater_adjust_limit_factor = Z::get_setting_double(_resource_base + "repeater_adjust_limit_factor", 0);
		_box_E_on_250 = Z::get_setting_bool(_resource_base + "box_E_on_250", false);
		_box_E_on_sharp = Z::get_setting_bool(_resource_base + "box_E_on_sharp", false);

		_box_E_charge_dir_roll = Z::get_setting_double(_resource_base + "box_E_charge_dir_roll", -100);
		_box_E_charge_dir_bp = Z::get_setting_double(_resource_base + "box_E_charge_dir_bp", 0);
		_box_E_charge_q_multiplier = tc_vector<double>(Z::get_setting(_resource_base + "box_E_charge_q_multiplier", "1"));
		_box_E_min_p_charge = tc_range<double>(Z::get_setting(_resource_base + "box_E_min_p_charge", "75.1:1001"));
		_box_E_repeat_p_slope = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope", 2);
		_box_E_repeat_p_slope_tip = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope_tip", 0);
		_box_E_repeat_p_slope_child = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope_child", 0);
		_box_E_repeat_child = Z::get_setting_double(_resource_base + "box_E_repeat_child", 0.3);
		_box_E_repeat_vol = Z::get_setting_double(_resource_base + "box_E_repeat_vol", 0);

		_box_E_repeat_p_slope_peak = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope_peak", 2);
		_box_E_repeat_p_slope_peak_confirm = Z::get_setting_bool(_resource_base + "box_E_repeat_p_slope_peak_confirm", false);
		_box_E_repeat_p_slope_peak_no_enter = Z::get_setting_bool(_resource_base + "box_E_repeat_p_slope_peak_no_enter", false);
		_box_E_repeat_p_slope_shadow = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope_shadow", 2);
		_box_E_repeat_p_slope_stitch = Z::get_setting_double(_resource_base + "box_E_repeat_p_slope_stitch", 0);
		_box_E_report_shadow = Z::get_setting_bool(_resource_base + "box_E_report_shadow", false);
		_box_E_report_stitch = Z::get_setting_bool(_resource_base + "box_E_report_stitch", false);
		_box_E_report_slope_peak = Z::get_setting_bool(_resource_base + "box_E_report_slope_peak", false);

		_box_E_repeat_charge = Z::get_setting_double(_resource_base + "box_E_repeat_charge", 11);
		_box_E_repeat_stitch = Z::get_setting_bool(_resource_base + "box_E_repeat_stitch", false);
		_box_E_b_len_charge_micro = Z::get_setting_double(_resource_base + "box_E_b_len_charge_micro", 0);
		_box_E_charge_exp_bounce_avg = tc_vector<double>(Z::get_setting(_resource_base + "box_E_charge_exp_bounce_avg", "20"));
		_box_E_repeat_child_reverse = Z::get_setting_bool(_resource_base + "box_E_repeat_child_reverse", false);
		_box_E_report_failed_matches = Z::get_setting_bool(_resource_base + "box_E_report_failed_matches", false);
		_box_E_diff_p_charge = tc_range<double>(Z::get_setting(_resource_base + "box_E_diff_p_charge", "7:25"));
		_box_E_diff_t_charge = tc_range<double>(Z::get_setting(_resource_base + "box_E_diff_t_charge", "1:1001"));
		_box_E_diff_p_charge_micro = Z::get_setting_double(_resource_base + "box_E_diff_p_charge_micro", 33);
		_box_E_diff_t_charge_micro = tc_vector<double>(Z::get_setting(_resource_base + "box_E_diff_t_charge_micro", "100"));

		_box_E_repeat_slope_diff = tc_range<double>(Z::get_setting(_resource_base + "box_E_repeat_slope_diff", "1:0"));
		_box_E_repeat_slope_diff_t = Z::get_setting_double(_resource_base + "box_E_repeat_slope_diff_t", 1);
		_box_E_repeat_slope_diff_p = Z::get_setting_double(_resource_base + "box_E_repeat_slope_diff_p", 7);
		_box_E_repeat_slope_diff_valid = tc_range<double>(Z::get_setting(_resource_base + "box_E_repeat_slope_diff_valid", "1:0"));

		_box_X_Micro_pct = Z::get_setting_double(_resource_base + "box_X_Micro_pct", 0);
		_box_X_Scope_pct = Z::get_setting_double(_resource_base + "box_X_Scope_pct", 100);
		_box_X_MicroConf_bpct = Z::get_setting_double(_resource_base + "box_X_MicroConf_bpct", 0);
		_box_X_MicroConf_pct = Z::get_setting_double(_resource_base + "box_X_MicroConf_pct", 0);
		_box_X_Accel_pct = Z::get_setting_double(_resource_base + "box_X_Accel_pct", 0);

		_box_X_Micro_ticks = tc_vector<double>(Z::get_setting(_resource_base + "box_X_Micro_ticks", "0"));
		_box_X_MicroConf_ticks = Z::get_setting_double(_resource_base + "box_X_MicroConf_ticks", -1001);
		_box_X_contra_always = Z::get_setting_bool(_resource_base + "box_X_contra_always", false);
		_box_X_BounceExact = Z::get_setting_bool(_resource_base + "box_X_BounceExact", false);

		_box_E_repeat_force = Z::get_setting_bool(_resource_base + "box_E_repeat_force", true);
		_box_E_ignore_loss = Z::get_setting_bool(_resource_base + "box_E_ignore_loss", false);
		_box_E_ignore_loss_outside = Z::get_setting_bool(_resource_base + "box_E_ignore_loss_outside", false);
		_box_E_avg_only_limit = Z::get_setting_double(_resource_base + "box_E_avg_only_limit", 1001);
		_box_E_avg_rr_factor = Z::get_setting_double(_resource_base + "box_E_avg_rr_factor", 0);
		_box_E_cover_loss_pos_limit = tc_vector<double>(Z::get_setting(_resource_base + "box_E_cover_loss_pos_limit", "30"));
		_box_E_old_average = tc_vector<bool>(Z::get_setting(_resource_base + "box_E_old_average", "true"));
		_box_E_pias_allied = Z::get_setting_bool(_resource_base + "box_E_pias_allied", true);
		_box_E_min_density = Z::get_setting_double(_resource_base + "box_E_min_density", 0);
		_box_E_pias_contra = Z::get_setting_bool(_resource_base + "box_E_pias_contra", true);
		_box_E_pias_shadow = Z::get_setting_bool(_resource_base + "box_E_pias_shadow", true);
		_box_E_pias_shadow_stitch = Z::get_setting_bool(_resource_base + "box_E_pias_shadow_stitch", false);

		_trends_out_file = Z::get_setting_bool(_resource_base + "trends_out_file", false);
		_tnt_out_file = Z::get_setting_bool(_resource_base + "tnt_out_file", false);

		_exit_trend_end_pct = Z::get_setting_double(_resource_base + "exit_trend_end_pct", 100);

		_pre_avg_ticks = Z::get_setting_double(_resource_base + "pre_avg_ticks", 3);
	}

}
LFSignal_HighLow::~LFSignal_HighLow()
{
}

//======================================================================
void LFSignal_HighLow::set_sharp_micro(const LFBox& bc_, const LFBounce& b_, const std::string& action_)
{
	_enter_sharp_chargemicro = bc_;
	_enter_sharp_chargemicro._comment = action_;
	LF::model_send(_enter_sharp_charge, "CONF_" + action_, _d._t, _ematches_out_file, _d._box_key + "_enter");
	if (_sharp_charge._side == _enter_sharp_charge._side)
		_sharp_charge.set_ended();

	LFMatchPtr m;
	m.reset(new LFMatch("micro", "micro_" + get_name(), false));
	m->reset(_enter_sharp_chargemicro._side->contra_s(), _d.dp());
	m->_comment = action_;
	//	m->_m_scale = (long)_d.scopes().find_bounce_scope(_enter_sharp_chargemicro._w);

	m->_box = _enter_sharp_chargemicro;
	m->_m_v = b_._cluster_v;
	m->_m_l = b_._cluster_l;
	m->_peak_v = _enter_sharp_chargemicro._w._cluster_v;
	m->_peak_vol = bc_._w._cluster_vol;
	m->_peak_trn = bc_._w._cluster_tran;
	m->_peak_den = bc_._w._cluster_density;
	m->_peak_frq = bc_._w._cluster_frequency;
	m->_m_vol = b_._cluster_vol;
	m->_m_trn = b_._cluster_tran;
	m->_m_den = b_._cluster_density;
	m->_m_frq = b_._cluster_frequency;
	m->_m_vol_pct = b_._vol_pct;
	m->_m_b_pct = b_._v_pct;
	m->_m_ratio = b_._l_pct;

	LF::model_send(*m, "", _d._t, _ematches_out_file, "e_" + m->_m_name);
	_history.push_back(m);
}

//======================================================================
void LFSignal_HighLow::enter_sharp_micro()
{
	if (!_enter_sharp_charge.valid() || _enter_sharp_charge.equal(_enter_sharp_chargemicro))
	{
		_sharp_charge_conf.reset();
		return;
	}
	if (!_sharp_charge_conf.valid() || !_sharp_charge_conf._s_dp->equal(*_enter_sharp_charge._s_dp))
	{
		_sharp_charge_conf.reset(_enter_sharp_charge._side,	_enter_sharp_charge._s_dp, _d._model._tick_size);
		_d.init_merge(_sharp_charge_conf);

		LF::model_send(_sharp_charge_conf, "RESET", _d._t, _d._model._out_file_boxes, _d._box_key + "_conf");
	}
	if (!_d._box.equal(_check_box) && _sharp_charge_conf._c_dp->older(*_d._latter_box._c_dp))
	{
		_d.init_merge(_sharp_charge_conf);
		LF::model_send(_sharp_charge_conf, "MERGE", _d._t, _d._model._out_file_boxes, _d._box_key + "_conf");
	}

	LFBox bc = _sharp_charge_conf;
	bc.merge(_d._box);
	_d.set_scope(bc);
	_d.set_wave(bc, _enter_sharp_charge._w);
	_enter_sharp_charge._w.set_identity(bc._w);

	LFBounce b;
	_d.set_bounce(b, bc._w, bc._c_dp);

	if (bc._w._id._scale < 0)
	{
		LF::model_send(bc, "BC", _d._t, _d._model._out_file_boxes, _d._box_key + "_no_scope");
		LF::model_send(_sharp_charge_conf, "conf", _d._t, _d._model._out_file_boxes, _d._box_key + "_no_scope");
		LF::model_send(_d._box, "d_box", _d._t, _d._model._out_file_boxes, _d._box_key + "_no_scope");
		set_sharp_micro(bc, b, "no_scope");
	}
	else
	{
		double indx = LF::by_val(_crl_box_charge_sets, bc._w._cluster_v, _crl_box_charge_sets_inter);

		const LFScope& scope = _d.scopes()[_enter_sharp_charge._w._id._scale];
		const LFBox& scope_peak = scope[bc._w._id._roll];
		const LFCluster& scope_tip = scope_peak._tip;
		
//		double slt = (scope_tip._cluster_v - scope_tip.value(bc._c_p, _d._model._tick_size)) / (scope_tip._cluster_l + scope_tip.length(bc._c_t));
		double slt = LF::slope(bc._side, scope_tip._cluster_dp, bc._c_dp, _d._model._tick_size);

		size_t up_scp = _enter_sharp_charge._w._id._scale + 1;
		if (up_scp == _d.scopes().size())
			up_scp = _enter_sharp_charge._w._id._scale;
		const LFScope& up_scope = _d.scopes()[up_scp];

		const LFCluster* scope_child = 0;
		double slc = 0;
		if (bc._w._id._scale > 0)
		{
			size_t pin;
			size_t bin;
			const LFScope& child = _d.scopes()[bc._w._id._scale - 1];
			child.find_indices(bc._w, pin, bin);
			if (pin < child.size())
			{
				scope_child = &child[pin]._w;
//				slc = (scope_child->_cluster_v - scope_child->value(bc._c_p, _d._model._tick_size)) / (scope_child->_cluster_l + scope_child->length(bc._c_t));
				slc = LF::slope(bc._side, scope_child->_cluster_dp, bc._c_dp, _d._model._tick_size);
			}
		}

		if (b._v_pct > _box_E_diff_p_charge_micro)
		{
			set_sharp_micro(bc, b, "ppct");
		}
		else if (_enter_sharp_charge._side->sign()*(_d._box._d_pias) < 0 &&
			b._l_pct > LF::by_ind(_box_E_diff_t_charge_micro, indx, _crl_box_charge_sets_inter) && b._cluster_l > _box_E_b_len_charge_micro)
		{
			set_sharp_micro(bc, b, "tpct");
		}
		else if (bc._w._cluster_s > bc.p_slope()*_box_E_repeat_p_slope)
		{
			set_sharp_micro(bc, b, "slb");
		}
		else if (_box_E_repeat_p_slope_tip > 0 && !TCCompare::is_NaN(slt)
			&& bc._w._cluster_s > slt*_box_E_repeat_p_slope_tip)
		{
			std::ostringstream os;
			os << std::setprecision(1) << std::fixed;
			os << "slt[" << slt << "_" << _enter_sharp_charge._w._cluster_s << "]";
			LF::model_send(scope_tip, os.str(), _d._t, _d._model._out_file_boxes, _d._box_key + "_sl");
			set_sharp_micro(bc, b, os.str());
		}
		else if (_box_E_repeat_p_slope_child > 0 && !TCCompare::is_NaN(slc)
			&& scope_child != 0
			&& scope_child->_cluster_v >= bc._w._cluster_v*_box_E_repeat_child
			&& bc._w._cluster_s > slc*_box_E_repeat_p_slope_child)
		{
			std::ostringstream os;
			os << std::setprecision(1) << std::fixed;
			os << "slc[" << slc << "_" << _enter_sharp_charge._w._cluster_s << "]";
			LF::model_send(*scope_child, os.str(), _d._t, _d._model._out_file_boxes, _d._box_key + "_sl");
			set_sharp_micro(bc, b, os.str());
		}
		else if (_box_E_repeat_child_reverse
			&& scope_child != 0
			&& slc < 0)
		{
			std::ostringstream os;
			os << std::setprecision(1) << std::fixed;
			os << "slcr[" << slc << "]";
			LF::model_send(*scope_child, os.str(), _d._t, _d._model._out_file_boxes, _d._box_key + "_sl");
			set_sharp_micro(bc, b, os.str());
		}
//		else if (_enter_sharp_charge._w._dp->_t < up_scope[up_scope.bounce_ind(_enter_sharp_charge._side)]._w._dp->_t)
		else if (_enter_sharp_charge._w.older(up_scope[up_scope.bounce_ind(_enter_sharp_charge._side)]._w))
		{
			set_sharp_micro(bc, b, "up_scope");
		}
	}
}
//======================================================================
void LFSignal_HighLow::process_request(const LFStrategyRequest& r_)
{
	if (r_._st_action == LF::satSHARP)
		process_request_sharp(r_);
}
void LFSignal_HighLow::process_request_sharp(const LFStrategyRequest& r_)
{
	if (r_._st_active)
	{
		Z::report_info("%s -- Activate_sharp - Activate (side %d)", get_name().c_str(), r_._order_side);
		if (!_d._sharp_box.valid())
		{
			Z::report_error("%s -- Activate_sharp - no valid sharp box", get_name().c_str());
			return;
		}
		if (_d._sharp_box._side->value() != r_._order_side)
		{
			Z::report_error("%s -- Activate_sharp - wrong side", get_name().c_str());
			return;
		}
		if (_enter_sharp_charge.valid())
			_enter_sharp_charge.set_ended();

		_sharp_charge.reset_from(_d._sharp_box);
		_request_qty = r_._q;
		LF::model_send(_sharp_charge, "R_SET", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
		_action = LF::paPreEnter;
	}
	else
	{
		Z::report_info("%s -- process_request_sharp - Deactivate (side %d)", get_name().c_str(), r_._order_side);
		if (!_sharp_charge.valid())
		{
			Z::report_error("%s -- Deactivate_sharp - no valid sharp charge", get_name().c_str());
			return;
		}
		if (_sharp_charge._side->value() != r_._order_side)
		{
			Z::report_error("%s -- Deactivate_sharp - wrong side", get_name().c_str());
			return;
		}
		if (_enter_sharp_charge.valid() && !_enter_sharp_charge.ended()
			&& _sharp_charge._side == _enter_sharp_charge._side
			&& _sharp_charge._s_dp->_t <= _enter_sharp_charge._c_dp->_t)
		{
			Z::report_error("%s -- Deactivate_sharp - already entered", get_name().c_str());
			return;
		}
		_sharp_charge.reset();
	}
}
void LFSignal_HighLow::enter_sharp()
{
	if (!_d._box.equal(_check_box))
	{
		if (_enter_sharp_charge.valid() && !_enter_sharp_charge.ended())
		{
			if (_enter_sharp_charge._w._id._scale < 0)
			{
				_enter_sharp_charge.set_ended();
				LF::model_send(_enter_sharp_charge, "END_no_scope", _d._t, _ematches_out_file, _d._box_key + "_enter");
			}
			else if (!_enter_sharp_charge.turned() && _enter_sharp_charge._side->zsign(_d.scopes()[_enter_sharp_charge._w._id._scale][0]._pias) > 0)
			{
				_enter_sharp_charge.set_turned();
				LF::model_send(_enter_sharp_charge, "TURNED", _d._t, _ematches_out_file, _d._box_key + "_enter");
			}
			else
			{
				size_t up_scp = _enter_sharp_charge._w._id._scale + 1;
				if (up_scp == _d.scopes().size())
					up_scp = _enter_sharp_charge._w._id._scale;
				const LFScope& up_scope = _d.scopes()[up_scp];
				size_t bin = up_scope.bounce_ind(_enter_sharp_charge._side);
				if (bin < up_scope.size()
					&& up_scope[bin].valid()
					&& _enter_sharp_charge._w._dp->_t < up_scope[bin]._w._dp->_t)
				{
					_enter_sharp_charge.set_ended();
					LF::model_send(_enter_sharp_charge, "END_up_scope", _d._t, _ematches_out_file, _d._box_key + "_enter");
				}
			}

		}

		bool existing = false;
		if (_sharp_charge.valid() && !_sharp_charge.ended()
			&& _sharp_charge._c_dp->_t < _d._latter_box._c_dp->_t)
		{
//			_sharp_charge.merge(_d._latter_box, _sharp_charge._side->sign());
			_d.init_merge(_sharp_charge);
			_d.set_scope(_sharp_charge);
			LF::model_send(_sharp_charge, "MERGE", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");

			if (_sharp_charge._w._cluster_s > _sharp_charge.p_slope()*_box_E_repeat_p_slope)
			{
				LF::model_send(_sharp_charge, "RESET_FLAT", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
				_sharp_charge.set_ended();
			}
			else if (_enter_sharp_charge.valid() && !_enter_sharp_charge.ended()
				&& _sharp_charge._side == _enter_sharp_charge._side
				&& _sharp_charge._s_dp->_t <= _enter_sharp_charge._c_dp->_t)
			{
				_d.set_wave(_sharp_charge, _enter_sharp_charge._w);
				existing = true;
				//if (_enter_sharp_charge._w._cluster_s > _sharp_charge._w._cluster_s*_box_E_repeat_p_slope_peak
				//	|| _enter_sharp_charge._w._cluster_s > _sharp_charge._p_slope*_box_E_repeat_p_slope)

				//{
				//	LF::model_send(_sharp_charge, "RESET_FLAT", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
				//	_sharp_charge.set_ended();
				//}
			}
			else
			{
				if (_sharp_charge._w._cluster_s <= _d._model._crl_box_min_slope)

				{
					LF::model_send(_sharp_charge, "RESET_SLOPE", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
					_sharp_charge.set_ended();
				}
			}

		}
// Check stitching first
		if (_box_E_repeat_stitch
			&& _enter_sharp_charge.valid() && !_enter_sharp_charge.ended()
			&& (!_sharp_charge.valid() || _sharp_charge.ended() || _enter_sharp_charge._s_dp->_t < _sharp_charge._s_dp->_t)
			&& _d._latter_box._side == _enter_sharp_charge._side)
		{
			//double d_value = -_enter_sharp_charge._side->v(_d._latter_box._max_p, _enter_sharp_charge._max_p, _d._model._tick_size);
			//double slope = (_enter_sharp_charge._w._cluster_v + d_value) / LF::length_from_t(_d._latter_box._max_t - _enter_sharp_charge._s_t);
			double d_value = LF::value_i(_enter_sharp_charge._side, _enter_sharp_charge._max_dp, _d._latter_box._max_dp, _d._model._tick_size);
			double slope = LF::slope_i(_enter_sharp_charge._side, _enter_sharp_charge._w_dp, _d._latter_box._max_dp, _d._model._tick_size);;

			if (d_value > _box_E_repeat_charge
//				&& _enter_sharp_charge._w._cluster_s > _d._model._crl_box_min_slope*_box_E_repeat_p_slope_peak
				)
			{
				if (slope*_box_E_repeat_p_slope_peak > _enter_sharp_charge._w._cluster_s)
				{
					LFBox sharp_charge;
					sharp_charge.reset(_enter_sharp_charge._side, _enter_sharp_charge._s_dp, _d._model._tick_size);
					_d.init_merge(sharp_charge);
					if (_d._latter_box._max_dp->_t == sharp_charge._max_dp->_t)
					{
						std::string comment = "STITCH";
						if (_sharp_charge.valid() && !_sharp_charge.ended())
							comment += "_REPLACE";
						_sharp_charge.reset_from(sharp_charge);
						_d.set_scope(_sharp_charge);
						_d.set_wave(_sharp_charge, _enter_sharp_charge._w);
						existing = true;
						LF::model_send(_sharp_charge, comment, _d._t, _ematches_out_file, _d._box_key + "_bc");

						_d._p_store->report(comment, get_name(), _sharp_charge, _box_E_report_stitch);
					}
					else
					{
						LF::model_send(sharp_charge, "no_stitch_max", _d._t, _ematches_out_file, _d._box_key + "_bc");
					}
				}
				else
				{
					std::ostringstream os;
					os << std::setprecision(1) << std::fixed
						<< "no_stitch_slope_" << slope;
					LF::model_send(_d._latter_box, os.str(), _d._t, _ematches_out_file, _d._box_key + "_bc");
				}
			}

		}

// Check sharp
		if (_d._sharp_box.valid()
			&& _d._sharp_box._w._cluster_v > _crl_box_min_charge
			&& _d._sharp_box._w._cluster_s > _d._model._crl_box_min_slope)
		{
			bool make_bc = true;
			LFCluster stitch_to;
			if (_sharp_charge.valid())
			{
				if (_sharp_charge.ended())
				{
					if (_sharp_charge._side == _d._latter_box._side && _d._latter_box._max_dp->_t <= _sharp_charge._end_t)
					{
						make_bc = false;
						LF::model_send(_sharp_charge, "ended_same", _d._t, _ematches_out_file, _d._box_key + "_bc");
					}
				}
				else if (
//					existing  && 
					_d._sharp_box._side == _sharp_charge._side && !_d._sharp_box.older(_sharp_charge))
				{
					make_bc = false;
					LF::model_send(_sharp_charge, "existing_not_ended", _d._t, _ematches_out_file, _d._box_key + "_bc");
				}
			}
			if (make_bc && _enter_sharp_charge.valid() && !_enter_sharp_charge.ended())
			{
				if (_box_E_pias_contra && _enter_sharp_charge._side != _d._sharp_box._side)
				{
					make_bc = false;
					LF::model_send(_d._sharp_box, "pias_contra", _d._t, _ematches_out_file, _d._box_key + "_bc");
				}
				if (_box_E_pias_shadow && _enter_sharp_charge._side == _d._sharp_box._side && _enter_sharp_charge._max_dp->_t < _d._sharp_box._s_dp->_t)
				{
					_d.set_wave(_d._sharp_box, _enter_sharp_charge._w);
					if (_box_E_pias_shadow_stitch)
						stitch_to = _enter_sharp_charge._w;
					if (_box_E_repeat_p_slope_shadow > 0)
					{
//						double slope = (_enter_sharp_charge._w._cluster_v + _d._sharp_box._w_dv) / LF::length_from_t(_d._sharp_box._max_t - _enter_sharp_charge._s_t);
						double d_v = LF::value_i(_d._sharp_box._side, _enter_sharp_charge._max_dp, _d._sharp_box._max_dp, _d._model._tick_size);
						double slope = LF::slope_i(_d._sharp_box._side, _enter_sharp_charge._w_dp, _d._sharp_box._max_dp, _d._model._tick_size);
						if (d_v <= _box_E_repeat_charge
							|| slope*_box_E_repeat_p_slope_shadow <= _enter_sharp_charge._w._cluster_s)
						{
							make_bc = false;
							LF::model_send(_d._sharp_box, "pias_shadow", _d._t, _ematches_out_file, _d._box_key + "_bc");
							_d._p_store->report("pias_shadow", get_name(), _d._sharp_box, _box_E_report_shadow);
						}
					}
					else
					{
						// expectation based shadow
						const LFScope& scope = _d.scopes()[_enter_sharp_charge._w._id._scale];
						size_t pin = scope.peak_ind(_d._sharp_box._side);
						if (scope[pin]._max_dp->_t < _d._sharp_box._max_dp->_t || !scope[pin]._w._pattern.test(LF::pb_ACCELERATED))
						{
							make_bc = false;
							LF::model_send(_d._sharp_box, "exp_shadow", _d._t, _ematches_out_file, _d._box_key + "_bc");
							_d._p_store->report("exp_shadow", get_name(), _d._sharp_box, _box_E_report_shadow);
						}

					}
				}
			}
				
			if(make_bc)
			{
				LFBox sharp_charge;
				std::string act;
				if (!stitch_to.valid())
				{
					act = "SET";
					sharp_charge.reset_from(_d._sharp_box);
					_d.set_scope(sharp_charge);
				}
				else
				{
					sharp_charge.reset(stitch_to._side,	stitch_to._cluster_dp, _d._model._tick_size);
					_d.init_merge(sharp_charge);
					if (sharp_charge._w._cluster_s > _box_E_repeat_p_slope_stitch)
					{
						act = "SET_STITCH";
					}
					else
					{
						act = "SET_no_stitch";
						sharp_charge.reset_from(_d._sharp_box);
					}

					_d.set_scope(sharp_charge);
					_d.set_wave(sharp_charge, stitch_to);

				}

				_sharp_charge.reset_from(sharp_charge);
				LF::model_send(sharp_charge, act, _d._t, _ematches_out_file, _d._box_key + "_bc");

				if (_box_E_min_p_charge.in(_d._sharp_box._w._cluster_v))
					_action = LF::paPreEnter;
			}

		}

	}

	if (!_sharp_charge.valid() || _sharp_charge.ended())
	{
		_action = LF::paUndefined;
		return;
	}

	LFBox bc = _sharp_charge;
	bc.merge(_d._box, true);
	
	_d.set_scope(bc);


	bool to_enter = false;
	bool first_enter = false;
	bool to_confirm = false;
	bool any_bounce = false;
	double repeate_charge = 0;
	LFBounce b;
	_d.set_bounce(b, bc._w, bc._c_dp);

	double t_pct_limit = _box_E_diff_t_charge.first;
	double p_pct_limit = _box_E_diff_p_charge.first;
	if (_enter_sharp_charge.valid()
		&& _sharp_charge._side == _enter_sharp_charge._side
		&& _sharp_charge._s_dp->_t <= _enter_sharp_charge._c_dp->_t
		&& _box_E_repeat_slope_diff_valid.in(_enter_sharp_charge._w._cluster_s)
		&& _box_E_repeat_slope_diff.in(bc._w._cluster_s / _enter_sharp_charge._w._cluster_s)
		)
	{
		t_pct_limit = _box_E_repeat_slope_diff_t;
		p_pct_limit = _box_E_repeat_slope_diff_p;
		any_bounce = true;
	}

	if (bc._w._id._scale < 0)
	{
		LF::model_send(bc, "NO_SCOPE", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
	}
	else if (b.good(p_pct_limit, t_pct_limit, _d._model._crl_box_bounce_min_l))
	{
		if (_enter_sharp_charge.valid()
			&& _sharp_charge._side == _enter_sharp_charge._side
			&& _sharp_charge._s_dp->_t <= _enter_sharp_charge._c_dp->_t)
		{
			if (bc._max_dp->_t > _enter_sharp_charge._max_dp->_t)
			{
				to_confirm = _box_E_repeat_p_slope_peak_confirm && _enter_sharp_charge._s_dp->_t <= bc._s_dp->_t && _enter_sharp_charge._w._cluster_s > bc._w._cluster_s*_box_E_repeat_p_slope_peak;
				repeate_charge = _sharp_charge._side->v(_enter_sharp_charge._max_dp->_p, bc._max_dp->_p, _d._model._tick_size);

				if (repeate_charge > _box_E_repeat_charge || _sharp_charge._s_dp->_t < _enter_sharp_charge._s_dp->_t)
				{
					std::string comm = "REPEAT";
					to_enter = true;
					_action = LF::paAverage;
					if (to_confirm && _box_E_repeat_p_slope_peak_no_enter)
					{
						set_sharp_micro(bc, b, "slp");
						comm = "slope_peak";
						to_enter = false;
						_action = LF::paUndefined;
						_d._p_store->report("slope_peak", get_name(), bc, _box_E_report_slope_peak);
					}

					LF::model_send(bc, comm, _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
				}
				else
				{
					if (repeate_charge > _box_E_repeat_charge - _pre_avg_ticks)
						_action = LF::paPreAverage;

					LF::model_send(bc, "NO_REPEAT", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
				}
			}
		}
		else
		{
			std::string comm = "FIRST";
			to_enter = true;
			first_enter = true;
			LF::model_send(bc, comm, _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
			if (_box_X_contra_always && !_repeater)
				trade_exit(bc._side, 100, "X_SharpCharge");

			_action = LF::paEnter;
		}
	}
	else
	{
		LF::model_send(bc, "NO_DIFF", _d._t, _d._model._out_file_boxes, _d._box_key + "_merge");
	}

	if (to_enter)
	{
		if (_enter_sharp_charge.valid() && !_enter_sharp_charge.ended()
			&& bc._side == _enter_sharp_charge._side && bc._s_dp->_t <= _enter_sharp_charge._c_dp->_t)
		{
			_d.set_wave(bc, _enter_sharp_charge._w);
		}
		const LFScope& enter_scope = _d.scopes()[bc._w._id._scale];
		const LFBox& enter_roll = enter_scope[bc._w._id._roll];
		if (bc._side->zsign(enter_roll._pias) > 0)
			bc.set_turned();
			
		size_t zero_pin = _d.scopes()[0].find_ind_best(bc._w._side, bc._w._dp->_t);
		LFMatchPtr m;
		m.reset(new LFMatch("sharp", "sharp_" + get_name(), true));
		m->reset(bc._side->contra_s(), _d.dp());
		m->_m_scale = bc._w._id._scale;
		m->_box = bc;
		m->_m_v = b._cluster_v;
		m->_m_l = b._cluster_l;
		m->_peak_v = bc._w._cluster_v;
		m->_peak_vol = bc._w._cluster_vol;
		m->_peak_trn = bc._w._cluster_tran;
		m->_peak_den = bc._w._cluster_density;
		m->_peak_frq = bc._w._cluster_frequency;
		m->_m_vol = b._cluster_vol;
		m->_m_trn = b._cluster_tran;
		m->_m_den = b._cluster_density;
		m->_m_frq = b._cluster_frequency;
		m->_m_vol_pct = b._vol_pct;

		double indx = LF::by_val(_crl_box_charge_sets, m->_peak_v, _crl_box_charge_sets_inter);

		m->_exp_v = m->_peak_v*LF::by_ind(_box_E_charge_exp_bounce_avg, indx, _crl_box_charge_sets_inter) / 100. - m->_m_v;

		m->_m_b_pct = b._v_pct;
		m->_m_ratio = b._l_pct;
		m->_m_repeat_charge = repeate_charge;

		m->_m_loss_v = m->_m_v;
		m->_m_profit_v = _box_report_profit;
		if (!any_bounce)
		{
			m->_m_mask.set(LF::mb_CHARGE_DIFF, m->_m_b_pct > _box_E_diff_p_charge.second);
			m->_m_mask.set(LF::mb_T_DIFF, m->_m_ratio > _box_E_diff_t_charge.second);
		}

		if (_box_E_pias_allied
			&& bc._side->zsign(enter_roll._pias) <= 0)
		{
			m->_mp_mask.set(LF::mp_PIAS);
		}
		if (_box_E_min_density > 0
			&& zero_pin < _d.scopes()[0].size()
			&& _d.scopes()[0][zero_pin]._w.cluster_valid()
			&& _d.scopes()[0][zero_pin]._w._cluster_density > 0
			&& _d.scopes()[0][zero_pin]._w._cluster_density < _box_E_min_density)
		{
			m->_mp_mask.set(LF::mp_DENSITY);
		}
		m->_mp_mask.set(LF::mp_SLOPE, bc._w._cluster_s <= _d._model._crl_box_min_slope);

		_enter_sharp_charge = bc;
		_enter_sharp_charge._comment = m->_m_mask.to_alfa() + m->_mp_mask.to_alfa();
		LF::model_send(_enter_sharp_charge, "SET", _d._t, _ematches_out_file, _d._box_key + "_enter");
		_enter_sharp_chargemicro.reset();

		LF::model_send(*m, "", _d._t, _ematches_out_file, "e_" + m->_m_name);

		if (m->_m_mask.none())
		{
			double realized = 0;
			size_t seq = 0;
			if (m->_side == _entry._side)
			{
				if (_entry._open._q == 0)
					realized += _entry._realized_pnl;
				if (_enter_sharp_charge._s_dp->_t <= _entry._s_t)
					seq = _entry._seq + 1;
			}

			if (realized > 0)
				realized = 0;

			if (_box_E_ignore_loss)
				realized = 0;
			else if (_box_E_ignore_loss_outside && seq == 0)
				realized = 0;

			LFPositionData& pd = _strat.pos_data();
			_entry.reset(m->_side, _d._t, pd, realized, _d._md, seq);

			double q_limit = _strat.position_limit();
			double qty = 0;
			double expected_v = m->_exp_v;
			if (_request_qty != 0)
			{
				qty = _request_qty;
				_button_entry = true;
			}
			else
			{

				double avg_limit = _strat.position_limit();

				if (!_entry._open.good())
					avg_limit = std::min(LF::by_ind(_box_E_cover_loss_pos_limit, indx, _crl_box_charge_sets_inter),
					_strat.position_limit());
				if (_repeater)
					avg_limit = 0;
				_entry._old_average_logic = LF::by_ind(_box_E_old_average, indx, false);
				_entry.avg(expected_v, _enter_sharp_charge._w._dp->_p, _box_E_avg_rr_factor, avg_limit, _d._md, _box_X_Profit_pct);
				LF::model_send(_entry, "SharpCharge", _d._t, _ematches_out_file);

				if (_entry._enter.good())
				{
					m->_qty += _entry._enter._q;
					trade(m->_side->contra_s(), _entry._enter._q, "A_" + m->_m_name);
				}
				double strong_qty = LF::by_ind(_strong_qty_set, indx, _crl_box_charge_sets_inter);

				qty = (long)(strong_qty + 0.5);
				if (m->_mp_mask.any())
					qty = 0;
				q_limit = std::max(_box_E_avg_only_limit, qty);
				q_limit = std::min(q_limit, _strat.position_limit());
				if (q_limit < 0)
					q_limit = 0;

				if (_repeater)
					q_limit = 0;
			}

			_entry.enter(qty, q_limit, _d._md, expected_v, _box_X_Profit_pct);

			LF::model_send(_entry, "SharpCharge", _d._t, _ematches_out_file);
			if (_entry._enter.good())
			{
				m->_qty += _entry._enter._q;
				trade(m->_side->contra_s(), _entry._enter._q, "E_" + m->_m_name);
			}

			_bias_enter_matches.push_back(m);

		}
		else 
		{
			// failed match
			if(first_enter && m->_mp_mask.none())
				trade_exit(_enter_sharp_charge._side, 100, "X_BoxSharpCharge");
			if (_box_E_report_failed_matches)
				_history.push_back(m);
		}
		_request_qty = 0;

	}
}
void LFSignal_HighLow::exit_sharp_bounce()
{
	if (!_enter_sharp_charge.valid())
		return;


	const LFSide* p_side = _enter_sharp_charge._side->contra_s();

	if (!_sharp_charge_bounce.valid() || _sharp_charge_bounce._s_dp->_t != _enter_sharp_charge._max_dp->_t)
	{
		_sharp_charge_bounce.reset(_enter_sharp_charge._side->contra_s(), _enter_sharp_charge._max_dp, _d._model._tick_size);
		_d.init_merge(_sharp_charge_bounce);
		LF::model_send(_sharp_charge_bounce, "RESET", _d._t, _d._model._out_file_boxes, _d._box_key + "_bounce");
	}
	else if (_sharp_charge_bounce.ended())
	{
		return;
	}
	else if (_enter_sharp_chargemicro.equal(_enter_sharp_charge)
//			&& _enter_sharp_chargemicro.ended()
			&& (_entry._side != p_side || !_entry._open.good()))
	{
		_sharp_charge_bounce.set_ended();
		return;
	}
	else if (!_d._box.equal(_check_box)
		&& _sharp_charge_bounce._s_dp->_t < _d._latter_box._c_dp->_t)
	{
		_sharp_charge_bounce.merge(_d._latter_box);
		LF::model_send(_sharp_charge_bounce, "MERGE", _d._t, _d._model._out_file_boxes, _d._box_key + "_bounce");
	}

	double b_pct = 0;
	double bmax_pct = 0;
	double b_t_pct = 0;
	double b_v = 0;
	double b_l = 0;

	LFBox bc_bounce = _sharp_charge_bounce;
	bc_bounce.merge(_d._box, true);

	bmax_pct = bc_bounce._w._cluster_v*100. / _enter_sharp_charge._w._cluster_v;
	b_v = _sharp_charge_bounce._side->sign()*bc_bounce.p_charge();
	b_pct = b_v*100. / _enter_sharp_charge._w._cluster_v;
	b_l = bc_bounce.length();
	b_t_pct = b_l*100. / _enter_sharp_charge._w._cluster_l;


	LFScope& scope_0 = _d.scopes()[0];

	if (_enter_sharp_chargemicro.equal(_enter_sharp_charge))
	{
		//confirmed
		if (_entry._side == p_side)
		{
			double u_ticks = _entry._unrealized_ticks;
			if (scope_0._just_ended_side == p_side)
			{
				double x_pct = _box_X_MicroConf_bpct;
				if (b_pct > x_pct && x_pct > 0)
				{
					_entry.exit_pct(_box_X_MicroConf_pct, _d._md);
					if (_entry._exit.good())
					{
						LFMatchPtr m;
						m.reset(new LFMatch("MicroConf_bpct", "MicroConf_bpct_" + get_name(), false));
						m->reset(_entry._side->contra_s(), _d.dp());
						m->_box = bc_bounce;
						m->_m_v = b_v;
						m->_m_l = b_l;
						m->_m_b_pct = b_pct;
						m->_m_ratio = b_t_pct;
						m->_peak_v = _enter_sharp_chargemicro._w._cluster_v;
						m->_qty = _entry._exit._q;
						LF::model_send(*m, "", _d._t, _ematches_out_file, "x_" + m->_m_name);
						_history.push_back(m);

						std::ostringstream os;
						os << std::setprecision(0) << std::fixed
							<< "X_bpct_" << b_pct << "(" << x_pct << ")";

						trade(_entry._side->contra_s(), -_entry._exit._q, os.str());

						LF::model_send(_entry, "X_MicroConf_bpct", _d._t, _ematches_out_file);
					}
				}
			}
			size_t b_scope = _d.scopes().find_bounce_scope(_enter_sharp_chargemicro._w, _box_X_BounceExact);

			if (b_scope < _d.scopes().size())
			{
				LFScope& scope_bounce = _d.scopes()[b_scope];
				size_t b_peak_ind = scope_bounce.peak_ended_ind(_enter_sharp_chargemicro._side);
				LFBox& b_scope_peak = scope_bounce[b_peak_ind];
				if (scope_bounce._just_ended_side == p_side)
				{
					trade_exit(p_side, _box_X_Scope_pct, "X_bscope_" + scope_bounce._prefix + "_ended");
				}
				else if (b_scope_peak._w.value(_d._p, _d._model._tick_size) < -0.9)
				{
					trade_exit(p_side, _box_X_Scope_pct, "X_bscope_" + scope_bounce._prefix + "_busted");

				}
				else if (u_ticks >= _box_X_MicroConf_ticks)
				{
					size_t h_accel = _d.scopes().size();
					for (size_t i = b_scope + 1; i > 0; --i)
					{
						LFScope& sc = _d.scopes()[i - 1];
						size_t scb = sc.bounce_ind(_enter_sharp_chargemicro._side);
						if (sc[scb]._w._pattern.test(LF::pb_ACCELERATED))
							h_accel = i - 1;

					}
					if (h_accel < _d.scopes().size())
					{
						//					h_accel = h_accel > 0 ? h_accel - 1 : 0;
						LFScope& scope_accel = _d.scopes()[h_accel];
						size_t scb = scope_accel.bounce_ind(_enter_sharp_chargemicro._side);
						scope_accel.model_send(scb, scope_accel._prefix, _d._t, _d._model._out_file_boxes, "_accel");

						if (scope_accel._just_ended_side == p_side)
						{
							std::ostringstream os;
							os << "X_accel_" << scope_accel._prefix << "_ended";
							double x_pct = _box_X_Accel_pct;
							if (x_pct < 0 && _box_X_MicroConf_bpct > 0)
							{
								x_pct = b_pct*100. / _box_X_MicroConf_bpct;
								if (x_pct > 100)
									x_pct = 100;
								os << std::setprecision(1) << std::fixed
									<< "_" << x_pct << "%";
							}
							trade_exit(p_side, x_pct, os.str());
						}

					}
				}
			}
		}
	}
	else
	{
		//not confirmed
		if (scope_0._just_ended_side == p_side
			&& _entry._side == p_side)
		{
			double indx = LF::by_val(_crl_box_charge_sets, _enter_sharp_charge._w._cluster_v, _crl_box_charge_sets_inter);
			double u_ticks = _entry._unrealized_ticks;
			double x_ticks = LF::by_ind(_box_X_Micro_ticks, indx, false);
			if (u_ticks > x_ticks)
			{
				_entry.exit_pct(_box_X_Micro_pct, _d._md);
				if (_entry._exit.good())
				{
					LFMatchPtr m;
					m.reset(new LFMatch("Micro_ticks", "Micro_ticks_" + get_name(), false));
					m->reset(_entry._side->contra_s(), _d.dp());
					m->_box = bc_bounce;
					m->_m_v = b_v;
					m->_m_l = b_l;
					m->_m_b_pct = b_pct;
					m->_m_ratio = b_t_pct;
					m->_peak_v = _enter_sharp_charge._w._cluster_v;
					m->_qty = _entry._exit._q;
					LF::model_send(*m, "", _d._t, _ematches_out_file, "x_" + m->_m_name);
					_history.push_back(m);
					std::ostringstream os;
					os << std::setprecision(0) << std::fixed
						<< "X_ticks_" << u_ticks << "(" << x_ticks << ")";

					trade(_entry._side->contra_s(), -_entry._exit._q, os.str());
					LF::model_send(_entry, "X_Micro_ticks", _d._t, _ematches_out_file);
				}
			}
		}


	}
}

//======================================================================
void LFSignal_HighLow::report_sharp_trigger(bool active_)
{
	if (active_ && _d._sharp_box.valid())
	{
		std::ostringstream os;
		os << "\nSHARP \n" << std::setprecision(0) << std::fixed << _d._sharp_box._w._cluster_v
			<< "/" << std::setprecision(0) << _d._sharp_box._w._cluster_s;

		report_trigger(_d._sharp_box, "sharp", os.str(), LF::satSHARP);
	}
	else
	{
		report_trigger(_d._sharp_box, "sharp", "", LF::satUNKNOWN);
	}

}
//======================================================================
void LFSignal_HighLow::process_sharp()
{
	if (_box_E_on_sharp)
	{
		enter_sharp();
		enter_sharp_micro();
		exit_sharp_bounce();
		report_sharp_trigger(true);
	}
	else
	{
		report_sharp_trigger(false);
	}

	if (_enter_sharp_charge.equal(_sharp_charge) && _sharp_charge._max_dp->_t <= _enter_sharp_charge._max_dp->_t)
	{
		LFBox tmp = _enter_sharp_charge;
		tmp._end_t = _sharp_charge._end_t;
		std::string bc_comment = _enter_sharp_charge._comment;
		if (_enter_sharp_chargemicro.equal(_enter_sharp_charge))
			bc_comment += "_" + _enter_sharp_chargemicro._comment;
		report_action(tmp, bc_comment);
	}
	else
	{
		std::string bc_comment;
		if (_enter_sharp_chargemicro.equal(_sharp_charge) && _sharp_charge._max_dp->_t <= _enter_sharp_chargemicro._max_dp->_t)
			bc_comment += "_" + _enter_sharp_chargemicro._comment;
		report_action(_sharp_charge, bc_comment);
	}
}
//======================================================================
void LFSignal_HighLow::process_250()
{
	if (!_box_E_on_250)
		return;

	if (_d._last_close != 0)
	{
		LFMatchPtr m_250;

		double ticks_250 = Z::get_setting_double(_resource_base + "250_move_ticks", 2.50);
		double up = _d._last_close + ticks_250;
		double down = _d._last_close - ticks_250;

		m_250.reset(new LFMatch("move_250", "move_250_" + get_name(), true));

		if (_d._p > up)
			m_250->reset(LFSide::s_by_sign(_d._p - up), _d.dp());
		else if (_d._p < down)
			m_250->reset(LFSide::s_by_sign(_d._p - down), _d.dp());
		else
			return;

		_d._last_close = 0;

		m_250->_m_loss_v = Z::get_setting_double(_resource_base + "250_loss_ticks", 25);
		m_250->_m_profit_v = Z::get_setting_double(_resource_base + "250_success_ticks", 50);
		
		_bias_enter_matches.push_back(m_250);
	}
}
//======================================================================
void LFSignal_HighLow::process_box()
{
	process_sharp();

	process_250();

}

//======================================================================
double LFSignal_HighLow::repeater_enter_qty(double requested_q_, double existed_q_)
{
	if (requested_q_ < existed_q_)
		return 0;
	//if (existed_q_ > 0)
	double q =  requested_q_ - existed_q_;
	
	double adj_limit = (long)(_strat.position_limit() * _repeater_adjust_limit_factor + 0.5);
	if (requested_q_ >= adj_limit)
		return q;

	return (long)(q * _repeater_adjust_factor + 0.5);
}

//======================================================================
void LFSignal_HighLow::process_repeater()
{
	if (!_repeater)
		return;

	if (_strat._m_signal.get() == 0)
	{
		return;
	}

	LFInfo_Position prev_data = _m_data;
	_m_data = *_strat._m_signal;
	_strat._m_signal.reset();

	LFPositionData& pd = _strat.pos_data();
	const LFSide* m_side = LFSide::s(_m_data._side);
	const LFSide* p_side = LFSide::s(pd._side);

	if (m_side == 0)
	{
		// liquidate
		if (p_side != 0 && pd._p_remain_q > 0)
		{
			_entry.reset(p_side, _d._t, pd, 0, _d._md);
			_entry.set_exit(pd._p_remain_q, _d._md);
			trade(p_side->contra_s(), -pd._p_remain_q, "master_liq");
		}
		return;
	}

	if (m_side != p_side)
	{
		// different side
		if (_m_data._open._q > 0)
		{
			_entry.reset(m_side, _d._t, pd, 0, _d._md);
			double q = repeater_enter_qty(_m_data._open._q, 0);
			_entry.enter(q, _strat.position_limit(), _d._md, 0, 0);
			trade(m_side->contra_s(), q, "master_new");
		}
		else if (p_side != 0 && pd._p_remain_q > 0)
		{
			_entry.reset(p_side, _d._t, pd, 0, _d._md);
			_entry.set_exit(pd._p_remain_q, _d._md);
			trade(p_side->contra_s(), -pd._p_remain_q, "master_liq");
		}

		return;
	}

	// same side
	if (_m_data._open._q > pd._p_open._q)
	{
		// add qty
		_entry.reset(m_side, _d._t, pd, 0, _d._md);
		double q = repeater_enter_qty(_m_data._open._q, pd._p_open._q);
		_entry.enter(q, _strat.position_limit(), _d._md, 0, 0);
		trade(m_side->contra_s(), q, "master_add");
	}
	else if (pd._p_remain_q > 0 && _m_data._open._q < pd._p_remain_q)
	{
		// remove qty
		_entry.reset(p_side, _d._t, pd, 0, _d._md);
		double q = pd._p_remain_q - _m_data._open._q;
		_entry.set_exit(q, _d._md);
		trade(m_side->contra_s(), -q, "master_remove");
	}

}
//======================================================================
void LFSignal_HighLow::trading()
{
	LFPositionData& pd = _strat.pos_data();
	if (!pd._opening.good() && pd._p_remain_q == 0 && _button_entry)
	{
		_button_entry = false;
		_sharp_charge.set_ended();
		_enter_sharp_charge.set_ended();
	}

	process_repeater();
	if (_d._box.valid() && !_check_box.valid())
		_check_box = _d._box;

	if (_d._box.valid() && _d._box._c_dp->_t != _check_box._c_dp->_t)
	{
		process_box();
		_check_box = _d._box;
	}

}
//======================================================================
//======================================================================
//======================================================================
LFSignal_Repeater::LFSignal_Repeater(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_)
	:LFSignal_Market(strat_, d_, type_)
{
	if (_on)
	{
		//
	}


}
//======================================================================
LFSignal_Repeater::~LFSignal_Repeater()
{
}

//======================================================================
void LFSignal_Repeater::trading()
{
	if (_strat._m_signal.get() == 0)
	{
		//if (_m_data.is_valid() && _d._md.is_valid() && _d._md._t < _m_data.get_t())
		//{
		//	Z::report_error("%s -- old_tick t=%s, info_t=%s",
		//		get_name().c_str(), TCTimestamp(_d._md._t).c_str(), TCTimestamp(_m_data.get_t()).c_str());
		//}
		return;
	}

	LFInfo_Position prev_data = _m_data;
	_m_data = *_strat._m_signal;
	_strat._m_signal.reset();
	//if (prev_data.is_valid() && _d._md.is_valid() && _m_data.get_t() < _d._md._t)
	//{
	//	Z::report_error("%s -- old_info t=%s, info_t=%s",
	//		get_name().c_str(), TCTimestamp(_d._md._t).c_str(), TCTimestamp(_m_data.get_t()).c_str());
	//}


	LFPositionData& pd = _strat.pos_data();
	const LFSide* m_side = LFSide::s(_m_data._side);
	const LFSide* p_side = LFSide::s(pd._side);

	if (m_side == 0)
	{
		// liquidate
		if (p_side != 0 && pd._p_remain_q > 0)
		{
			trade(p_side->contra_s(), -pd._p_remain_q, "master_liq");
		}
		return;
	}

	if (m_side != p_side)
	{
		// different side
		if (_m_data._open._q > 0)
		{
			trade(m_side->contra_s(), _m_data._open._q, "master_new");
		}
		else if (p_side != 0 && pd._p_remain_q > 0)
		{
			trade(p_side->contra_s(), -pd._p_remain_q, "master_liq");
		}

		return;
	}

	// same side
	if (_m_data._open._q > pd._p_open._q)
	{
		// add qty
		trade(m_side->contra_s(), _m_data._open._q - pd._p_open._q, "master_add");
	}
	else if (pd._p_remain_q > 0 && _m_data._open._q < pd._p_remain_q)
	{
		// remove qty
		trade(m_side->contra_s(), _m_data._open._q - pd._p_remain_q, "master_remove");
	}

}
//======================================================================
