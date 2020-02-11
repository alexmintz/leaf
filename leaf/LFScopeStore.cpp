
/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFScopeStore.h"
#include <cstdlib> 
#include <set>
#include <cfloat>
#include <iomanip>

//======================================================================
LFBoxTrend::LFBoxTrend()
	: Papa(TREND_SIZE)
{
}
//======================================================================
bool LFBoxTrend::operator==(const LFBoxTrend& s_) const
{
	const Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
	{
		if (!self[i].t_equal(s_[i]))
			return false;
	}
	return true;
}
//======================================================================
const LFBox& LFBoxTrend::operator[](size_t i) const
{ 
	return Papa::operator[](i); 
}
//======================================================================
LFBox& LFBoxTrend::operator[](size_t i)
{ 
	return Papa::operator[](i); 
}
//======================================================================
size_t LFBoxTrend::size() const
{
	return Papa::size();
}
//======================================================================
void LFBoxTrend::reset()
{
	Papa(TREND_SIZE).swap(*this);
}

//======================================================================
//======================================================================
LFBoxTrends::LFBoxTrends()
	: Papa(LFScopeStore::NUMBER_OF_SCOPES)
{
}
//======================================================================
const LFBoxTrend& LFBoxTrends::operator[](size_t i) const
{ 
	return Papa::operator[](i); 
}
//======================================================================
LFBoxTrend& LFBoxTrends::operator[](size_t i)
{ 
	return Papa::operator[](i); 
}
//======================================================================
size_t LFBoxTrends::size() const
{
	return Papa::size();
}

//======================================================================
void LFBoxTrends::reset()
{
	Papa(LFScopeStore::NUMBER_OF_SCOPES).swap(*this);
}
//======================================================================
bool LFBoxTrends::operator==(const LFBoxTrends& s) const
{
	const Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
	{
		if (self[i] != s[i])
			return false;
	}
	return true;
}

//======================================================================
//======================================================================
LFScope::LFScope()
	: _s_store(0),
	_p_store(0),
	_model(0),
	_scale(0),
	_avg_l(0),
	_avg_v(0),
	_avg_cnt(0),
	_min_l(0),
	_min_v(0),
	_joined(-1),
	_just_ended_side(0)

{
}
//======================================================================
void LFScope::init(LFScopeStore* s_store_, LFPointStore* p_store_, const std::string& prefix_, size_t scale_)
{
	_s_store = s_store_;
	_p_store = p_store_;
	_model = _p_store->_model;
	_prefix = prefix_;
	_scale = (long)scale_;
	set_key(_model->_name + "_" + _prefix);

}

//======================================================================
void LFScope::reset()
{
	Papa().swap(*this);
	_avg_l = 0;
	_avg_v = 0;
	_avg_cnt = 0;
	_min_l = 0;
	_min_v = 0;
	_joined = -1;
	_just_ended_side = 0;
	_crl_bias_min_l = 0;
	_crl_bias_min_v = 0;

	std::string base = "LFModel:" + _model->_market + ":crl_" + _prefix + "_";
	_crl_bias_min_l = Z::get_setting_double(base + "bias_min_l", 0.5);
	_crl_bias_min_v = Z::get_setting_double(base + "bias_min_v", 0.5);

}
//======================================================================
size_t LFScope::get_ind(const LFSide* side_, bool ended_) const
{
	const Papa& self = *this;
	size_t sz = size();
	for (size_t i = sz; i > 0; --i)
	{
		if ((side_ == 0 || self[i - 1]._side == side_)
			&& (!ended_ || i < sz || self[i - 1].ended()))
		{
			return i - 1;
		}
	}
	return sz;
}

//======================================================================
size_t LFScope::find_ind_best(const LFSide* side_, const ACE_Time_Value& t_) const
{
	size_t sz = size();
	/* AK TODO
	const Papa& self = *this;
	for (size_t i = 0; i < sz; ++i)
	{
		if (self[i].valid() && self[i]._max_dp->_t <= t_)
		{
			if (self[i]._side == side_)
				return i;
			return i == 0 ? sz : i - 1;
		}
	}
	*/
	return sz;
}
//======================================================================
void LFScope::find_indices(const LFCluster& c_, size_t& pin_, size_t& bin_) const
{
	const Papa& self = *this;
	pin_ = find_ind_best(c_._side, c_._dp->_t);
	/* AK TODO
	if (pin_ < size() && self[pin_].valid() && self[pin_]._max_dp->_t != c_._dp->_t)
		pin_ = size();
		*/
	bin_ = find_ind_best(c_._side->contra_s(), c_._cluster_dp->_t);
}

//======================================================================
void LFScope::model_send(size_t ind_, const std::string& comment_, const ACE_Time_Value& t_, bool out_file_, const std::string& key_suffix_) const
{
	if (!out_file_)
		return;
	const Papa& self = *this;
	std::string key = _key + key_suffix_;
	std::ostringstream os;
	os << comment_ << "_" << ind_;
	LF::model_send(self[ind_], os.str(), t_, out_file_, key);
}
//======================================================================
void LFScope::clear_just_flags()
{
	_just_ended_side = 0;
}
//======================================================================
void LFScope::fill_box_trend(LFBoxTrend& tr_) const
{
	Papa::const_reverse_iterator it = rbegin();
	for (size_t i = 0; i < tr_.size(); ++i)
	{
		if (it != rend())
		{
			tr_[i] = (*it);
			++it;
		}
		else
		{
			tr_[i].reset();
		}
	}
}
//======================================================================
void LFScope::push_box(const LFBox& b_)
{
	LFBox tmp = b_;
	if (!empty())
	{
		LFBox cont;
		tmp.split_min(cont, _model->_tick_size);
		if (cont.valid())
		{
			back().merge(cont, true);
			update_roll(last_ind());
			calc_bias(last_ind());
		}
	}
	tmp._w_changed = true;
	push_back(tmp);
	update_roll(last_ind(), true);
	update_avg();
	calc_bias(last_ind());
}

//======================================================================
void LFScope::update_current(const LFDataPointPtr& dp_)
{
	if (empty())
		return;
	back().update_current(dp_);

	update_roll(last_ind());
	calc_bias(last_ind());
}
//======================================================================
void LFScope::replace_box(size_t ind_, const LFBox& b_)
{
	(*this)[ind_] = b_;
	update_roll(ind_);
	calc_bias(ind_);
}
//======================================================================
bool LFScope::set_ended(size_t ind_)
{
	Papa& self = *this;
	if (self[ind_].ended())
		return false;
	self[ind_].set_ended();

	return true;
}
//======================================================================
bool LFScope::is_in(long id_) const
{
	if (id_ < 2)
		return false;
	return roll(id_).value(roll(id_ - 2)._max_dp) < 0;
}
//======================================================================
bool LFScope::is_touch_or_out(long id_) const
{
	const Papa& self = *this;
	if (id_ < 2)
		return false;
	return roll(id_).value(roll(id_ - 2)._max_dp) >= 0;
}
//======================================================================
void LFScope::add_joined(LFJoinIndex j_ind_)
{
	if (j_ind_.first < 0)
		return;

	if (_joined < 0 || _joined > j_ind_.first)
		_joined = j_ind_.first;

}

//======================================================================
long LFScope::join(size_t ind_, bool join_pinned_)
{
	Papa& self = *this;
	if (ind_ < 2)
		return -1;

	LFJoinIndex ind((long)ind_, 2);

	
	while (valid_id(ind.first) && is_in(ind.first))
	{
		ind.first++;
	}
	if (!valid_id(ind.first))
	{
		ind.first = last_id();
		ind.second = 1;
	}
	else if (ind.first == (long)ind_ && is_touch_or_out(ind.first - 1))
	{
		ind.first--;
		if (is_touch_or_out(ind.first - 1))
			return -1;
	}


	for (long ii = 0; ii < ind.second; ++ii)
	{
		const LFBox& b = roll(ind.first - ii - 1);
		if (!join_pinned_ && b._pinned)
			return -1;
		//AK TODO remove _crl_join_check_parrent from settings
		if (_model->_crl_join_check_parrent && _s_store->find_parent_index(b._w) >= 0)
			return -1;
	}
	// ind.first points to result of join
	ind.first -= ind.second;

	LFBox tmp;
	init_merge(tmp, ind.first, ind.first + ind.second + 1);
	replace_box(ind.first, tmp);

	for (long i = ind.first + 1; valid_id(i + ind.second); ++i)
	{
		replace_box(i, roll(i + ind.second));
	}
	size_t off = size() - (size_t)ind.second;
	erase(begin() + off, end());
	roll(ind.first)._pinned = _model->_crl_pin_joined && !join_pinned_;
	add_joined(ind);
	return ind.first;
}
//======================================================================
bool LFScope::joinable(size_t ind_, std::string& reason_)
{
	Papa& self = *this;
	if (_min_l <= 0 && _min_v <= 0)
		return false;

	if ((_min_l <= 0 || self[ind_]._w_l < _min_l)
		&& (_min_v <= 0 || self[ind_]._w_v < _min_v))
	{
		std::ostringstream os;
		os << std::setprecision(1) << std::fixed;
		os << "[" << self[ind_]._w_l << "_" << _min_l
			<< "_lv_"
			<< self[ind_]._w_v << "_" << _min_v << "]";
		reason_ = os.str();
		return true;
	}
	return false;
}

//======================================================================
long LFScope::check_join(size_t ind_, std::string& reason_)
{

	if (empty() || ind_ < 2 || ind_ == last_ind() )
		return -1;

	if (!joinable(ind_, reason_))
		return -1;

	return join(ind_);

}
//======================================================================
long LFScope::check_join(std::string& reason_)
{
	if (size() < 3)
		return -1;

	long j = check_join(size() - 2, reason_);
	if (j >= 0)
		return j;
	j = check_join(size() - 3, reason_);
	return j;
}
//======================================================================
long LFScope::check_child_join(std::string& reason_)
{
	if (_scale <= 0)
		return -1;
	LFScope& child = _s_store->scope(_scale - 1);
	if (child._joined < 0)
		return -1;
	
	long id = find_lower_index(_s_store->roll(_scale - 1, child._joined)._w);
	if (id >= 0)
		update_identities(id);

	if (size() < 3)
		return -1;

	long affected_id = 0;
	if (id > 0)
		affected_id = id - 1;
	else if(id < 0)
		affected_id = last_id() - 1;

	std::ostringstream os;
	os << "(cji_" << child._joined << "_aff_" << affected_id << ")";
	reason_ = os.str();
	for (long i = last_id() - 1; i >= affected_id; --i)
	{
		std::ostringstream os;
		os << "|" << i << "_";
		reason_ += os.str();
		if (check_not_ended(i, reason_))
		{
			return join(i + 1, true);
		}
	}
	LF::model_send(back(), reason_, _p_store->dp()->_t, _model->_out_file_boxes_trace, get_key() + "_nojoin");
	return -1;
}
//======================================================================
bool LFScope::check_not_ended(long id_, std::string& reason_)
{
	Papa& self = *this;

	if (!valid_id(id_ + 1))
		return false;

	LFBox& rl = roll(id_);

	double p = roll(id_ + 1)._w._dp->_p;

	// check for E_x
	if (valid_id(id_ - 1) 
		&& roll(id_ - 1).value(p) < 0)
	{
		reason_ += "x";
		return false;
	}
	// check for E_touch
	if (valid_id(id_ - 1)
		&& roll(id_ - 1).value(p) == 0)
	{
		reason_ += "touch";
		return false;
	}
	// check for E_bloss
	if (_s_store->child_confirm_bounce(rl))
	{
		reason_ += "bloss";
		return false;
	}

	// check for E_exp
	if (_s_store->child_confirm_exp(rl))
	{
		reason_ += "exp";
		return false;
	}

	return true;
}
//======================================================================
bool LFScope::check_init(const LFBoxTrend& charge_trend_)
{
	if (!empty())
		return true;
	
	size_t ch_pin = 1;

	if (!charge_trend_[3]._w.cluster_valid())
		return false;
	if (charge_trend_[3].value(charge_trend_[1]._w._dp) > 0
		|| charge_trend_[3].value(charge_trend_[1]._w._cluster_dp) > charge_trend_[3]._w._cluster_v)
	{
		return false;
	}
	
	LFBox tmp;
	tmp.reset_from(charge_trend_[3]);
	tmp.merge(charge_trend_[2]);
	tmp.merge(charge_trend_[1]);

	push_box(tmp);
	LF::model_send(back(), "init" + min_as_string(), _p_store->dp()->_t, _model->_out_file_boxes, get_key());

	return true;
}
//======================================================================
bool LFScope::check_init()
{
	if (_scale == 0)
		return false;
	LFScope& child = _s_store->scope(_scale - 1);
	if (child.size() < 3)
		return false;

	size_t ch_last = child.last_ind();

	if (child[ch_last - 2].value(child[ch_last]._w._dp) > 0
		|| child[ch_last - 2].value(child[ch_last]._w._cluster_dp) > child[ch_last - 2]._w._cluster_v)
	{
		return false;
	}
	Papa& self = *this;
	if (child.size() > 3)
	{
		LFBox tmp;
		tmp.reset_from(child[ch_last - 3]);
		push_box(tmp);
		LF::model_send(back(), "init" + min_as_string(), _p_store->dp()->_t, _model->_out_file_boxes, get_key());
	}

	LFBox tmp;
	tmp.reset_from(child[ch_last - 2]);
	tmp.merge(child[ch_last - 1]);
	tmp.merge(child[ch_last]);
	
	push_box(tmp);
	LF::model_send(back(), "init" + min_as_string(), _p_store->dp()->_t, _model->_out_file_boxes, get_key());

	return true;
}
//======================================================================
bool LFScope::check_set_ended(const LFBoxTrend& charge_trend_, bool force_ended_, std::string& action_)
{
	if (empty())
	{
		// init
		if (!check_init(charge_trend_))
			return false;
	}

	Papa& self = *this;
	LFBox& rl = back();
	if (rl.ended())
		return false;

	if (rl.value(_p_store->dp()) <= 0)
		return false;

	if (force_ended_)
	{
		set_ended(last_ind());
		action_ = "E_f";
		return true;
	}

	double p = _p_store->_p.back();

	const LFBox& charge = (rl._side == charge_trend_[1]._side) ? charge_trend_[1] : charge_trend_[2];

	if (charge.valid() && !charge._w.includes(rl._w))
	{
		if (charge.exp_valid()
			&& charge._w._dp->_t >= rl._w._dp->_t
			&& rl.value(charge._w._dp) >= 0)
		{
			if (charge.in_exp_pct(p) < -_model->_crl_in_exp_pct)
			{
				set_ended(last_ind());
				action_ = "E_exp";
				return true;
			}
		}
	}
	const LFBox& charge0 = charge_trend_[0];
	if (charge0._side == rl._side
		&& charge0.exp_valid()
		&& charge0._w._dp->_t >= rl._w._dp->_t
		&& rl.value(charge0._w._dp) >= 0)
	{
		if (charge0.in_exp_pct(p) < -_model->_crl_in_exp_pct)
		{
			set_ended(last_ind());
			action_ = "E_0exp";

			return true;
		}
	}
	if (size() < 2)
		return false;

	LFBox& prev = self[size() - 2];

	if (prev.value(p) < 0)
	{
		set_ended(last_ind());
		action_ = "E_x";
		return true;
	}
	if (prev.value(p) == 0)
	{
		set_ended(last_ind());
		action_ = "E_touch";
		return true;
	}

	return false;
}
//======================================================================
bool LFScope::check_set_ended(std::string& action_)
{
	if (_scale <= 0)
		return false;
	if (empty())
	{
		// init 
		if (!check_init())
			return false;
	}
	Papa& self = *this;
	LFBox& rl = back();

	if (rl.ended())
		return false;

	if (rl.value(_p_store->dp()) <= 0)
		return false;

	double p = _p_store->dp()->_p;

	LFScope& child = _s_store->scope(_scale - 1);

	if (_s_store->child_confirm_bounce(rl)
		&& child.back()._side != rl._side
		&& child.back().at_max())
	{
		set_ended(last_ind());
		action_ = "E_bloss";
		return true;
	}

	if (_s_store->child_confirm_exp(rl)
		&& child.back()._side != rl._side
		&& child.back().at_max())
	{
		set_ended(last_ind());
		action_ = "E_exp";
		return true;
	}

	if (size() < 2)
		return false;

	LFBox& prev = self[size() - 2];

	if (prev.value(p) < 0)
	{
		set_ended(last_ind());
		action_ = "E_x";
		return true;
	}
	if (prev.value(p) == 0)
	{
		set_ended(last_ind());
		action_ = "E_touch";
		return true;
	}
	return false;
}

//======================================================================
void LFScope::check_set_continuous(long id_)
{
	if (!valid_id(id_))
		return;
	LFBox& rl = roll(id_);
	if (_scale == 0)
	{
		rl._w._pattern.set(LF::pb_COUNTINUOUS);
		return;
	}
	LF::Id bnc_id = _s_store->bounce_id(rl.id());
	if (bnc_id.valid() && bnc_id._scale < _scale)
	{
		// in bounce
		if (!rl._w._pattern.test(LF::pb_INTERRUPTED) && rl._w._pattern.test(LF::pb_COUNTINUOUS))
		{
			double cnty = rl._w.continuity(_s_store->roll(bnc_id)._w, *_model);
			if (cnty < _model->_crl_continuous)
			{
				rl._w._pattern.set(LF::pb_INTERRUPTED);
				std::ostringstream os;
				
				os << std::setprecision(2) << std::fixed << cnty << "_I";
				LF::model_send(_s_store->roll(bnc_id), os.str(), _p_store->dp()->_t, _model->_out_file_blossom.in(_scale), get_key() + "_blossom");
				LF::model_send(rl, "in_bounce", _p_store->dp()->_t, _model->_out_file_blossom.in(_scale), get_key() + "_blossom");
			}
		}
		return;
	}
	if (rl._w._pattern.test(LF::pb_INTERRUPTED))
	{
		rl._w._pattern.set(LF::pb_COUNTINUOUS, false);
		rl._w._pattern.set(LF::pb_INTERRUPTED, false);
	}
	if (rl._w._pattern.test(LF::pb_COUNTINUOUS))
		return;
	LF::Id fc_id = _s_store->first_child_id(rl.id());
	if (fc_id._roll == rl.id()._child)
	{
		// follow first child
		rl._w._pattern.set(LF::pb_COUNTINUOUS, _s_store->roll(fc_id)._w._pattern.test(LF::pb_COUNTINUOUS));
		LF::model_send(rl, "first_child", _p_store->dp()->_t, _model->_out_file_blossom.in(_scale), get_key() + "_blossom");
		return;
	}

	LF::IdVec bloss_ids;
	_s_store->fill_blossom_ids(rl.id(), _scale, bloss_ids);

	std::string comment = rl.id().to_alfa();
	bool cntous = true;
	bool straight = true;
	for (size_t i = 0; i < bloss_ids.size(); ++i)
	{
		std::ostringstream os;
		const LF::Id& b_id = bloss_ids[i];
		double cnty = rl._w.continuity(_s_store->roll(b_id._scale, b_id._roll + 1)._w, *_model);
		os << std::setprecision(2) << std::fixed << cnty;
		if (cnty < _model->_crl_continuous)
		{
			cntous = false;
			straight = false;
		}
		else
		{
			if (cnty < _model->_crl_continuous_straight)
			{
				os << "_C";
				straight = false;
			}
			else
			{
				os << "_S";
				_s_store->roll(b_id)._w._id._top_scale = _scale - 1;
			}
		}
		LF::model_send(_s_store->roll(b_id), os.str(), _p_store->dp()->_t, _model->_out_file_blossom.in(_scale), get_key() + "_blossom");
	}
	if (cntous)
		rl._w._pattern.set(LF::pb_COUNTINUOUS);
	if (straight)
		rl._w._pattern.set(LF::pb_STRAIGHT);

	LF::model_send(rl, "in_peak", _p_store->dp()->_t, _model->_out_file_blossom.in(_scale), get_key() + "_blossom");
}

//======================================================================
void LFScope::check_set_pattern()
{
	if (empty())
		return;

	LFBox& rl = back();

	//accelerated
	if (size() < 2)
		return;
	Papa& self = *this;
	LFBox& prev = self[size() - 2];
	if (!rl._w._pattern.test(LF::pb_ACCELERATED)
		&& prev.in_exp_v(rl._w._dp) < -0.9)
	{
		rl._w._pattern.set(LF::pb_ACCELERATED);
	}
}
//======================================================================
void LFScope::update(const LFBox& b_, const LFBoxTrend& charge_trend_, bool force_ended_)
{
	const LFSide* back_side = empty() ? 0 : back()._side;

	reset_joined();
	set_min(_avg_l*_crl_bias_min_l, _avg_v*_crl_bias_min_v);
	update_current(_p_store->dp());

	std::string action;
	ACE_Time_Value t = _p_store->dp()->_t;

	if (check_set_ended(charge_trend_, force_ended_, action))
	{
		LF::model_send(back(), action, t, _model->_out_file_boxes, get_key());

		LFBox bounce;
		if (_model->_box_trace.in_range(t))
			bounce.set_trace(t, get_key() + "_I_bounce");
		else
			bounce.set_trace();

		LFBox f = back();
		f.cut_max();
		bounce.reset(f._side->contra_s(), f._max_dp, _model->_tick_size);
		_p_store->init_merge(bounce, b_);

		LF::model_send(bounce, "I_bounce", t, _model->_out_file_boxes_trace, get_key());
		bounce.set_trace();

		if (bounce._min_dp->_t <= bounce._max_dp->_t)
		{
			replace_box(last_ind(), f);
			LF::model_send(back(), "front", t, _model->_out_file_boxes_trace, get_key());
			push_box(bounce);
			LF::model_send(back(), "new"+ min_as_string(), t, _model->_out_file_boxes, get_key());
		}
		else
		{
			//AK TODO -- check if it ever happens
			bounce.cut_max();
			LFBox peak;
			peak.reset(bounce._side->contra_s(), bounce._max_dp, _model->_tick_size);
			_p_store->init_merge(peak, b_);
			replace_box(last_ind(), f);
			LF::model_send(back(), "front", t, _model->_out_file_boxes, get_key());
			push_box(bounce);
			LF::model_send(back(), "bounce" + min_as_string(), t, _model->_out_file_boxes, get_key());
			push_box(peak);
			LF::model_send(back(), "new" + min_as_string(), t, _model->_out_file_boxes, get_key());
		}
		std::string j_reason;
		long j_ind = check_join(j_reason);
		if (j_ind >= 0)
		{
			std::ostringstream os;
			os << "J[" << j_ind << "]_" << j_reason;
			Papa& self = *this;
			LF::model_send(self[j_ind], os.str(), t, _model->_out_file_boxes, get_key());
		}
	}
	check_set_pattern();
	if (!empty())
	{
		if (back_side != back()._side)
			_just_ended_side = back_side;

		LF::model_send(back(), "", t, _model->_out_file_boxes_trace, get_key());
	}

}
//======================================================================
void LFScope::update()
{
	if (_scale <= 0)
		return;
	Papa& self = *this;
	const LFSide* back_side = empty() ? 0 : back()._side;
	reset_joined();
	set_min(_avg_l*_crl_bias_min_l, _avg_v*_crl_bias_min_v);
	update_current(_p_store->dp());

	std::string action;
	ACE_Time_Value t = _p_store->dp()->_t;

	std::string jreason;
	long j = check_child_join(jreason);
	if (j >= 0)
	{
		std::ostringstream os;
		os << "CJ[" << j << "]_" << jreason;
		LF::model_send(self[j], os.str(), t, _model->_out_file_boxes, get_key());
	}

	if (check_set_ended(action))
	{
		LF::model_send(back(), action, t, _model->_out_file_boxes, get_key());

		LFBox bounce;
		if (_model->_box_trace.in_range(t))
			bounce.set_trace(t, get_key() + "_I_bounce");
		else
			bounce.set_trace();

		LFBox f = back();

		if (!_s_store->child_valid(f._w) || !_s_store->valid_id(f._w._id._scale - 1, f._w._id._child + 1))
		{
			_p_store->report("update_no_child_" + _prefix, _model->_name, f, _model->_report_scope_sanity);
			return;
		}
		f.cut_max();
		
		_s_store->init_merge(bounce, f._w._id._scale - 1, f._w._id._child + 1);

		LF::model_send(bounce, "I_bounce", t, _model->_out_file_boxes_trace, get_key());
		bounce.set_trace();

		replace_box(last_ind(), f);
		LF::model_send(back(), "front", t, _model->_out_file_boxes, get_key());
		push_box(bounce);
		LF::model_send(back(), "new" + min_as_string(), t, _model->_out_file_boxes, get_key());

		std::string j_reason;
		long j_ind = check_join(j_reason);
		if (j_ind >= 0)
		{
			std::ostringstream os;
			os << "J[" << j_ind << "]_" << j_reason;
			LF::model_send(self[j_ind], os.str(), t, _model->_out_file_boxes, get_key());
		}
	}
	check_set_pattern();
	if (!empty())
	{
		if (back_side != back()._side)
			_just_ended_side = back_side;

		LF::model_send(back(), "", t, _model->_out_file_boxes_trace, get_key());
	}

}

//======================================================================
void LFScope::set_identity(long id_)
{
	LFPoint& p = roll(id_)._w;
	LFPoint old_p = p;
	p._id._scale = _scale;
	p._id._roll = id_;
	if (old_p._id._scale != p._id._scale || old_p._id._roll != p._id._roll)
		p._id._child = -1;
	if (p._id._top_scale < p._id._scale)
	{
		p._id._top_scale = p._id._scale;
	}

	if (_scale == 0)
		return;

	if (!_s_store->set_child_index(p))
	{
		_p_store->report("sanity_child_" + _prefix, _model->_name, roll(id_), _model->_report_scope_sanity);
	}

}
//======================================================================
void LFScope::update_identities(long first_)
{
	long sz = (long)size();
	for (long i = first_; i < sz; ++i)
		set_identity(i);
}
//======================================================================
void LFScope::update_roll(size_t ind_, bool just_pushed_)
{
	Papa& self = *this;
	if (ind_ < 1)
	{
		self[ind_].reset_w();
		self[ind_].reset_exp();
	}
	else
	{
		self[ind_].set_w(self[ind_ - 1], *_model);
	}

	if (self[ind_]._w_changed)
	{
		// tip
		if (self[ind_]._w._cluster_v > _model->_crl_tip_min_ticks)
		{
			double minv = std::max(self[ind_]._w._cluster_v*_model->_crl_tip, _model->_crl_tip_min_ticks);
			_p_store->find_tip(self[ind_]._tip, self[ind_]._w, minv);
		}
	}
	set_identity((long)ind_);
	if(just_pushed_)
		_s_store->set_children_top_scale(self[ind_].id());
	check_set_continuous((long)ind_);

	self[ind_]._w_changed = false;

}
//======================================================================
void LFScope::update_avg()
{
	Papa& self = *this;

	if (_avg_cnt == 0)
	{
		if (size() < _model->_crl_bias_avg_size + 2)
			return;
		_avg_cnt = (long)_p_store->_model->_crl_bias_avg_size;
		_avg_l = 0;
		_avg_v = 0;
		size_t first_ind = size() - _model->_crl_bias_avg_size - 1;
		for (size_t i = 0; i < _model->_crl_bias_avg_size; ++i)
		{
			_avg_l += self[first_ind + i]._w_l;
			_avg_v += self[first_ind + i]._w_v;
		}
		_avg_l /= _avg_cnt;
		_avg_v /= _avg_cnt;
	}
	else
	{
		// add self[prev] to moving averages
		const LFBox& b = self[size() - 2];
		_avg_l = (_avg_l*(_avg_cnt - 1) + b._w_l) / _avg_cnt;
		_avg_v = (_avg_v*(_avg_cnt - 1) + b._w_v) / _avg_cnt;
	}

}
//======================================================================
std::string LFScope::min_as_string() const
{
	std::ostringstream os;
	os << std::setprecision(1) << std::fixed
		<< "[" << _min_l << "_mlv_" << _min_v << "][" << _avg_l << "_alv_" << _avg_v << "]";
	return os.str();
}
//======================================================================
void LFScope::calc_bias(size_t ind_)
{
	Papa& self = *this;
	self[ind_]._bias = 0;
	self[ind_]._pias = 0;
	self[ind_]._d_pias = 0;
	self[ind_].reset_exp();

	if (ind_ < 2)
		return;
	double bf0 = 2;
	double bf1 = 3;
	double bf2 = 1;

	if (_model->_crl_bias_use_density)
	{
		double tot = self[ind_]._w._cluster_density + self[ind_ - 1]._w._cluster_density;
		self[ind_]._bias = tot <= 0 ? 0 : self[ind_]._side->sign()*self[ind_]._w._cluster_density / tot;
	}
	else
	{
		double tot = self[ind_]._w._cluster_vol + self[ind_ - 1]._w._cluster_vol;
		self[ind_]._bias = tot <= 0 ? 0 : self[ind_]._side->sign()*self[ind_]._w._cluster_vol / tot;
	}

	self[ind_]._pias = bf0*self[ind_]._w.charge() + bf1*self[ind_ - 1]._w.charge() + bf2*self[ind_ - 2]._w.charge();

	self[ind_]._d_pias = self[ind_]._pias - self[ind_ - 1]._pias;

	//pias peak
	self[ind_].set_exp_contra_p(calc_exp_contra_p(ind_, self[ind_]._pias));

	if (self[ind_ - 1].exp_valid())
	{
		self[ind_]._e_in_v = self[ind_ - 1].in_exp_v(self[ind_]._w._dp);
		self[ind_]._e_w = self[ind_]._e_v + self[ind_]._e_in_v;
	}
}

//======================================================================
double LFScope::calc_exp_contra_p(size_t ind_, double exp_contra_pias_)
{
	Papa& self = *this;
	if (ind_ < 2)
		return 0;
	double bf0 = 2;
	double bf1 = 3;
	double bf2 = 1;

	double indep_pias = bf1*self[ind_]._w.charge() + bf2*self[ind_ - 1]._w.charge();

	double exp_sax_p = self[ind_]._side->contra_s()->sign()*(exp_contra_pias_ - indep_pias) / bf0;
	return self[ind_]._side->p_from_v(self[ind_]._max_dp->_p, exp_sax_p, _p_store->_model->_tick_size);

}
//======================================================================
void LFScope::init_merge(LFBox& to_, long first_, long last_) const
{
	long ssz = (long)size();

	long l = last_ < 0 ? ssz : last_;
	if (l > ssz)
		l = ssz;
	if (l < first_)
	{
		to_.reset();
		return;
	}
	to_.reset_from(roll(first_));
	for (long i = first_ + 1; i < l; ++i)
		to_.merge(roll(i));
}
//======================================================================
long LFScope::find_lower_index(const LFPoint& p_) const
{
	LFBoxVec::const_iterator b = std::lower_bound(begin(), end(), p_, LF::less_point_st);

	if (b == end())
		return -1;

	return (long)std::distance(begin(), b);
}
//=================================================================================
//=================================================================================
LFScopeStore::LFScopeStore(LFPointStore* p_store_)
	:Papa(LFScopeStore::NUMBER_OF_SCOPES, LFScope()),
	_p_store(p_store_),
	_model(p_store_->_model)
{
	Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
	{
		std::ostringstream prefix;
		prefix << "g" << i;
		self[i].init(this, _p_store, prefix.str(), i);
	}

}
//======================================================================
void LFScopeStore::reset()
{
	Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
	{
		self[i].reset();
	}
}
//======================================================================
size_t LFScopeStore::find_scope(const LFCluster& c_, size_t& pin_, size_t& bin_) const
{
	const Papa& self = *this;
	size_t sz = size();
	pin_ = LFBoxTrend::TREND_SIZE;
	bin_ = LFBoxTrend::TREND_SIZE;
	size_t h_ind = sz;
	size_t h_pin = LFBoxTrend::TREND_SIZE;
	size_t h_bin = LFBoxTrend::TREND_SIZE;
	for (size_t i = sz; i > 0 && h_ind == sz; --i)
	{
		const LFScope& scp = self[i - 1];
		scp.find_indices(c_, h_pin, h_bin);
		if (h_pin < scp.size() && h_bin < scp.size())
			h_ind = i - 1;
	}

	if (h_ind == sz)
		return sz;

	for (size_t i = 0; i < h_ind; ++i)
	{
		const LFScope& scp = self[i];

		scp.find_indices(c_, pin_, bin_);

		if (pin_ < scp.size() && bin_ < scp.size() && bin_ == pin_ + 1)
		{
			return i;
		}
	}
	pin_ = h_pin;
	bin_ = h_bin;

	return h_ind;
}

//======================================================================
size_t LFScopeStore::find_bounce_scope(const LFCluster& c_, bool exact_pin_) const
{
	const Papa& self = *this;
	size_t sz = size();

	for (size_t i = sz; i > 0; --i)
	{
		size_t s_ind = i - 1;
		const LFScope& scp = self[s_ind];
		if (exact_pin_)
		{
			size_t pin;
			size_t bin;
			scp.find_indices(c_, pin, bin);

			if (pin < scp.size() && pin >= scp.ended_ind())
			{
				return s_ind;
			}
		}
		else
		{
			size_t pin = scp.peak_ended_ind(c_._side);
			if (pin < scp.size() && scp[pin].valid() && c_._dp->_t <= scp[pin]._max_dp->_t)
				return s_ind;
		}
	}
	return sz;
}
//======================================================================
void LFScopeStore::clear_just_flags()
{
	Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
		self[i].clear_just_flags();
}
//======================================================================
void LFScopeStore::init_merge(LFBox& to_, long scale_id_, long first_, long last_) const
{
	const Papa& self = *this;
	const LFScope& scope = self[(size_t)scale_id_];
	size_t ssz = scope.size();
	size_t f = (size_t)first_;
	size_t l = last_ < 0 ? ssz : (size_t)last_;
	if (l > ssz)
		l = ssz;
	if (l < f)
	{
		to_.reset();
		return;
	}
	to_.reset_from(scope[f]);
	for (size_t i = f + 1; i < l; ++i)
		to_.merge(scope[i]);

}
//======================================================================
bool LFScopeStore::child_confirm_bounce(const LFBox& b_) const
{
	const LFPoint& p = b_._w;
	if (!child_valid(p))
		return false;

	if (!valid_id(p._id._scale - 1, p._id._child + 3))
		return false;

	const LFBox& ch_roll = roll(p._id._scale - 1, p._id._child + 1);

	long ch_to = scope_size(p._id._scale - 1);
	if (valid_id(p._id._scale, p._id._roll + 1))
	{
		const LFPoint& next_roll = roll(p._id._scale, p._id._roll + 1)._w;
		if (child_valid(next_roll))
			ch_to = (size_t)next_roll._id._child + 1;
	}

	for (long i = p._id._child + 3; i < ch_to; ++i)
	{
		const LFBox& i_roll = roll(p._id._scale - 1, i);
		if (i_roll._side == ch_roll._side && ch_roll.value(i_roll._w._dp) <= 0)
			return true;
	}
	return false;
}
//======================================================================
bool LFScopeStore::child_confirm_exp(const LFBox& b_) const
{
	const LFPoint& p = b_._w;
	if (!child_valid(p))
		return false;

	if (!valid_id(p._id._scale - 1, p._id._child + 1))
		return false;

	long ch_to = scope_size(p._id._scale - 1);
	if (valid_id(p._id._scale, p._id._roll + 1))
	{
		const LFPoint& next_roll = roll(p._id._scale, p._id._roll + 1)._w;
		if (child_valid(next_roll))
			ch_to = (size_t)next_roll._id._child + 1;
	}

	for (long i = p._id._child + 1; i < ch_to; ++i)
	{
		const LFBox& i_roll = roll(p._id._scale - 1, i);
		if (i_roll._side != p._side)
		{
			const LFBox& a_roll = roll(p._id._scale - 1, i - 1);
			if (!a_roll._w.includes(b_._w) && a_roll.in_exp_pct(i_roll._w._dp) < -_model->_crl_in_exp_pct)
				return true;
		}
	}
	return false;
}
//======================================================================
bool LFScopeStore::child_valid(const LFPoint& p_) const
{
	if (!p_.valid() || p_._id._scale  == 0 || p_._id._child < 0)
		return false;

	if (!valid_id(p_._id._scale - 1, p_._id._child))
		return false;

	const LFPoint& ch_p = roll(p_._id._scale - 1, p_._id._child)._w;

	return ch_p._side == p_._side  && ch_p._dp->equal(*p_._dp);
}
//=================================================================================
void LFScopeStore::set_top_scale(long scale_id_, long roll_id_, long top_scale_)
{

	if (scale_id_ < 0 || roll_id_ < 0)
		return;

	LFPoint& z_sp = roll(scale_id_, roll_id_)._w;
	z_sp._id._top_scale = top_scale_;
	set_top_scale(scale_id_ - 1, z_sp._id._child, top_scale_);
}
//=================================================================================
bool LFScopeStore::set_child_index(LFPoint& p_)
{
	if (!p_.valid())
		return true;
	if (child_valid(p_))
		return true;

	p_._id._child = -1;

	if (p_._id._scale == 0)
		return true;

	Papa& self = *this;
	LFScope& z_child = self[(size_t)p_._id._scale - 1];
	
	//AK TODO -- make more effective search
	for (size_t i = z_child.size(); i > 0; --i)
	{
		long ci = (long)i - 1;
		LFBox& c_rl = z_child.roll(ci);
		if (c_rl._w._dp->older(*p_._dp))
		{
			return false;
		}
		if (c_rl._w._side == p_._side  && c_rl._w._dp->equal(*p_._dp))
		{
			p_._id._child = ci;
			set_top_scale(p_._id._scale - 1, p_._id._child, p_._id._top_scale);
			
			return true;
		}
	}
	return false;

}

//======================================================================
long LFScopeStore::find_parent_index(const LFPoint& p_) const
{
	if (!valid_scale(p_._id._scale + 1))
		return -1;

	const LFScope& parent = scope(p_._id._scale + 1);

	long p_id = parent.find_lower_index(p_);

	if (p_id < 0 || !parent.roll(p_id)._w._dp->equal(*p_._dp))
		return -1;

	if (parent.roll(p_id)._w._side == p_._side)
		return p_id;

	if (!parent.valid_id(p_id + 1) || !parent.roll(p_id + 1)._w._dp->equal(*p_._dp))
		return -1;
	return p_id + 1;
}
//======================================================================
LF::Id LFScopeStore::first_child_id(const LF::Id& id_) const
{
	if (!id_.valid() || id_._scale == 0)
		return LF::Id();
	
	if (id_._roll > 0)
		return roll(id_._scale - 1, roll(id_._scale, id_._roll - 1).id()._child + 1).id();
	return roll(id_._scale - 1, id_._child).id();
}
//======================================================================
LF::Id LFScopeStore::bounce_id(const LF::Id& id_) const
{
	if (valid_id(id_._scale, id_._roll + 1))
		return roll(id_._scale, id_._roll + 1).id();
	if (id_._scale > 0)
		return bounce_id(roll(id_._scale - 1, id_._child).id());
	return LF::Id();

}
//======================================================================
void LFScopeStore::set_children_top_scale(const LF::Id& id_)
{
	if (id_._scale <= 0)
		return;

	LF::IdVec b_vec;
	fill_blossom_ids(id_, id_._scale - 1, b_vec);
	LFDataPointPtr max_dp;
	for (LF::IdVec::reverse_iterator i = b_vec.rbegin(); i != b_vec.rend(); ++i)
	{
		LFBox& i_roll = roll((*i));
		if (max_dp.get() == 0 || i_roll.value(max_dp) >= 0)
		{
			max_dp = i_roll._w._dp;
			set_top_scale((*i)._scale, (*i)._roll, id_._top_scale);
		}
	}

}
//======================================================================
void LFScopeStore::fill_blossom_ids(const LF::Id& id_, long top_scale_, LF::IdVec& b_ids_) const
{
	if (id_._scale <= 0)
		return;
	const LFSide* side = roll(id_)._side;
	long prev_c = -1;
	if(id_._roll > 0)
		prev_c = roll(id_._scale, id_._roll - 1).id()._child;

	for (long c_i = id_._child; c_i > prev_c; --c_i)
	{
		const LFBox& c_rl = roll(id_._scale - 1, c_i);
		if (c_rl._side == side)
		{
			LF::Id c_id = c_rl.id();
			if (c_i != id_._child && c_id._top_scale >= top_scale_)
				b_ids_.push_back(c_id);
			fill_blossom_ids(c_id, top_scale_, b_ids_);
		}
	}
}

//======================================================================
void LFScopeStore::fill_box_trends(LFBoxTrends& trs_) const
{
	const Papa& self = *this;
	for (size_t i = 0; i < size(); ++i)
	{
		self[i].fill_box_trend(trs_[i]);
	}
}
//======================================================================
void LFScopeStore::fill_lcy_trends(LFBoxVec& bvec_, LFBoxTrends& trs_) const
{
	trs_.reset();
	bvec_.clear();
	const Papa& self = *this;
	std::list<const LFBox*> rls;
	for (size_t i = 1; i < size(); ++i)
	{
		const LFScope& scp = self[i];
//		if (!scp.empty() && scp.back().continuous_not_straight())
		if (!scp.empty() && scp.back().continuous())
			{
			rls.push_back(&scp.back());
		}
	}
	std::list<const LFBox*>::iterator ii = rls.begin();
	while(ii != rls.end())
	{
		std::list<const LFBox*>::iterator inext = ii;
		inext++;
		if (inext == rls.end())
			break;
		const LFBox& rl = *(*ii);
		const LFBox& rlnext = *(*inext);

		if (rl._w._dp->equal(*rlnext._w._dp))
		{
			if (rlnext._w._cluster_v > rl._w._cluster_v)
			{
				ii = rls.erase(ii);
			}
			else
			{
				rls.erase(inext);
			}
		}
		else
		{
			++ii;
		}
	}

	size_t ind = 0;
	for (std::list<const LFBox*>::reverse_iterator ii = rls.rbegin(); ii != rls.rend(); ++ii)
	{
		const LFBox& rl = *(*ii);
		bvec_.push_back(rl);
		if (_model->_out_file_boxes)
		{
			std::ostringstream os;
			os << "lcy[" << ind << "]";
			LF::model_send(rl, os.str(), _p_store->dp()->_t, _model->_out_file_boxes, _model->_name + "_lcy");
		}
		LF::Id bnc = bounce_id(rl.id());
		if (!bnc.valid())
		{
			trs_[ind][0] = rl;
		}
		else
		{
			trs_[ind][1] = rl;
			trs_[ind][0] = roll(bnc);
			trs_[ind][0]._comment = "bnc";
		}
		ind++;
	}

}
//======================================================================
