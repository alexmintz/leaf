/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMomentumData.h"
#include <cstdlib> 
#include <set>
#include <cfloat>
#include <iomanip>
#include "gsl/gsl_fit.h"
#include "gsl_stats_spearman.h"

//======================================================================
//======================================================================
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFMomentumData_P& d_)
{
	return true;
}

//======================================================================
//======================================================================
bool operator>>(ACE_InputCDR& strm, LFMomentumData_P& d_)
{
	return true;
}

//======================================================================
std::ostream& operator<<(std::ostream& os, const LFMomentumData_P& d_)
{

	os << TCTimestamp(d_._t).c_str() ;
	os << "," << d_._p ;
	os << "," << d_._last_close;
	
	return os;
}
//======================================================================
void LFMomentumData_P::from_csv_vec(int& ind_, const std::vector<const char*>& v_)
{
}
//======================================================================
//======================================================================
LFMomentumData_P::LFMomentumData_P(LFMarketModel* model_)
	:
	_p(0),
	_obv(0),
	_vma(0),
	_mfi(0),
	_u_vol(0),
	_d_vol(0),
	_u_tran(0),
	_d_tran(0),
	_t(ACE_Time_Value::zero),
	_last_close(0),
	_p_activity(0),
	_v_activity(0),
	_corr(0),
	_corr_dyn_high(0),
	_corr_dyn_low(0),
	_trend_side(0),
	_in_trend(false),
	_trend_hard_p(0),
	_trend_soft_p(0)

{
}
//======================================================================
LFMomentumData_P::~LFMomentumData_P()
{
}

//======================================================================
void LFMomentumData_P::reset()
{
	_md = LFDepthData();
	_p = 0;
	_obv = 0;
	_vma = 0;
	_mfi = 0;
	_u_vol = 0;
	_d_vol = 0;
	_u_tran = 0;
	_d_tran = 0;
	_t = ACE_Time_Value::zero;
	_last_close = 0;
	_p_activity = 0;
	_v_activity = 0;
	_corr = 0;
	_corr_dyn_high = 0;
	_corr_dyn_low = 0;
	_box.reset();
	_box_trends.reset();
	_lcy_trends.reset();
	_lcy.clear();
	_latter_box.reset();
	_former_box.reset();
	_charge_trend.reset();
	_sharp_box.reset();
	_trend_side = 0;
	_in_trend = false;
	_trend_hard_p = 0;
	_trend_soft_p = 0;
	_bias_bc.reset();
	_bias_bx.reset();
}
//======================================================================
//======================================================================
LFMomentumData::LFMomentumData(const std::string& market_, const std::string& momentum_)
:	LFMomentumData_P(&_model),
	_model(market_, momentum_),
	_p_store(new LFPointStore(&_model)),
	_tick_limit_from_trade(false),
	_last_trade_side(0),
	_i_count(0)

{
	_s_store.reset(new LFScopeStore(_p_store.get()));

	_levels_key = _model._name + "_levels";
	_box_key = _model._name + "_box";
	_datapoint_key = _model._name + "_dpoint";
	_box.set_key(_box_key);

	reset();
}
//======================================================================
LFMomentumData::~LFMomentumData() 
{
}

//======================================================================
void LFMomentumData::load_and_reset()
{
	_model.reset();
	_model.load();
	reset();
}
//======================================================================
void LFMomentumData::reset()
{
	LFMomentumData_P::reset();
	_p_store->reset();
	_s_store->reset();
	_tick_limit_tv = ACE_Time_Value::zero;
	_tick_limit_from_trade = false;
	_last_trade_side = 0;
	_last_trade.reset();
	_i_count = 0;
}
//======================================================================
void LFMomentumData::clear_just_flags()
{
	scopes().clear_just_flags();

}
//======================================================================
bool LFMomentumData::process_missed_tick(const ACE_Time_Value& t_)
{
	ACE_Time_Value t = _tick_limit_tv;
	if (_tick_limit_tv == ACE_Time_Value::zero)
		return false;
	
	LFPoint dp;
	dp.reset(0, LFDataPointPtr(new LFDataPoint(_t)));
	dp.reset_identity(_i_count);

	///AK TODO 
	//if (BMModel::instance()->is_live()
	//	&& t_ > _t + _model._tick_timeout_max)
	//{
	//	_tick_limit_tv = t_ + _model._tick_timeout;
	//	_last_trade.reset();
	//	Z::report_error("-- TICK_TIMEOUT -- %s_%s -- %s > %s",
	//		_model._market.c_str(), _model._momentum.c_str(), TCTimestamp(t_).c_str(), TCTimestamp(_t).c_str());
	//	if (_model._out_file_points_trace)
	//	{
	//		LF::model_send(dp, "TICK_TIMEOUT", t_, _model._out_file_points_trace, _datapoint_key + "_in");
	//	}
	//	return false;
	//}

	std::string comment = "MISSED";
	if (_last_trade.valid())
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "MISSED_T", t_, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return process_last_trade(t_);
	}

	if (_model._out_file_points_trace)
	{
		LF::model_send(dp, "MISSED_P", t_, _model._out_file_points_trace, _datapoint_key + "_in");
	}

	update(t, _p, false);
	return true;

}

//======================================================================
bool LFMomentumData::process_depth(const LFDepthData& md_)
{
	LFPoint dp;
	dp.reset(0, LFDataPointPtr(new LFDataPoint(md_._t)));
	dp.reset_identity(_i_count);

	if (!md_._ask[0].good() || !md_._bid[0].good())
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "0_bbo", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}
	if (!(md_._bid[0]._p < md_._ask[0]._p))
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "cross_bbo", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}

	if (_t != ACE_Time_Value::zero && md_._t < _t)
	{
		_p_store->report("old_depth", _model._name, _model._report_old_depth);
		_model._report_old_depth = false;
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "old_depth", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}

	double p = 0;
	//if (_last_trade_side == 0)
	//{
		p = (md_._ask[0].p_p() + md_._bid[0].p_p()) / 2.;
		double ticks = 0;
		LFSide::mod_tick(p, md_._tick_size / 10., &ticks);
		p = ticks * md_._tick_size / 10.;
	//}
	//else
	//{
	//	p = _last_trade_side->contra_p(md_);
	//}
	dp._dp->_p = p;
	if (_t != ACE_Time_Value::zero
		&& md_._t < _tick_limit_tv
		&& TCComparePrice::diff(md_._bid[0]._p, _md._bid[0]._p) == 0
		&& TCComparePrice::diff(md_._ask[0]._p, _md._ask[0]._p) == 0)
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "same_bbo", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}

	_md = md_;
	_model.set_md(md_);

	if (!_model._use_depth_price)
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "disabled_depth_p", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return true;
	}

	if (_last_trade.valid())
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "last_trade_valid", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return true;
	}

	if (_tick_limit_from_trade
		&& _t != ACE_Time_Value::zero
		&& md_._t < _tick_limit_tv)
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "last_trade_recent", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return true;
	}

	if (_model._out_file_points_trace)
	{
		LF::model_send(dp, "TICK", md_._t, _model._out_file_points_trace, _datapoint_key + "_in");
	}

	update(md_._t, p, false);
	return true;
}
//======================================================================
bool LFMomentumData::check_process_last_trade(const ACE_Time_Value& t_)
{
	if (!_last_trade.valid() || (_last_trade._t + _model._same_trade_timeout) > t_)
		return false;

	return process_last_trade(t_);
}
//======================================================================
bool LFMomentumData::process_last_trade(const ACE_Time_Value& t_)
{
	if (_model._out_file_points)
	{
		LFPoint dp;
		dp.reset(_last_trade._side, 
			LFDataPointPtr(new LFDataPoint(_last_trade._t, _last_trade._trade.p_p())));
		dp._strength = (long)_last_trade._trade._q;
		dp.reset_identity(_i_count);
		LF::model_send(dp, "", t_, _model._out_file_points, _datapoint_key + "_trade");
	}

	if (_last_trade._side->sign() > 0)
	{
		_u_vol += _last_trade._trade._q;
		_u_tran += _last_trade._tran;
	}
	else
	{
		_d_vol += _last_trade._trade._q;
		_d_tran += _last_trade._tran;
	}
	update(_last_trade._t, _last_trade._trade.p_p(), true);
	_last_trade.reset();
	return true;

}
//======================================================================
bool LFMomentumData::process_trade(const LFTradeData& td_)
{
	LFPoint dp;
	dp.reset(0,
		LFDataPointPtr(new LFDataPoint(td_._t, td_._trade.p_p())));
	dp._strength = (long)td_._trade._q;
	dp.reset_identity(_i_count);


	if (!_md.is_valid())
	{
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "no_md_trade", td_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}

	if (_t != ACE_Time_Value::zero && (td_._t + _model._same_trade_timeout) < _t)
	{
		//Z::report_error("-- OLD TRADE -- %s_%s -- %s < %s",
		//	_model._market.c_str(), _model._momentum.c_str(), TCTimestamp(td_._t).c_str(), TCTimestamp(_t).c_str());
		_p_store->report("old_trade", _model._name, _model._report_old_trades);
		_model._report_old_trades = false;
		if (_model._out_file_points_trace)
		{
			LF::model_send(dp, "old_trade", td_._t, _model._out_file_points_trace, _datapoint_key + "_in");
		}
		return false;
	}
	bool processed = check_process_last_trade(td_._t);

	LFTradePoint tp;
	fill_trade(tp, td_._t, td_._trade);
	dp._side = tp._side;

	std::string comment;

	if (!_last_trade.valid())
	{
		_last_trade = tp;
		set_limit_t(_last_trade._t, true);
		comment = "TR_NEW";
	}
	else
	{
		if (tp._side == _last_trade._side)
		{
			comment = "TR_SAME";
			if (tp._t > _last_trade._t)
				_last_trade._t = tp._t;
			_last_trade._trade._p = tp._trade._p;
			_last_trade._trade._q += tp._trade._q;
			_last_trade._tran += tp._tran;
		}
		else
		{
			comment = "TR_SIDE";
			ACE_Time_Value next_t = _last_trade._t;
			next_t = next_t + ACE_Time_Value(0, 1);

			processed = process_last_trade(tp._t);
			_last_trade = tp;
			_last_trade._t = next_t;
		}
	}
	if (_model._out_file_points_trace)
	{
		LF::model_send(dp, comment, td_._t, _model._out_file_points_trace, _datapoint_key + "_in");
	}

	return processed;
}

//======================================================================
void LFMomentumData::set_limit_t(const ACE_Time_Value& from_t_, bool from_trade_)
{
	ACE_Time_Value t = from_t_ + _model._tick_timeout;

	if (_tick_limit_tv == ACE_Time_Value::zero || _tick_limit_tv < t)
	{
		_tick_limit_tv = t;
		_tick_limit_from_trade = from_trade_;
	}
}
//======================================================================
void LFMomentumData::update(const ACE_Time_Value& t_, double p_, bool trade_)
{
	double last_p = _p;
	_p = p_;
	_t = t_;
	_obv = _u_vol - _d_vol;

	set_limit_t(_t, trade_);

	if (!_model._waves_on)
		return;
	if (TCComparePrice::diff(_p, last_p) != 0)
	{
		if (_p_store->add_point(_t, _p, _u_vol, _d_vol, _u_tran, _d_tran))
			calc_point("s");
	}
	else
	{
		if (_p_store->add_virt_point(_t, _p, _u_vol, _d_vol, _u_tran, _d_tran))
			calc_point("virt");
	}
}
//======================================================================
void LFMomentumData::fill_trade(LFTradePoint& tp_, const ACE_Time_Value& t_, const TC_pqp& pqp_)
{
	bool check_side = false;
	double p = 0;
	if (_last_trade.valid())
	{
		p = _last_trade._trade._p;
		check_side = true;
	}
	else if (_t != ACE_Time_Value::zero)
	{
		p = _p;
		check_side = true;
	}

	if (check_side)
	{
		if (TCComparePrice::diff(pqp_._p, p) > 0)
		{
			_last_trade_side = LFSide::s(LF::s_BUY);
		}
		else if (TCComparePrice::diff(pqp_._p, p) < 0)
		{
			_last_trade_side = LFSide::s(LF::s_SELL);
		}
	}
	if (_last_trade_side == 0)
	{
		_last_trade_side = LFSide::s(LF::s_BUY);
	}
	tp_.reset(t_, _last_trade_side, pqp_, 1);

}

//======================================================================
void LFMomentumData::fill_sharp_roll(LFBox& sb_, LFCluster& etalon_, std::string& comment_,
	tc_range<double>& pslope_, tc_range<double>& min_v_, double pslope_decline_, double max_l_)
{
	sb_.reset();
	etalon_.reset();
	if (!_box.valid())
		return;
	const LFSide* b_side = _box._side;
	if (_model._crl_box_sharp_scope_charge_etalon
		&& min_v_.in(_box._w._cluster_v) && pslope_.in(_box._w._cluster_s))
	{
		etalon_ = _box._w;
	}
	
	for (size_t i = 0; i < scopes().size(); ++i)
	{
		LFScope& scope = scopes()[i];
		size_t pin = scope.peak_ind(b_side);
		if (pin >= scope.size())
			return;
		LFBox& peak = scope[pin];
		if (!peak._w.cluster_valid())
			return;
		if (peak._w._dp->older(*_box._s_dp))
			return;
		if (!etalon_.valid()
			&& min_v_.in(peak._w._cluster_v) && pslope_.in(peak._w._cluster_s))
		{
			etalon_ = peak._w;
		}
		if (etalon_.valid())
		{
			if (etalon_._cluster_s > peak._w._cluster_s*pslope_decline_)
			{
				comment_ = "s_decline";
				return;
			}
			if (peak._w._cluster_s < _model._crl_box_min_slope)
			{
				comment_ = "min_slope";
				return;
			}
			if (!sb_.valid())
			{
				sb_ = peak;
//				sb_._w.reset_identity((long)i, (long)pin, (long)(pin + 1));
			}
			else if (peak._w._cluster_v > sb_._w._cluster_v)
			{
				//AK use LF::slope_i
				LFCluster bot;
				bot.reset_cluster(sb_._w._side, sb_._w._cluster_dp, peak._w._cluster_dp, _model._tick_size);

				if (_model._crl_box_sharp_scope_decline <= 0
					|| sb_._w._cluster_s <= bot._cluster_s*_model._crl_box_sharp_scope_decline)
				{
					sb_ = peak;
//					sb_._w.reset_identity((long)i, (long)pin, (long)(pin + 1));
				}
				else if (_model._crl_box_sharp_scope_decline_stop)
					return;
			}


			if (peak._w._cluster_s > etalon_._cluster_s)
			{
				etalon_ = peak._w;
			}
			if (peak.w_declined(pslope_decline_))
			{
				comment_ = "w_decline";
				return;
			}
			if (_model._crl_box_sharp_scope_exact && i + 1 < scopes().size())
			{
				/*AK TODO revisit
				LFScope& up_scope = scopes()[i+1];
				LFBox& up_peak = up_scope[up_scope.peak_ind(b_side)];
				if (!up_peak._w.cluster_valid() || up_peak._w._dp->older(*_box._s_dp))
					return;

				for (size_t ii = pin + 4; ii < scope.size() && scope[ii]._w.valid() && up_peak._w._cluster_dp->older(*scope[ii]._max_dp); ii +=2)
				{
					double ii_s = LF::slope_i(peak._side, scope[ii]._max_dp, peak._max_dp, _model._tick_size);
					if (peak._w._cluster_s > ii_s*pslope_decline_)
					{
						comment_ = "ww_decline";
						return;
					}
				}
				*/
			}
		}
	}
}
//======================================================================
void LFMomentumData::fill_sharp_box(LFBox& sb_, LFCluster& etalon_, std::string& comment_, 
									tc_range<double>& pslope_, tc_range<double>& min_v_, double pslope_decline_, double max_l_)
{
	LFBoxList& bx = _p_store->_boxes;

	if(bx.empty())
	{
		sb_.reset();
		comment_ = "empty";
		return;
	}
	LFBoxList::iterator start_i = bx.begin();
	LFBox f_box = _box;
	if (f_box.equal(bx.front()))
		++start_i;
	
	sb_.reset();
	etalon_.reset();
	if(pslope_.in(f_box._w._cluster_s)
		&& min_v_.in(f_box._w._cluster_v))
	{
		sb_ = f_box;
		etalon_ = f_box._w;
		LF::model_send(etalon_, "FRONT", _t, _model._out_file_boxes, _box_key + "_etalon");
		LF::model_send(f_box, "front", _t, _model._out_file_boxes, _box_key + "_et_box");

	}

	if( min_v_.in(f_box._w._cluster_v)
		&& f_box.triggered())
	{
		LFCluster tr_etalon;
		tr_etalon.reset(f_box._side, f_box._max_dp);
		LFDataPointPtr dp(new LFDataPoint());
		_p_store->find_point(*dp, f_box._s_dp->_p, tr_etalon._side, LF::length_from_t(f_box._c_dp->_t - f_box._triggered_t));
		if(dp->valid())
		{
			tr_etalon.set_cluster(dp, _model._tick_size);
		}

		if(min_v_.in(tr_etalon._cluster_v))
		{
			if(tr_etalon._cluster_s < pslope_.first)
			{
				comment_ = "flat_etalon";
				return;
			}
			etalon_ = tr_etalon; 
			LF::model_send(etalon_, "TR_ETALON", _t, _model._out_file_boxes, _box_key + "_etalon");
		}
	}

	LFBox tmp = f_box;
	for(LFBoxList::iterator i = start_i; i != bx.end(); ++i)
	{
		LFBox si = *i;
		si.reset_side(f_box._side);
		if(f_box.value(si._max_dp) < 0)
		{
			comment_ = "cross";
			return;
		}
		si.merge(tmp);
		tmp = si;
		if(tmp.length() > max_l_)
		{
			comment_ = "len";
			return;
		}

		if(tmp._side == f_box._side
			&& min_v_.in(tmp._w._cluster_v))
		{
			if (!etalon_.valid())
			{
				if(tmp._w._cluster_s < pslope_.first)
				{
					comment_ = "minv";
					return;
				}
				etalon_ = tmp._w;
				sb_ = tmp;
				LF::model_send(etalon_, "MIN_V", _t, _model._out_file_boxes, _box_key + "_etalon");
				LF::model_send(tmp, "min_v", _t, _model._out_file_boxes, _box_key + "_et_box");
			}
			else
			{
				if(etalon_._cluster_s > tmp._w._cluster_s*pslope_decline_)
				{
					comment_ = "decline";
					return;
				}
				if(tmp._w._cluster_s < _model._crl_box_min_slope)
				{
					comment_ = "min_slope";
					return;
				}
				if(tmp._w._cluster_s > etalon_._cluster_s)
				{
					etalon_ = tmp._w;
					LF::model_send(etalon_, "EXTEND", _t, _model._out_file_boxes, _box_key + "_etalon");
					LF::model_send(tmp, "extend", _t, _model._out_file_boxes, _box_key + "_et_box");
				}

				if(!sb_.valid() || f_box._side->zsign(tmp._w.charge() - sb_._w.charge()) > 0)
				{
					sb_ = tmp;
				}
			}
		}
	}
}

//======================================================================
void LFMomentumData::set_wave(LFBox& b_, const LFPoint& from_)
{
	b_.reset_w();
	if (!b_._w.cluster_valid() || !from_.valid() || b_._w._dp->_t <= from_._dp->_t)
		return;

	if (_p_store.get() == 0)
		return;

	LFCluster tmp;
	tmp.reset_cluster(b_._side, b_._w._dp, from_._dp, _model._tick_size);
	b_._w_l = tmp._cluster_l;
	b_._w_v = b_._w._cluster_v;
	b_._w_dv = tmp._cluster_v;
	b_._w_s = tmp._cluster_s;
}
//======================================================================
void LFMomentumData::update_scopes(bool force_0_end_)
{
	//Scope 0
	LFScope& scope_0 = scopes()[0];
	scope_0.update(_box, _charge_trend, force_0_end_);
	if (scope_0.empty())
		return;
	
	//scopes 1...
	for(size_t i = 1; i < scopes().size(); ++i)
	{
		LFScope& scope = scopes()[i];
		scope.update();
		if (scope.empty())
			return;
	}
}
//======================================================================
void LFMomentumData::reset_bias(bool add_)
{
	reset_local_bias(add_);
	if (add_)
	{
		LFBoxList& bx = _p_store->_boxes;
		bx.push_front(_box);
		_former_box = _latter_box;
		_latter_box = _box;
		_latter_box.reset_ended();
		LF::model_send(_latter_box, "", _t, _model._out_file_boxes, _box_key);
		std::string com = "replace";
		if (_charge_trend[0].valid() && _charge_trend[0]._side != _latter_box._side)
		{
			com = "add";
			for (size_t i = _charge_trend.size(); i > 1; --i)
				_charge_trend[i - 1] = _charge_trend[i - 2];

		}
		LFBox f = _latter_box;
		f.cut_max();
		_charge_trend[0] = f;
		LF::model_send(_charge_trend[0], com, _t, _model._out_file_boxes, _box_key + "_ct");
		LFBox b;
		b.reset(_latter_box._side->contra_s(), _latter_box._max_dp, _model._tick_size);
		b.merge_same(_latter_box);
		b.set_exp_contra_p(calc_exp_contra_p(b, _latter_box._w.charge(), _latter_box._pias));
		if (_latter_box.exp_valid())
		{
			b._e_in_v = _latter_box.in_exp_v(b._w._dp);
			b._e_w = b._e_v + b._e_in_v;
		}
		for (size_t i = _charge_trend.size(); i > 1; --i)
			_charge_trend[i - 1] = _charge_trend[i - 2];
		_charge_trend[0] = b;

		LF::model_send(_charge_trend[0], "0", _t, _model._out_file_boxes, _box_key + "_ct");

	}
}
//======================================================================
double LFMomentumData::calc_exp_contra_p(const LFBox& b_, double p_charge_1_, double exp_contra_pias_) const
{
	double indep_pias = 3.*b_._w.charge() + p_charge_1_;
	double exp_sax_p = b_._side->contra_s()->sign()*(exp_contra_pias_ - indep_pias) / 2.;

	return b_._side->p_from_v(_box._max_dp->_p, exp_sax_p, _model._tick_size);
}
//======================================================================
void LFMomentumData::reset_local_bias(bool force_overwrite_)
{
	if(!_latter_box.valid() || !_former_box.valid())
		return;
	if(_box._s_bias == LFBox::UNKNOWN_BIAS)
	{
		_box._bias = _latter_box._bias;
		_box._s_bias = _box._bias;
	}
	if(_box._s_pias == LFBox::UNKNOWN_BIAS)
	{
		_box._pias = _latter_box._pias;
		_box._d_pias = _latter_box._d_pias;
		_box._s_pias = _box._pias;
	}

	LFCluster w1;
	LFCluster w2;
	if (_latter_box._side == _box._side)
	{
		w1.reset_cluster(_box._side->contra_s(), _box._s_dp, _latter_box._max_dp, _model._tick_size);
		w2 = _latter_box._w;
	}
	else
	{
		w1 = _latter_box._w;

		if (_latter_box._side == _former_box._side)
		{
			w2.reset_cluster(_latter_box._side->contra_s(), _latter_box._s_dp, _former_box._max_dp, _model._tick_size);
		}
		else
		{
			w2 = _former_box._w;
		}
	}

	double bias = 0;
	if (_model._crl_bias_use_density)
	{
		double tot = _box._w._cluster_density + w1._cluster_density;
		bias = tot <= 0 ? 0 : _box._side->sign()*_box._w._cluster_density / tot;
	}
	else
	{
		double tot = _box._w._cluster_vol + w1._cluster_vol;
		bias = tot <= 0 ? 0 : _box._side->sign()*_box._w._cluster_vol / tot;
	}

	double pias = 2.*_box._w.charge() + 3.*w1.charge() + w2.charge();

	double old_pias = _box._pias;
	if (force_overwrite_)
	{
		if (_box._side->zsign(bias - _box._bias) > 0)
			_box._bias = bias;
		else if (_box._side->zsign(bias) > 0)
			_box._bias = bias;
		else if (_box._s_bias*bias < 0)
			_box._bias = LFBox::BIAS_MIN_VALUE*LF::sign(_box._s_bias);



		if (_box._side->zsign(pias - _box._pias) > 0)
			_box._pias = pias;
		else if (_box._side->zsign(pias) > 0)
			_box._pias = pias;
		else if (_box._s_pias*pias < 0)
			_box._pias = LFBox::BIAS_MIN_VALUE*LF::sign(_box._s_pias);
	}
	else if (_box.triggered())
	{
		if (_box._side->zsign(bias - _box._bias) > 0)
			_box._bias = bias;

		if (_box._side->zsign(pias - _box._pias) > 0)
			_box._pias = pias;
	}

	double d_pias = _box._pias - old_pias;
	if (::fabs(d_pias) > 0.9)
	{
		_box._d_pias = d_pias;
	}

	_box.set_exp_contra_p(calc_exp_contra_p(_box, w1.charge(), _box._pias));

}

//======================================================================
void LFMomentumData::init_merge(LFBox& to_) const
{
	_p_store->init_merge(to_);
}
//======================================================================
void LFMomentumData::set_scope(LFBox& b_) const
{
	if (!b_._w.cluster_valid())
		return;
/*
	size_t pin;
	size_t bin;
	size_t scp = scopes().find_scope(b_._w, pin, bin);
	if (scp < scopes().size())
	{
		b_._w.reset_identity((long)scp, (long)pin, (long)bin);
	}
	else
	{
		b_._w._id._scale =  - 1;
		b_._w._id._roll = 0;
		b_._w._id._child = 0;
	}
	*/
}
//======================================================================
void LFMomentumData::set_bounce(LFBounce& bnc_, const LFCluster& peak_, const LFDataPointPtr& dp_)
{
	if (!peak_.valid() || _p_store.get() == 0)
	{
		bnc_.reset();
		return;
	}
	bnc_.reset(peak_._side->contra_s(), dp_);
	bnc_.set_cluster(peak_._dp, _model._tick_size);
	bnc_.set_bounce(peak_);
}

//======================================================================
void LFMomentumData::calc_point(const std::string& comment_)
{
	size_t n = _p_store->get_n(_model._smoothness_l, _model._smoothness_decay_factor, _model._smoothness_weight_round);
	if(n == 0)
	{
		_obv = 0;
		_vma = 0;
		_mfi = 0;
		_p_activity = 0;
		_v_activity = 0;
		_corr = -1;
		_corr_dyn_high = _model._crl_high;
		_corr_dyn_low = _model._crl_low;
		_p_store->set_prev_point();
		_p_store->set_last_point();
		LF::model_send(*dp(), comment_, _t, _model._out_file_datapoints, _datapoint_key);

	}
	else
	{
		_p_store->set_prev_point();

		size_t ind = _p_store->_t.size() - n;
		// activity
		double dpu = 0;
		double dps = 0;
		double dvu = 0;
		double dvs = 0;
		double dmfu = 0;
		double dmfs = 0;
		_vma = 0;

		for(size_t i = 1; i < n; ++i)
		{
			double pc = _p_store->_p[ind + i] - _p_store->_p[ind + i - 1];
			double vc = (_p_store->_u_vol[ind + i] - _p_store->_d_vol[ind + i])
						- (_p_store->_u_vol[ind + i - 1] - _p_store->_d_vol[ind + i - 1]);
			double tvc = (_p_store->_u_vol[ind + i] + _p_store->_d_vol[ind + i])
						- (_p_store->_u_vol[ind + i - 1] + _p_store->_d_vol[ind + i - 1]);

			double mf = tvc*pc;

			if (pc >= 0)
				dpu += pc*_p_store->_w[ind + i];
			else
				dps -= pc*_p_store->_w[ind + i];
			if (vc >= 0)
				dvu += vc*_p_store->_w[ind + i];
			else
				dvs -= vc*_p_store->_w[ind + i];

			if (pc >= 0)
				dmfu += mf*_p_store->_w[ind + i];
			else
				dmfs -= mf*_p_store->_w[ind + i];
			
			double vma_w = _model._smoothness_vma_linear ? _p_store->_w_linear[ind + i] : _p_store->_w[ind + i];
			_vma += tvc*vma_w;

		}
		if (dmfu + dmfs > 0)
			_mfi = LF::round((dmfu - dmfs)*100. / (dmfu + dmfs), _model._smoothness_activity_round);

		if (dpu + dps > 0)
			_p_activity = LF::round((dpu - dps)*100. / (dpu + dps), _model._smoothness_activity_round);
		double v_base = dvu + dvs;
		if (v_base > 0)
			_v_activity = LF::round((dvu - dvs)*100. / v_base, _model._smoothness_activity_round);

		_p_store->_p_activity.back() = _p_activity;
		_p_store->_v_activity.back() = _v_activity;
		if (::fabs(_p_activity) == 100. && prev_dp()->_p_activity*LF::sign(_p_activity) >= 100.)
			_p_store->_p_activity.back() = prev_dp()->_p_activity + LF::sign(prev_dp()->_p_activity)*0.001;
		if (::fabs(_v_activity) == 100. && prev_dp()->_v_activity*LF::sign(_v_activity) >= 100.)
			_p_store->_v_activity.back() = prev_dp()->_v_activity + LF::sign(prev_dp()->_v_activity)*0.001;
		size_t n = _p_store->get_n(_model._smoothness_corr_l, -1.);
		ind = _p_store->_t.size() - n;

		// correlation
		double* act_base = &_p_store->_v_activity[ind];

		if(_model._spearman_corr)
		{
			LF_gsl_stats_spearman_workspace * w = LF_gsl_stats_spearman_alloc (n);
			gsl_vector_view vgroupa = gsl_vector_view_array_with_stride(act_base, 1, n);
			gsl_vector_view vgroupb = gsl_vector_view_array_with_stride (&_p_store->_p_activity[ind], 1, n);
			_corr = LF_gsl_stats_spearman (&vgroupa.vector, &vgroupb.vector, w);
			LF_gsl_stats_spearman_free (w);
			
		}
		else
			_corr = gsl_stats_correlation(act_base, 1, &_p_store->_p_activity[ind], 1, n);
		
		if(!(_corr >= -1. &&  _corr <= 1.))
			_corr = _p_store->prev_dp()->_corr;
		_p_store->_corr.back() = _corr;


		// set last point with extra data that are not kept int the store
		_p_store->set_last_point(_obv, _vma, _mfi);

		LF::model_send(*dp(), comment_, _t, _model._out_file_datapoints, _datapoint_key);

		ACE_Time_Value t = dp()->_s_t;
		ACE_Time_Value last_t = prev_dp()->_s_t;
		double p = dp()->_p;
		double last_p = prev_dp()->_p;

		// charges

		double pact_sign = LF::sign(p - last_p);
		const LFSide* pact_side = LFSide::s_by_sign(pact_sign);
		bool new_box = false;
		bool new_triggered = false;
		bool new_synthetic = false;
		double dp_charge = LF::charge_i(prev_dp(), dp(), _model._tick_size);
		if (!_box.valid())
		{
			_box.reset(pact_side, prev_dp(), _model._tick_size);
			_box.reset_corr(_corr);
			_corr_dyn_low = _corr - _model._crl_low_offset;
			_corr_dyn_high = std::max(_corr + _model._crl_high_offset, _model._crl_high);
		}
		else if(_corr < _corr_dyn_low && _box._max_corr > _corr_dyn_high)
		{
			new_box = true;
			_corr_dyn_high = std::max(_corr + _model._crl_high_offset, _model._crl_high);
			_box._comment = "CORR";
		}
		else if(_corr > _corr_dyn_high && _box._min_corr < _corr_dyn_low)
		{
			if(!_box.triggered())
			{
				if( ::fabs(_box.max_p_charge()) > _model._crl_split_min
					&& dp_charge*_box._side->sign() <= 0.
					&& (_box.p_charge() + dp_charge - _box.max_p_charge())*_box._side->sign() < -_model._crl_split_offset)
				{
					new_box = true;
					new_triggered = true;
					pact_side = _box._side->contra_s();
					_corr_dyn_low = _corr - _model._crl_low_offset;
					_box._comment = "OFF_SPLIT";
				}
				else if(_box._min_corr < _model._crl_split_negative)
				{
					new_box = true;
					new_triggered = true;
					_corr_dyn_low = _corr - _model._crl_low_offset;
					_box._comment = "CORR_SPLIT";
				}
				else
				{
					_box._triggered_t = t;
				}
			}
			else
			{
				// triggered

				double sign_p_charge = _box.p_charge() + dp_charge;
				if(_model._crl_split_side_charge
					&& sign_p_charge != 0
					&& _latter_box.valid()
					&& _box._side == _latter_box._side
					&& _box._side != LFSide::s_by_sign(LF::sign(sign_p_charge))
					&& ::fabs(_box.max_p_charge()) > _model._crl_split_side_min)
				{
					new_box = true;
					new_triggered = true;
					pact_side = _box._side->contra_s();
					_corr_dyn_low = _corr - _model._crl_low_offset;
					_box._comment = "SIDE_SPLIT";
				}

			}
		}
		LFBox s_box;
		LFCluster s_etalon;
		std::string comment;
		fill_sharp_box(s_box, s_etalon, comment,
			_model._crl_box_sharp_slope, _model._crl_box_sharp_min_v,
			_model._crl_box_sharp_slope_decline, _model._crl_box_sharp_len);
		
		if (!s_box.valid() && _sharp_box.valid() && !_sharp_box.ended())
		{
			s_box.reset(_sharp_box._side, _sharp_box._s_dp, _model._tick_size);
			init_merge(s_box);
			s_box.merge(_box);
		}
		s_box._tip = s_etalon;

		if (s_box.valid())
		{
			LFBounce s_bounce;
			set_bounce(s_bounce, s_box._w, dp());
			if (s_bounce.good(_model._crl_box_bounce_p_pct, _model._crl_box_bounce_l_pct, _model._crl_box_bounce_min_l))
			{
				s_box.set_ended();
				if (!new_box && _box._s_dp->older(*s_box._max_dp))
				{
					new_box = true;
					new_triggered = _box.triggered();
					new_synthetic = true;
					_corr_dyn_low = _corr - _model._crl_low_offset;
					_corr_dyn_high = std::max(_corr + _model._crl_high_offset, _model._crl_high);
					_box._comment = "SHARP";
				}
			}
		}

		if (_model._crl_split_charge
			&&	!new_box
			&& _box.valid() && _latter_box.valid()
			&& _box.triggered()
			&& _box._side != _latter_box._side
			&& _latter_box._side == _former_box._side
			&& _box.length() > _latter_box.length() + _former_box.length()
			&& _box._side->sign()*LF::sign(_box.p_charge() + dp_charge) < 0)
		{
			new_box = true;
			new_triggered = _box.triggered();
			new_synthetic = true;
			_corr_dyn_low = _corr - _model._crl_low_offset;
			_corr_dyn_high = std::max(_corr + _model._crl_high_offset, _model._crl_high);
			_box._comment = "LEN_SPLIT";
		}

		if(new_box)
		{
			reset_bias(true);
			if (_model._crl_box_sharp_scope_based)
			{
				//AK TODO do it after model update
				LFCluster s_etalon;
				std::string comment;
				fill_sharp_roll(_sharp_box, s_etalon, comment,
					_model._crl_box_sharp_slope, _model._crl_box_sharp_min_v,
					_model._crl_box_sharp_slope_decline, _model._crl_box_sharp_len);
				if (_sharp_box.valid())
				{
					LF::model_send(_sharp_box, comment, _t, _model._out_file_boxes, _box_key + "_sharp");
					LF::model_send(_sharp_box._w, comment, _t, _model._out_file_boxes, _box_key + "_sharp_w");
				}
			}
			else
			{
				_sharp_box = s_box;
				if (_sharp_box.valid())
				{
					set_scope(_sharp_box);
					LF::model_send(_sharp_box, comment, _t, _model._out_file_boxes, _box_key + "_sharp");
					LF::model_send(_sharp_box._w, comment, _t, _model._out_file_boxes, _box_key + "_sharp_w");
					LF::model_send(_sharp_box._tip, comment, _t, _model._out_file_boxes, _box_key + "_sharp_tip");
				}
			}

			_box.reset(pact_side, prev_dp(), _model._tick_size, _latter_box._w);
			_box.reset_corr(_corr);

			if(new_triggered)
				_box._triggered_t = last_t;
		} 
		if(_box.valid())
		{
			const LFSide* old_p_side = _box._side;
			if(_corr > _box._max_corr)
			{
				if(_box._min_corr >= _corr_dyn_low)
					_box._min_corr = _corr;
				if(_corr > _corr_dyn_high)
					_corr_dyn_low = _corr - _model._crl_low_offset;
				_box._max_corr = _corr;
			}
			else if(_corr < _box._min_corr)
			{
				if(_box._max_corr <= _corr_dyn_high)
					_box._max_corr =_corr;
				if(_corr < _corr_dyn_low)
				{
					_corr_dyn_high = std::max(_corr + _model._crl_high_offset, _model._crl_high);
				}
				_box._min_corr = _corr;
			}

			double sign_p_charge = _box.p_charge() + dp_charge;
			if(sign_p_charge != 0)
			{
				double pbsign = LF::sign(sign_p_charge);
				_box.reset_side(LFSide::s_by_sign(pbsign), _latter_box._w);
			}

			_box.update_current(dp());
			reset_bias(false);
			if(_box._side != old_p_side)
				LF::model_send(_box, "p_side", _t, _model._out_file_boxes, _box_key);
			update_scopes(new_box && _model._crl_g0_end_on_sharp && s_box.valid() && s_box.ended());
			scopes().fill_box_trends(_box_trends);
			scopes().fill_lcy_trends(_lcy, _lcy_trends);
		}
	}

}

//======================================================================
