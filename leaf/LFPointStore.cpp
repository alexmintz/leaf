/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFPointStore.h"
#include <cstdlib> 
#include <set>
#include <cfloat>
#include <iomanip>

//=================================================================================
//======================================================================
//======================================================================
LFPointStore::LFPointStore(LFMarketModel* model_)
	: _model(model_)
{
}
LFPointStore::~LFPointStore()
{
}

void LFPointStore::reset()
{
	_s_t = ACE_Time_Value::zero;
	_tt.clear();
	_t.clear();
	_w.clear();
	_w_linear.clear();
	_p.clear();
	_p_activity.clear();
	_v_activity.clear();
	_corr.clear();
	_u_vol.clear();
	_u_tran.clear();
	_d_vol.clear();
	_d_tran.clear();

	_dp.reset();
	_prev_dp.reset();
	_boxes.clear();
	_to_report.clear();

}

void LFPointStore::set_prev_point()
{
	if (_dp.get() != 0)
	{
		_prev_dp = _dp;
	}
	else
	{
		_prev_dp.reset(new LFDataPoint());
		fill_prev_point(*_prev_dp);
	}
}
void LFPointStore::set_last_point(double obv_, double vma_, double mfi_)
{
	_dp.reset(new LFDataPoint());
	fill_last_point(*_dp);
	_dp->_obv = obv_;
	_dp->_vma = vma_;
	_dp->_mfi = mfi_;

}

void LFPointStore::fill_point(LFDataPoint& dp_, size_t ind_) const
{
	dp_.reset(_tt[ind_], _t[ind_],
		_p[ind_], _p_activity[ind_], _v_activity[ind_], _corr[ind_],
		_u_vol[ind_], _u_tran[ind_], _d_vol[ind_], _d_tran[ind_]);
}

void LFPointStore::fill_last_point(LFDataPoint& dp_) const
{
	dp_.reset();
	if (_s_t == ACE_Time_Value::zero || _t.empty())
		return;

	fill_point(dp_, _t.size() - 1);
}
void LFPointStore::fill_prev_point(LFDataPoint& dp_) const
{
	dp_.reset();
	if (_s_t == ACE_Time_Value::zero || _t.size() < 2)
		return;
	fill_point(dp_, _t.size() - 2);
}

bool LFPointStore::add_point(const ACE_Time_Value& t_, double p_, double u_vol_, double d_vol_, double u_tran_, double d_tran_)
{
	if (_s_t == ACE_Time_Value::zero)
	{
		_s_t = t_;
	}
	if (t_ < _s_t)
		return false;

	double ll = LF::length_from_t(t_ - _s_t);
	if (_t.size() > 0 && ll <= _t.back())
		return false;
	_tt.push_back(t_);

	_t.push_back(ll);
	_w.push_back(1);
	_w_linear.push_back(1);
	_p.push_back(p_);
	_p_activity.push_back(0);
	_v_activity.push_back(0);
	_corr.push_back(-1);

	_u_vol.push_back(u_vol_);
	_u_tran.push_back(u_tran_);
	_d_vol.push_back(d_vol_);
	_d_tran.push_back(d_tran_);

	return true;
}

bool LFPointStore::add_virt_point(const ACE_Time_Value& t_, double p_, double u_vol_, double d_vol_, double u_tran_, double d_tran_)
{
	if (!would_add_virt_point(t_))
		return false;
	return add_point(t_, p_, u_vol_, d_vol_, u_tran_, d_tran_);
}
bool LFPointStore::would_add_virt_point(const ACE_Time_Value& t_) const
{
	if (_s_t == ACE_Time_Value::zero || _t.size() == 0 || t_ < _s_t
		|| LF::length_from_t(t_ - _s_t) - _t.back() < _model->_smoothness_min_l)
		return false;
	return true;
}
size_t LFPointStore::get_n(double l_, double weight_factor_, double weight_prec_, size_t min_n_)
{

	if (min_n_ == 0)
		min_n_ = 3;
	if (_t.size() <= min_n_)
		return 0;
	double ll = _t.back();
	for (size_t i = min_n_ + 1; i <= _t.size(); ++i)
	{
		size_t ind = _t.size() - i;
		double tl = ll - _t[ind];

		if (tl >= l_)
		{
			return i - 1;
		}
		_w_linear[ind] = LF::round((l_ - tl) / l_, weight_prec_);
		if (weight_factor_ > 0)
			_w[ind] = LF::round(exp(-tl*weight_factor_ / l_), weight_prec_);
		else if (weight_factor_ == 0)
			_w[ind] = _w_linear[ind];
	}
	return 0;

}
void LFPointStore::find_point(LFDataPoint& dp_, double p_limit_, const LFSide* p_side_, double max_l_) const
{
	dp_.reset();
	if (_s_t == ACE_Time_Value::zero || _t.empty())
		return;

	double l_limit = _t[_t.size() - 1] - max_l_;
	for (size_t i = _t.size(); i > 0; --i)
	{
		if (_t[i - 1] < l_limit)
		{
			fill_point(dp_, i - 1);
			return;
		}
		if (p_side_->b_diff(_p[i - 1], p_limit_) > 0)
		{
			fill_point(dp_, i - 1);
			return;
		}

	}
}
//======================================================================
void LFPointStore::find_tip(LFCluster& tip_, const LFCluster& c_, double min_v_) const
{
	tip_.reset();
	if (_s_t == ACE_Time_Value::zero || _t.empty() || !c_.cluster_valid() || c_._dp->_t <= _s_t)
	{
		return;
	}

	for (size_t i = _t.size(); i > 0 && _tt[i - 1] >= c_._cluster_dp->_t; --i)
	{
		if (_tt[i - 1] < c_._dp->_t)
		{
			LFCluster tmp = c_;
			LFDataPointPtr dp(new LFDataPoint());
			fill_point(*dp, i - 1);
			tmp.set_cluster(dp, _model->_tick_size);
			if (tmp._cluster_v > min_v_
				&& (!tip_.valid() || tmp._cluster_s >= tip_._cluster_s))
			{
				tip_ = tmp;
			}
		}
	}
}
//======================================================================
LFMatchPtr LFPointStore::make_report(const std::string& name_, const std::string& key_base_, const LFSide* side_) const
{
	LFMatchPtr m;
	m.reset(new LFMatch(name_, name_ + "_" + key_base_, false));
	m->reset(side_, _dp);
	return m;
}
//======================================================================
LFMatchPtr LFPointStore::make_report(const std::string& name_, const std::string& key_base_, const LFBox& box_) const
{
	LFMatchPtr m = make_report(name_, key_base_, box_._side);
	m->_box = box_;
	return m;
}
//======================================================================
void LFPointStore::report(const LFMatchPtr& match_)
{
	_to_report.push_back(match_);
}
//======================================================================
void LFPointStore::report(const std::string& name_, const std::string& key_base_, bool out_file_)
{
	if (!out_file_)
		return;
	report(make_report(name_, key_base_, 0));
}

//======================================================================
void LFPointStore::report(const std::string& name_, const std::string& key_base_, const LFBox& box_, bool out_file_)
{
	if (!out_file_)
		return;
	report(make_report(name_, key_base_, box_));
}

//======================================================================
void LFPointStore::init_merge(LFBox& to_, const LFBox& cur_b_) const
{
	init_merge(to_);
	const LFBoxList& bx = _boxes;
	if (bx.empty() || bx.front()._c_dp->older(*cur_b_._c_dp))
		to_.merge(cur_b_);
}
//======================================================================
void LFPointStore::init_merge(LFBox& to_) const
{
	const LFBoxList& bx = _boxes;

	if (bx.empty())
	{
		to_.reset();
		return;
	}
	if (bx.front()._c_dp->_t <= to_._s_dp->_t)
		return;

	LFBoxList::const_reverse_iterator ri = bx.rbegin();
	for (LFBoxList::const_iterator i = bx.begin(); i != bx.end(); ++i)
	{
		const LFBox& si = *i;

		if (si._c_dp->_t <= to_._c_dp->_t)
		{
			ri = LFBoxList::const_reverse_iterator(i);
			break;
		}
		if (si._s_dp->_t <= to_._s_dp->_t)
		{
			ri = LFBoxList::const_reverse_iterator(++i);
			break;
		}
	}

	for (; ri != bx.rend(); ++ri)
		to_.merge(*ri);
}

//======================================================================
