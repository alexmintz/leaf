/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFSignal.h"
#include "leaf/LFStrategy.h"

#include <sstream>
#include <cmath>
#include <iomanip>

//======================================================================
LFSignal::LFSignal(LFStrategy& strat_, const std::string& type_, const std::string& market_)
	: _strat(strat_),
	_type(type_),
	_market(market_),
	_on(false),
	_trading_on(false),
	_position_owner(false),
	_start_pnl(0),
	_pre_pnl(0),
	_teaser(LFTeaser(), LFTeaser()),
	_active_peak_id(0,0),
	_action(LF::paUndefined)
{
	_resource_base = "LFModel:" + _type + ":";
	_name = _type;
	if(_market.length() > 0)
	{
		_resource_base = _resource_base + _market + ":";
		_name = _name + "_" + _market;
	}

	_on =  Z::get_setting_bool(_resource_base + "on", false);
	_trading_on =  Z::get_setting_bool(_resource_base + "trading", false);
	for(size_t i = 0; i < _teaser.size(); ++i)
	{
		_teaser[i].set_key(_name);
	}
}
//======================================================================
LFSignal::~LFSignal()
{
}

//======================================================================
void LFSignal::close()
{
	for(size_t i = 0; i < _teaser.size(); ++i)
	{
		LFTeaser& tsr = _teaser[i];
		if(tsr.valid())
		{
			tsr.reset();
		}
	}
}
//======================================================================
void LFSignal::set_position_owner(double pnl_)
{
	if (_position_owner)
		return;
	_position_owner = true;
	_start_pnl = pnl_;
	Z::report_info("%s - %s::set_position_owner(%f) -- pre_pnl = %f ", TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str(), pnl_, _pre_pnl);
}
//======================================================================
void LFSignal::reset_position_owner(double pnl_)
{
	if (!_position_owner)
		return;
	_pre_pnl += (pnl_ - _start_pnl);
	reconcile_pos();
	_position_owner = false;
	Z::report_info("%s - %s::reset_position_owner(%f) -- pre_pnl = %f ", TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str(), pnl_, _pre_pnl);
}
//======================================================================
//======================================================================
LFSignal_Market::LFSignal_Market(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_)
	: LFSignal(strat_, type_, d_._model._market),
	_d(d_),
	_cut_loss_unrealized(0),
	_cut_loss_suspend(false),
	_cut_loss_total(0)
{
	if(_on)
	{
		_box_X_Profit_pct = Z::get_setting_double(_resource_base + "box_X_Profit_pct", 0);
		_box_X_cut_loss_ticks = Z::get_setting_double(_resource_base + "box_X_cut_loss_ticks", 0);
		_box_report_profit = Z::get_setting_double(_resource_base + "box_report_profit", 25);
		_ematches_out_file = Z::get_setting_bool(_resource_base + "ematches_out_file", false);
		_xmatch_out_file = Z::get_setting_bool(_resource_base + "xmatch_out_file", false);

		_trace_r.init(Z::get_setting(_resource_base + "trace_range", "20111111_111111:20111111_111111"));
		if(_trace_r.is_valid())
			Z::report_info("Signal Trace On -- %s -- [%s]", _name.c_str(), _trace_r.as_string().c_str());

	}
	_entry.set_key(std::string("entry_") + get_name());
	_entry._cutloss_ticks = _box_X_cut_loss_ticks;
}
LFSignal_Market::~LFSignal_Market()
{
}
//======================================================================
void LFSignal_Market::close()
{
	LFSignal::close();
	close_bias_matches(0);
}

void LFSignal_Market::update()
{
	cut_loss();

	check_cut_loss();
	reconcile_pos();
	update_matches();
	exit_profit_loss();
	trading();
	// reporting
	_history.insert(_history.end(), _d._p_store->_to_report.begin(), _d._p_store->_to_report.end());
	_d._p_store->_to_report.clear();
}

bool LFSignal_Market::check_cut_loss()
{
	if (!_trading_on)
		return false;

	if (!_position_owner)
		return false;

	LFPositionData& pd = _strat.pos_data();
	if (pd._side == LF::s_UNKNOWN)
		return false;
	// In position
	const LFSide* pside = LFSide::s(pd._side);

	// cut loss total
	if (pd._p_remain_q > 0 && _cut_loss_total > 0)
	{
		// cut loss
		double pnl = pd._pnl - _start_pnl + _pre_pnl;
		if (pnl <= -_cut_loss_total)
		{
			Z::report_info("%s - %s::cut_loss_total %f (limit %f) ",
				TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str(), pnl, _cut_loss_total);
			trade(pside->contra_s(), -pd._p_remain_q, "cut_loss_total");
			_trading_on = false;
			Z::report_info("%s - %s::cut_loss_total -- signal suspended",
				TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str());
			return true;
		}
	}
	// cut loss (on trade)
	if (pd._p_remain_q > 0 && _cut_loss_unrealized > 0)
	{
		double cut_ticks = -_cut_loss_unrealized / pd._tick_value / pd._p_remain_q;
		if (pd._unrealized_ticks <= cut_ticks)
		{
			Z::report_info("%s - %s::cut_loss_unrealized ticks %f (limit %f ticks) ",
				TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str(), pd._unrealized_ticks, cut_ticks);
			trade(pside->contra_s(), -pd._p_remain_q, "cut_loss_unrealized");
			if (_cut_loss_suspend)
			{
				_trading_on = false;
				Z::report_info("%s - %s::cut_loss_unrealized -- signal suspended",
					TCTimestamp(_strat.get_input_time()).c_str(), get_name().c_str());
			}
			return true;
		}
	}
	return false;

}

//======================================================================
const LFSide* LFSignal_Market::cut_loss()
{
	if(!_trading_on)
		return 0;

	const LFSide* cl_side = _cut_loss.cross_reset(_d._p);

	if (cl_side == 0)
		return 0;

	_action = _cut_loss._action;
	const LFSide* p_side = cl_side->contra_s();
	trade_exit(p_side, 100, "cut_loss");
	return cl_side;
}
//=================================================================================
bool lf_compare_match(const LFMatchPtr& s1_, const LFMatchPtr& s2_)
{
	return s1_->_t < s2_->_t;
}
//======================================================================
void LFSignal_Market::update_matches()
{
	LFMatchPtrList::iterator i = _bias_enter_matches.begin();
	bool history_added = false;
	while (i != _bias_enter_matches.end())
	{
		if (update_match(*(*i)))
		{
			close_bias_match(*(*i));
			_history.push_back(*i);
			history_added = true;
			i = _bias_enter_matches.erase(i);
		}
		else
			++i;
	}
	if (history_added)
		_history.sort(lf_compare_match);
}
//======================================================================
bool LFSignal_Market::update_match(LFMatch& match_)
{
	if (!match_._is_enter)
		return false;
	double cv = -match_.value(_d._p, _d._model._tick_size);
	double l = LF::length_from_t(_d._t - match_._dp->_t);
	bool m_closed = false;
	if (cv > match_._res_max_v)
	{
		match_._res_max_v = cv;
		match_._res_max_l = l;
	}
	if (cv < match_._res_min_v)
	{
		match_._res_min_v = cv;
		match_._res_min_l = l;
	}

	if (match_._m_profit_l == 0 && match_._m_profit_v > 0 && cv > match_._m_profit_v)
	{
		match_._m_profit_l = l;
	}
	if (match_._m_loss_l == 0 && match_._m_loss_v > 0 && cv < 0 && -cv > match_._m_loss_v)
	{
		match_._m_loss_l = l;
		m_closed = true;
	}

	match_._m_success = match_._m_profit_l > 0;

	return m_closed;

}
//======================================================================
bool LFSignal_Market::trade_exit(const LFSide* pos_side_, double pos_pct_, const std::string& comment_)
{
	if (!_trading_on)
		return false;

	LFPositionData& pd = _strat.pos_data();

	if (pos_side_->value() != pd._side)
		return false;

	bool total = false;
	if (pd._p_remain_q > 0 && pos_pct_ > 0)
	{
		double q = (long)(pd._p_remain_q*pos_pct_ / 100. + 0.5);
		if (q == 0)
			q = 1;
		if (q > pd._p_remain_q)
		{
			q = pd._p_remain_q;
		}
		total = q >= pd._p_remain_q;
		trade(pos_side_->contra_s(), -q, comment_);
		if (_entry.valid() && _entry._side == pos_side_)
		{
			_entry.set_exit(q, _d._md);
			LF::model_send(_entry, "trade_exit", _d._t, _ematches_out_file);
			_action = LF::paTrendEnd;
		}

	}
	return total;
}
//======================================================================
void LFSignal_Market::close_bias_matches(const LFSide* side_)
{
	LFMatchPtrList::iterator i = _bias_enter_matches.begin();
	while (i != _bias_enter_matches.end())
	{
		if (side_ == 0 || LF::same_side((*i)->_side, side_))
		{
			close_bias_match(*(*i));
			_history.push_back(*i);
			i = _bias_enter_matches.erase(i);
		}
		else
			++i;
	}

}
//======================================================================
void LFSignal_Market::close_bias_match(LFMatch& match_)
{
	double cv = -match_.value(_d._p, _d._model._tick_size);
	double l = LF::length_from_t(_d._t - match_._dp->_t);

	match_._res_end_v = cv;
	match_._res_end_l = l;
}
//======================================================================
void LFSignal_Market::exit_profit_loss()
{
	_entry.exit_cutloss(_d._md);
	if (_entry._exit.good())
	{
		trade(_entry._side->contra_s(), -_entry._exit._q, "X_CutLoss");
		LF::model_send(_entry, "X_CutLoss", _d._t, _ematches_out_file);
	}
	_entry.exit_takeprofit(_d._md);
	if (_entry._exit.good())
	{
		trade(_entry._side->contra_s(), -_entry._exit._q, "X_TakeProfit");
		LF::model_send(_entry, "X_TakeProfit", _d._t, _ematches_out_file);
	}
}
//======================================================================
void LFSignal_Market::reconcile_pos()
{
	if (!_entry.valid())
		return;
	if (!_position_owner)
		return;
	_entry.mark_to_market(_d._md);

	std::string comment;
	LFPositionData& pd = _strat.pos_data();
	if (pd._opening.good() || pd._closing.good())
	{
		double pd_q = pd._p_open._q - pd._closing._q;
		if (pd._side == _entry._side->value() && _entry._open._q != pd_q)
		{
			_entry._open.reset(pd._p_open._p, pd_q);
			_entry._max_q = std::max(_entry._max_q, _entry._open._q);
			comment += "_iq";
		}
	}
	else
	{
		if (pd._side != _entry._side->value())
		{
			if (_entry._open.good())
			{
				comment += "_s";
				if (_entry._side_confirmed)
					_entry._realized_pnl = pd._realized_pnl - _entry._pre_realized_pnl;
				else
				{
					_entry._pre_realized_pnl = pd._realized_pnl - _entry._realized_pnl;
					_entry._side_confirmed = true;
				}
				_entry._open.reset();
			}
		}
		else
		{
			_entry._side_confirmed = true;

			if (pd._open._q != _entry._open._q)
				comment += "_q";
			if (pd._open._p != _entry._open._p)
				comment += "_p";
			if (pd._realized_pnl != _entry._pre_realized_pnl + _entry._realized_pnl)
				comment += "_r";

			_entry._open = pd._open;
			_entry._realized_pnl = pd._realized_pnl - _entry._pre_realized_pnl;

		}
		if (!comment.empty())
		{
			_entry._max_q = std::max(_entry._max_q, _entry._open._q);
			_entry.mark_to_market(_d._md);
			LF::model_send(_entry, "R" + comment, _d._t, _ematches_out_file);
		}
	}
}
//======================================================================
void LFSignal_Market::trade(const LFSide* t_side_, double q_, const std::string& comment_)
{
	if (!_trading_on)
		return;
	LFTeaser& tsr = _teaser[t_side_->value()];
	double q = q_;
	std::string comment = _type + "__" + comment_;
	if (tsr.valid() && !tsr._processed)
	{
		q += tsr._q;
		comment = tsr._comment + ", " + comment;
	}
	tsr.reset(t_side_, _d._md._t, t_side_->same_p(_d._md), q);
	tsr._comment = comment;
	LF::model_send(tsr, "", _strat.get_input_time(), true);
}
//======================================================================
void LFSignal_Market::report_trigger(const LFBox& b_, const std::string& comment_, const std::string& tr_comment_, LF::StrategyActionType action_)
{
	_bias_bx = b_;
	if (!b_.valid())
		return;
	if (!comment_.empty())
		_bias_bx._comment = comment_;
	if (!tr_comment_.empty())
		_bias_bx._w._comment = tr_comment_;
	_bias_bx._w._strength = (long)action_;
}
//======================================================================
void LFSignal_Market::report_action(const LFBox& b_, const std::string& comment_)
{
	_bias_bc = b_;
	if (!b_.valid())
		return;
	if (!comment_.empty())
		_bias_bc._comment = comment_;
}
//======================================================================
void LFSignal_Market::update_momentum_data()
{
	_d._bias_bc = _bias_bc;
	_d._bias_bx = _bias_bx;
}


//======================================================================
