/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_POINT_H
#define LF_POINT_H

#include "znet/ZNet.h"
#include "leaf/LFMarketModel.h"

//======================================================================
struct LEAF_Export LFDataPoint : public Z::Point
{
	LFDataPoint(const std::string& key_ = "");
	LFDataPoint(const ACE_Time_Value& t_, double p_ = 0, double u_vol_ = 0, double u_tran_ = 0, double d_vol_ = 0, double d_tran_ = 0);
	virtual ~LFDataPoint();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) { _key = k_; }

	virtual void reset(const ACE_Time_Value& t_ = ACE_Time_Value::zero, double l_ = 0,
		double p_ = 0, double p_activity_ = 0, double v_activity_ = 0, double corr_ = 0,
		double u_vol_ = 0, double u_tran_ = 0, double d_vol_ = 0, double d_tran_ = 0);

	bool valid() const { return _t != ACE_Time_Value::zero; }
	bool equal(const LFDataPoint& s_) const { return _t == s_._t; }

	bool older(const ACE_Time_Value& t_) const
	{
		return valid() && _t < t_;
	}

	bool older(const LFDataPoint& s_) const
	{
		return valid() && s_.valid() && _t < s_._t;
	}


	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFDataPoint& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFDataPoint& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFDataPoint& d_);
	void body_to_stream(std::ostream& os) const;
	void summary_to_stream(std::ostream& os) const;

	std::string _key;
	ACE_Time_Value _s_t;
	std::string _s_t_str;
	double	_l;
	double	_p;
	double	_p_activity;
	double	_v_activity;
	double	_corr;
	double	_u_vol;
	double	_u_tran;
	double	_d_vol;
	double	_d_tran;
	double  _obv;
	double  _vma;
	double  _mfi;

	std::string	_comment;
	static const char* header();
	static const char* summary_header();

};
typedef Z::smart_ptr<LFDataPoint> LFDataPointPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFDataPoint)

//======================================================================
namespace LF
{
	inline const ACE_Time_Value& get_t(const LFDataPointPtr& dp_)
	{
		if (dp_.get() != 0)
			return dp_->_t;
		return ACE_Time_Value::zero;
	}

	inline double same_point(const LFDataPointPtr& new_dp_, const LFDataPointPtr& old_dp_)
	{
		return new_dp_.get() != 0
			&& (old_dp_.get() == 0 || new_dp_->equal(*old_dp_));
	}

	inline double charge_i(double p_, double point_p_, double tick_size_)
	{
		return TCComparePrice::diff(point_p_, p_) / tick_size_;
	}
	inline double charge_i(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		return charge_i(dp_->_p, point_dp_->_p, tick_size_);
	}
	inline double charge(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		if (dp_.get() == 0 || point_dp_.get() == 0 || tick_size_ == 0)
			return TCCompare::NaN;
		return charge_i(dp_, point_dp_, tick_size_);
	}

	inline double value_i(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		return side_->v(dp_->_p, point_dp_->_p, tick_size_);
	}
	inline double value(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		if (side_ == 0 || dp_.get() == 0 || point_dp_.get() == 0 || tick_size_ == 0)
			return TCCompare::NaN;
		return value_i(side_, dp_, point_dp_, tick_size_);
	}

	inline double length_i(const ACE_Time_Value& t_, const ACE_Time_Value& point_t_)
	{
		if (point_t_ <= t_)
			return length_from_t(t_ - point_t_);
		return -length_from_t(point_t_ - t_);
	}
	inline double length_i(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		return length_i(dp_->_t, point_dp_->_t);
	}
	inline double length(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_.get() == 0 || point_dp_.get() == 0)
			return TCCompare::NaN;
		return length_i(dp_, point_dp_);
	}

	inline double slope_i(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		if (dp_->equal(*point_dp_))
			return 0;
		return value_i(side_, dp_, point_dp_, tick_size_) / -length_i(dp_, point_dp_);
	}
	inline double slope(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_, double tick_size_)
	{
		if (side_ == 0 || dp_.get() == 0 || point_dp_.get() == 0 || tick_size_ == 0)
			return TCCompare::NaN;
		return slope_i(side_, dp_, point_dp_, tick_size_);
	}

	inline double volume_i(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_->equal(*point_dp_))
			return 0;
		if (side_->sign() > 0)
			return point_dp_->_u_vol - dp_->_u_vol;
		else
			return point_dp_->_d_vol - dp_->_d_vol;
	}
	inline double volume(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (side_ == 0 || dp_.get() == 0 || point_dp_.get() == 0)
			return 0;
		return volume_i(side_, dp_, point_dp_);
	}

	inline double t_volume_i(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_->equal(*point_dp_))
			return 0;
		return point_dp_->_u_vol - dp_->_u_vol + point_dp_->_d_vol - dp_->_d_vol;
	}
	inline double t_volume(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_.get() == 0 || point_dp_.get() == 0)
			return 0;
		return t_volume_i(dp_, point_dp_);
	}
	inline double tran_i(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_->equal(*point_dp_))
			return 0;
		if (side_->sign() > 0)
			return point_dp_->_u_tran - dp_->_u_tran;
		else
			return point_dp_->_d_tran - dp_->_d_tran;
	}
	inline double tran(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (side_ == 0 || dp_.get() == 0 || point_dp_.get() == 0)
			return 0;
		return tran_i(side_, dp_, point_dp_);
	}

	inline double t_tran_i(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_->equal(*point_dp_))
			return 0;
		return point_dp_->_u_tran - dp_->_u_tran + point_dp_->_d_tran - dp_->_d_tran;
	}
	inline double t_tran(const LFDataPointPtr& dp_, const LFDataPointPtr& point_dp_)
	{
		if (dp_.get() == 0 || point_dp_.get() == 0)
			return 0;
		return t_tran_i(dp_, point_dp_);
	}
};

//======================================================================
struct LEAF_Export LFPoint : public Z::Point
{
	LFPoint(const std::string& key_ = "");
	virtual ~LFPoint();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) {_key = k_;}

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFPoint& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFPoint& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFPoint& d_);
	virtual void body_to_stream(std::ostream& os) const;
	virtual void summary_to_stream(std::ostream& os) const;

	virtual void reset(const LFSide* side_ = 0, const LFDataPointPtr& dp_ = LFDataPointPtr());
	void reset_point(const LFDataPointPtr& dp_);

	void reset_identity(long scale_id_ = 0, long roll_id_ = 0, long child_id_ = 0, long top_scale_ = 0);
	void set_identity(const LFPoint& from_);

	double value(double p_, double tick_size_) const;
	double value(const LFDataPointPtr& dp_, double tick_size_) const {
		return dp_.get() == 0 ? 0 : value(dp_->_p, tick_size_);
	}
	double price_from_value(double v_, double tick_size_) const;
	double price_from_value(double s_p_, double v_, double tick_size_) const;
	double length(const ACE_Time_Value& t_) const;
	ACE_Time_Value t_from_length(double l_) const;

	bool cross(double p_, double last_p_) const;

	// 
	bool in_approach(double p_, double level_p_, double approach_factor_, double tick_size_) const;

	double psign() const
	{
		if(_side == 0)
			return 0;
		return _side->sign();
	}
	virtual bool valid() const { return _side != 0 && _dp.get() != 0; }
	bool equal(const LFPoint& s_) const { return valid() && s_.valid() && _dp->equal(*s_._dp); }
	bool operator==(const LFPoint& s_) const { return level_equal(s_); }

	bool older(const ACE_Time_Value& t_) const
	{ return valid() && _dp->older(t_);	}

	bool older(const LFPoint& s_) const
	{
		return valid() && s_.valid() && _dp->older(*s_._dp);
	}

	virtual bool level_equal(const LFPoint& s_) const 
	{ 
		if(!valid()) return !s_.valid();
		return s_.valid()
			&& _dp->equal(*s_._dp)
			&& _strength == s_._strength
			&& _comment == s_._comment
			;
	}

	std::string _key;
	LF::Id	_id;

	const LFSide*	_side;
	LFDataPointPtr	_dp;
	long			_strength;

	std::string		_comment;

	static const char* header();
	static const char* summary_header();
};
typedef Z::smart_ptr<LFPoint> LFPointPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFPoint)

namespace LF
{
	inline
	bool less_point_st(const LFPoint& s1_, const LFPoint& s2_)
	{
		return s1_._dp->_s_t < s2_._dp->_s_t;
	}
};

//======================================================================
struct LEAF_Export LFPointPair : public LF::Pair<LFPoint>
{
	typedef LF::Pair<LFPoint> Papa;
	LFPointPair() : Papa(LFPoint(), LFPoint()) {}
	void reset() { Papa::reset(LFPoint(), LFPoint()); }
};

//======================================================================
#endif /*LF_POINT_H*/
