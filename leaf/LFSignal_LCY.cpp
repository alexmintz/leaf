/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFSignal_LCY.h"
#include "leaf/LFStrategy.h"

#include <sstream>
#include <cmath>
#include <iomanip>

//======================================================================
LFSignal_LCY::LFSignal_LCY(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_)
	:LFSignal_Market(strat_, d_, type_),
	_request_qty(0)

{
	if (_on)
	{
		_E_on_lcy = Z::get_setting_integer(_resource_base + "E_on_lcy", 0);
		_E_on_lcy_min_v = Z::get_setting_double(_resource_base + "E_on_lcy_min_v", 0);
		_E_on_lcy_min_vol = Z::get_setting_double(_resource_base + "E_on_lcy_min_vol", 0);
		_E_on_lcy_valid_bounce_cnty = Z::get_setting_double(_resource_base + "E_on_lcy_valid_bounce_cnty", 0);
		_X_on_lcy_valid_peak_cnty = Z::get_setting_double(_resource_base + "X_on_lcy_valid_peak_cnty", 0);

	}
}
LFSignal_LCY::~LFSignal_LCY()
{
}

//======================================================================
void LFSignal_LCY::process_request(const LFStrategyRequest& r_)
{
	if (r_._st_action != LF::satLCY)
		return;
	if (r_._st_active)
	{
		Z::report_info("%s -- Activate_LCY - Activate (side %d)", get_name().c_str(), r_._order_side);
		if (_d._lcy.empty())
		{
			Z::report_error("%s -- Activate_LCY - no valid LCY", get_name().c_str());
			return;
		}
		const LFBox& lcy = _d._lcy[0];
		if (lcy._side->value() != r_._order_side)
		{
			Z::report_error("%s -- Activate_LCY - wrong side", get_name().c_str());
			return;
		}
		if (_lcy_enter.valid())
			_lcy_enter.reset();

		_lcy_bc = lcy;

		_request_qty = r_._q;
		LF::model_send(_lcy_bc, "R_SET", _d._t, _d._model._out_file_boxes, _d._box_key + "_lcy_bc");
	}
}
//======================================================================
void LFSignal_LCY::report_lcy_trigger(bool active_)
{
	if (active_ && !_d._lcy.empty())
	{
		const LFBox& lcy = _d._lcy[0];
		std::ostringstream os;
		os << "\nLCY " << lcy.id()._scale << "\n" 
			<< std::setprecision(0) << std::fixed << lcy._w._cluster_v
			<< "/" << std::setprecision(0) << lcy._w._cluster_vol;

		report_trigger(lcy, "lcy", os.str(), LF::satLCY);
	}
	else
	{
		report_trigger(LFBox(), "lcy", "", LF::satUNKNOWN);
	}

}
//======================================================================
void LFSignal_LCY::enter_lcy()
{
	if (!_lcy_bc.valid())
		return;

	long trigger_scale = -1;
	for (long i = _lcy_bc.id()._scale - 1; _d.scopes().valid_scale(i) && trigger_scale < 0; --i)
	{
		if (_d.scopes().scope(i)._just_ended_side == _lcy_bc._side->contra_s())
		{
			trigger_scale = i;
		}
	}
	if (trigger_scale < 0)
		return;

	// look for a bounce
	LF::Id bnc_id = _d.scopes().bounce_id(_lcy_bc.id());
	if (!bnc_id.valid())
	{
		// look for last blossom
		LF::IdVec ids;
		_d.scopes().fill_blossom_ids(_lcy_bc.id(), _lcy_bc.id()._scale, ids);
		if (!ids.empty())
		{
			// last blossom
			bnc_id = *ids.begin();
			// bounce is next
			bnc_id._roll++;			
		}
	}
	if (!bnc_id.valid())
		return;

	const LFBox& bnc = _d.scopes().roll(bnc_id);
	double cnty = _lcy_bc._w.continuity(bnc._w, _d._model);

	if (_lcy_enter.valid())
	{
		if (cnty < _lcy_enter._max_corr)
		{
			// keep bounce with least continuity after enter
			if (!_lcy_bnc.valid() || cnty < _lcy_bnc._max_corr)
			{
				_lcy_bnc = bnc;
				_lcy_bnc._max_corr = cnty;
			}
		}
	}
	else
	{
		LFMatchPtr m;
		m.reset(new LFMatch("lcy", "lcy_" + get_name(), true));
		m->reset(_lcy_bc._side, _d.dp());
		m->_box = _lcy_bc;
		m->_m_scale = bnc_id._scale;
		m->_m_v = bnc._w._cluster_v;
		m->_m_vol = bnc._w._cluster_vol;
		m->_m_den = cnty;
		m->_m_loss_v = _box_report_profit;
		m->_m_profit_v = _box_report_profit;
		m->_peak_vol = _lcy_bc._w._cluster_vol;
		m->_peak_v = _lcy_bc._w._cluster_v;
		m->_mp_mask.set(LF::mp_DENSITY, m->_m_den > _E_on_lcy_valid_bounce_cnty);
		LF::model_send(*m, "", _d._t, _ematches_out_file, "e_" + m->_m_name);

		if (m->_m_mask.none())
		{
			// keep enter bounce with continuity value
			_lcy_enter = bnc;
			_lcy_enter._max_corr = cnty;
			_lcy_bnc.reset();
			LFPositionData& pd = _strat.pos_data();
			_entry.reset(m->_side, _d._t, pd, 0, _d._md);
			_entry.enter(_request_qty, _strat.position_limit(), _d._md, 0, 0);

			LF::model_send(_entry, "lcy", _d._t, _ematches_out_file);
			if (_entry._enter.good())
			{
				m->_qty += _entry._enter._q;
				trade(m->_side->contra_s(), _entry._enter._q, "E_" + m->_m_name);
			}

			_bias_enter_matches.push_back(m);

		}
	}
}
//======================================================================
void LFSignal_LCY::exit_lcy()
{
	if (_lcy_enter.valid())
	{
		if (_d._lcy.empty()
			|| _d._lcy[0]._side == _lcy_enter._side)
		{
			trade_exit(_lcy_enter._side->contra_s(), 100, "X_lcy");
			_lcy_enter.reset();
			_lcy_bnc.reset();
			_lcy_bc.reset();
		}
		else
		{
			LF::Id bnc_id = _d.scopes().bounce_id(_lcy_bc.id());
			if (!bnc_id.valid())
				return;
			if (_d.scopes().scope(bnc_id._scale)._just_ended_side != _lcy_bc._side)
				return;
			LFBox* bnc_ptr = &_lcy_enter;
			if(_lcy_bnc.valid())
				bnc_ptr = &_lcy_bnc;
			double cnty = _lcy_bc._w.continuity(bnc_ptr->_w, _d._model);
			if (cnty > bnc_ptr->_max_corr + _X_on_lcy_valid_peak_cnty)
			{
				trade_exit(_lcy_enter._side->contra_s(), 100, "X_lcy_peak");
				_lcy_enter.reset();
			}
		}
	}
}
//======================================================================
void LFSignal_LCY::process_lcy()
{
	if (_E_on_lcy <= 0)
		return;

	if (!_d._lcy.empty())
	{
		const LFBox& lcy = _d._lcy[0];
		if (lcy.id()._scale >= _E_on_lcy
			&& (_E_on_lcy_min_v <= 0 || lcy._w._cluster_v >= _E_on_lcy_min_v)
			&& (_E_on_lcy_min_vol <= 0 || lcy._w._cluster_vol >= _E_on_lcy_min_vol))
		{
			report_lcy_trigger(true);
			if (_lcy_bc.valid())
				_lcy_bc = lcy;
		}
		else
		{
			report_lcy_trigger(false);
		}
	}
	exit_lcy();
	enter_lcy();
	report_action(_lcy_bc, "");

}
//======================================================================
void LFSignal_LCY::trading()
{
	process_lcy();
}
//======================================================================
