/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_BOX_H
#define LF_BOX_H

#include "znet/ZNet.h"
#include "leaf/LFMarketModel.h"
#include "leaf/LFCluster.h"
#include <deque>

//======================================================================
struct LEAF_Export LFBox : public Z::Point
{
	enum { UNKNOWN_BIAS = -1000000 };
	static double BIAS_MIN_VALUE;

	LFBox(const std::string& key_ = "");
	virtual ~LFBox();

	operator const LFPoint&() const { return _w; }

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) { _key = k_; }

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFBox& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFBox& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFBox& d_);
	void summary_to_stream(std::ostream& os) const;
	void body_to_stream(std::ostream& os) const;

	void reset(const LFSide* side_ = 0, const LFDataPointPtr& dp_s_ = LFDataPointPtr(), double tick_size_ = 0, const LFCluster& prev_w_ = LFCluster());
	void reset_from(const LFBox& from_);
	void reset_corr(double corr_) {
		_max_corr = corr_;
		_min_corr = corr_;
	}
	void update_current(const LFDataPointPtr& dp_);

	void reset_side(const LFSide* side_, const LFCluster& prev_w_ = LFCluster());

	bool pias_allied() const
	{
		return valid() && _side->zsign(_pias) > 0;
	}

	bool pias_contra() const
	{
		return valid() && _side->zsign(_pias) < 0;
	}



	virtual bool valid() const { return _side != 0 && _s_dp.get() != 0; }
	bool equal(const LFBox& s_) const { return valid() && s_.valid() && _s_dp->equal(*s_._s_dp); }
	bool operator==(const LFBox& s_) const { return equal(s_); }
	bool older(const ACE_Time_Value& t_) const
	{
		return valid() && _s_dp->older(t_);
	}

	bool older(const LFBox& s_) const
	{
		return valid() && s_.valid() && _s_dp->older(*s_._s_dp);
	}

	bool t_equal(const LFBox& s_) const 
	{ 
		if(!valid()) return !s_.valid();
		return s_.valid()
			&& _side == s_._side
			&& _s_dp->equal(*s_._s_dp)
			&& _max_dp->equal(*s_._max_dp)
			&& _min_dp->equal(*s_._min_dp)
			&& _end_t == s_._end_t
			&& _turned_t == s_._turned_t
			&& _pinned == s_._pinned;
	}

	const LF::Id& id() const { return _w._id; }

	bool at_max() const { return valid() && _max_dp->equal(*_c_dp); }

	bool continuous() const { return valid() && _w._pattern.test(LF::pb_COUNTINUOUS) && !_w._pattern.test(LF::pb_INTERRUPTED); }
	bool continuous_not_straight() const { return continuous() && !_w._pattern.test(LF::pb_STRAIGHT); }
	void merge(const LFBox& from_, bool keep_bias_ = false);
	void merge_same(const LFBox& from_);
	void split_min(LFBox& m_, double tick_size_);
	void cut_max();

	double p_charge() const { return !valid() ? 0 : LF::charge_i(_s_dp, _c_dp, _w._tick_size); }
	double max_p_charge() const { return !valid() ? 0 : LF::charge_i(_s_dp, _max_dp, _w._tick_size); }
	
	double length() const { return !valid() ? 0 : LF::length_i(_c_dp, _s_dp); }
	double p_slope() const { return !valid() ? 0 : LF::slope_i(_side, _w_dp, _c_dp, _w._tick_size); }

	bool ended() const { return valid() && 	_end_t != ACE_Time_Value::zero; }
	void set_ended() { _end_t = LF::get_t(_c_dp); }
	void reset_ended() { _end_t = ACE_Time_Value::zero; }

	bool turned() const { return valid() && _turned_t != ACE_Time_Value::zero; }
	void set_turned() { _turned_t = LF::get_t(_c_dp); }
	void unset_turned() { _turned_t = ACE_Time_Value::zero; }
	void set_w(const LFBox& prev_, const LFMarketModel& model_);
	void reset_w();
	void update_w();

	//
	double exp_contra_p() const { return _exp_contra_p; }
	void set_exp_contra_p(double exp_contra_p_) 
	{ 
		_e_v = TCCompare::NaN;
		_exp_contra_p = exp_contra_p_;
		if (!valid() || TCCompare::is_NaN(_exp_contra_p))
			return;
		_e_v = _w.value(_exp_contra_p, _w._tick_size);
	}

	double value(double p_) const { return _w.value(p_, _w._tick_size); }
	double in_exp_v(double p_) const { return _side->v(exp_contra_p(), p_, _w._tick_size); }
	double in_exp_pct(double p_) const 
	{ 
		double v = _w.value(p_, _w._tick_size);
		double zv = _w.value(exp_contra_p(), _w._tick_size);
		double max_v = std::max(::fabs(v), ::fabs(zv));
		return max_v == 0 ? 0 : (zv - v)*100. / max_v;
	}

	double value(const LFDataPointPtr& dp_) const { return dp_.get() == 0 ? TCCompare::NaN : value(dp_->_p); }
	double in_exp_v(const LFDataPointPtr& dp_) const { return  dp_.get() == 0 ? TCCompare::NaN : in_exp_v(dp_->_p); }
	double in_exp_pct(const LFDataPointPtr& dp_) const { return dp_.get() == 0 ? TCCompare::NaN : in_exp_pct(dp_->_p); }

	void reset_exp();
	bool exp_valid() const { return !TCCompare::is_NaN(_exp_contra_p); }

	bool w_declined(double pslope_decline_) const {	return _w._cluster_s > _w_s*pslope_decline_; }

	//tip------------
	double tip_l() const { return _tip.cluster_valid() ? _tip._cluster_l : 0; }
	double tip_s() const { return _tip.cluster_valid() ? _tip._cluster_s : 0; }
	double tip_v() const { return _tip.cluster_valid() ? _tip._cluster_v : 0; }

	double tip_vol() const { return _tip.cluster_valid() ? _tip._cluster_vol : 0; }
	double tip_trn() const { return _tip.cluster_valid() ? _tip._cluster_tran : 0; }
	double tip_den() const { return _tip.cluster_valid() ? _tip._cluster_density : 0; }
	double tip_frq() const { return _tip.cluster_valid() ? _tip._cluster_frequency : 0; }

	double w_vol() const { return _w.cluster_valid() ? _w._cluster_vol : 0; }
	double w_trn() const { return _w.cluster_valid() ? _w._cluster_tran : 0; }
	double w_den() const { return _w.cluster_valid() ? _w._cluster_density : 0; }
	double w_frq() const { return _w.cluster_valid() ? _w._cluster_frequency : 0; }
	//-----------------
	bool triggered() const { return _triggered_t != ACE_Time_Value::zero; }

	void set_trace(const ACE_Time_Value& trace_t_ = ACE_Time_Value::zero, const std::string& trace_key_ = std::string())
	{
		_trace_t = trace_t_;
		_trace_key = trace_key_;
	}
	bool is_trace() const {	return _trace_t != ACE_Time_Value::zero; }

	std::string		_key;
	const LFSide*	_side;

	LFDataPointPtr	_s_dp;
	LFDataPointPtr	_c_dp;
	LFDataPointPtr	_max_dp;
	LFDataPointPtr	_min_dp;
	LFDataPointPtr	_w_dp;

	double			_max_corr;
	double			_min_corr;

	ACE_Time_Value	_end_t;
	ACE_Time_Value	_turned_t;

	double			_w_l;
	double			_w_v;
	double			_w_dv;
	double			_e_w;
	double			_w_s;
	double			_e_v;
	double			_e_in_v;
	double			_exp_contra_p;
	LFCluster		_tip;
	bool			_pinned;
	bool			_w_changed;
	double			_bias;
	double			_s_bias;
	double			_pias;
	double			_d_pias;
	double			_s_pias;
	ACE_Time_Value	_triggered_t;
	std::string		_comment;
	LFCluster		_w;

	ACE_Time_Value	_trace_t;
	std::string		_trace_key;

	static const char* header();
	static const char* summary_header();

};
//======================================================================
typedef Z::smart_ptr<LFBox> LFBoxPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFBox)
typedef std::list<LFBox> LFBoxList;
typedef Z::smart_ptr<LFBoxList> LFBoxListPtr;
typedef std::vector<LFBox> LFBoxVec;

//======================================================================
struct LEAF_Export LFBoxPair : public LF::Pair<LFBox>
{
	typedef LF::Pair<LFBox> Papa;
	LFBoxPair() : Papa(LFBox(), LFBox()) {}


	void reset() { Papa::reset(LFBox(), LFBox()); }
	bool valid() const { return Papa::_v[0].valid() && Papa::_v[1].valid(); }
};

//======================================================================
#endif /*LF_BOX_H*/

