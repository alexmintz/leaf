/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_CLUSTER_H
#define LF_CLUSTER_H

#include "znet/ZNet.h"
#include "leaf/LFPoint.h"

//======================================================================
typedef std::pair<double, double> LFContinuity;
//======================================================================
struct LEAF_Export LFCluster : public LFPoint
{
	LFCluster(const std::string& key_ = "");
	virtual ~LFCluster();

	bool operator==(const LFCluster& s_) const { return level_equal(s_); }

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFCluster& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFCluster& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFCluster& d_);
	virtual void body_to_stream(std::ostream& os) const;
	virtual void summary_to_stream(std::ostream& os) const;

	virtual void reset(const LFSide* side_ = 0, const LFDataPointPtr& dp_ = LFDataPointPtr());
	void reset(const LFPoint& point_);
	void reset_cluster(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& c_dp_, double tick_size_);

	bool cluster_valid() const { return valid() && _cluster_dp.get() != 0; }

	void set_cluster(const LFDataPointPtr& c_dp_, double tick_size_);
	void set_cluster(const LFPoint& c_point_, double tick_size_);

	void clear_cluster();
	void calc_cluster();

	bool level_equal(const LFCluster& s_) const
	{ 
		if(!valid()) return !s_.valid();
		return LFPoint::level_equal(s_)
			&& _cluster_dp->_t == s_._cluster_dp->_t
			&& _pattern == s_._pattern
			&& _te_pattern == s_._te_pattern
			;
	}

	void fill_cluster_point(LFPoint& p_) const 
	{ 
		if (!cluster_valid())
			p_.reset();
		else
			p_.reset(_side->contra_s(), _cluster_dp); 
	}
	bool t_in(const LFDataPointPtr& dp_) const
	{
		return cluster_valid() && dp_.get() != 0 && dp_->valid()
			&& !dp_->older(*_cluster_dp) && !_dp->older(*dp_);
	}

	double charge() const {
		return !valid() ? 0 : _side->sign() * _cluster_v;
	}

	bool includes(const LFCluster& c_) const
	{
		return cluster_valid() && c_.cluster_valid()
			&&	_dp->_t >= c_._dp->_t 
			&&	_cluster_dp->_t <= c_._cluster_dp->_t;
	}

	double continuity(const LFCluster& bounce_, const LFMarketModel& _model) const;

	double	_tick_size;
	LFDataPointPtr	_cluster_dp;
	double	_cluster_l;
	double	_cluster_v;
	double	_cluster_s;
	double	_cluster_vol;
	double	_cluster_tran;
	double	_cluster_density;
	double	_cluster_frequency;

	LF::Pattern		_pattern;
	LF::TEPattern	_te_pattern;

	static const char* header();
	static const char* summary_header();
};
typedef Z::smart_ptr<LFCluster> LFClusterPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFCluster)

typedef std::vector<LFCluster> LFClusterVec;

//======================================================================
struct LEAF_Export LFClusterPair : public LF::Pair<LFCluster>
{
	typedef LF::Pair<LFCluster> Papa;
	LFClusterPair() : Papa(LFCluster(), LFCluster()) {}


	void reset() { Papa::reset(LFCluster(), LFCluster()); }
	void reset_identity() { _v[0].reset_identity(); _v[1].reset_identity(); }
};

//======================================================================
struct LEAF_Export LFBounce : public LFCluster
{
	LFBounce();
	virtual ~LFBounce();

	virtual void reset(const LFSide* side_ = 0, const LFDataPointPtr& dp_ = LFDataPointPtr());

	void set_bounce(const LFCluster& peak_);

	bool good(double v_pct_, double l_pct_, double min_l_) const
	{
		return _v_pct > v_pct_ && _l_pct > l_pct_ && _cluster_l > min_l_;
	}


	double _v_pct;
	double _l_pct;
	double _vol_pct;

};

//======================================================================
#endif /*LF_CLUSTER_H*/
