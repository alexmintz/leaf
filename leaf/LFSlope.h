/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_SLOPE_H
#define LF_SLOPE_H

#include "znet/ZNet.h"
#include "leaf/LFPoint.h"

//======================================================================
struct LEAF_Export LFSlope : public LFPoint
{
	LFSlope(const std::string& key_ = "");
	virtual ~LFSlope();

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFSlope& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFSlope& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFSlope& d_);

	virtual void reset(const LFSide* side_ = 0, const ACE_Time_Value& t_ = ACE_Time_Value::zero, double p_ = 0, double eng_ = 0,
				double e_l_= 0., double e_v_ = 0., double tick_size_ = 0.);

	void reset(const LFSlope& from_);

	virtual void update(const ACE_Time_Value& t_, double p_, double eng_, double tick_size_, bool reset_max_, bool reset_max_eng_ = false);
	virtual void adjust_e_l(const ACE_Time_Value& to_t_, double tick_size_);
	virtual void reset_s_eng(const ACE_Time_Value& t_, double eng_);
	virtual void adjust_max(const LFSlope& from_);

	virtual bool current() const { return valid() && _c_l < _e_l; }
	virtual bool working() const { return current() && _v < _e_v; }
	virtual bool exceeded() const { return valid() && _exceeded; }

	double v_match(double v_) const 
	{
		if(v_ <= 0.)
			return 0.;
		if(_e_v <= 0.)
			return 1.;
		return v_/_e_v;
	}
	double v_match() const 
	{
		return v_match(_c_v);
	}

	double l_match(double l_) const 
	{
		if(_e_l <= 0.)
			return 1.;
		return l_/_e_l;
	}
	double l_match() const 
	{
		return l_match(_c_l);
	}
	// [1 -> 0]
	double l_factor(double l_) const 
	{
		double m = l_match(l_);
		return m >= 1.? 0 : (1. - m) ;
	}
	// [1 -> 0]
	double l_factor() const 
	{
		double m = l_match();
		return m >= 1.? 0 : (1. - m) ;
	}
	//imbalance [0-1] 0 -> best balanced
	double l_imbalance() const 
	{
		double m = l_match();
		return m > 1.? (1. - 1./m) : (1. - m);
	}
	double v_imbalance() const 
	{
		double m = v_match();
		return m > 1.? (1. - 1./m) : (1. - m);
	}

	double imbalance() const { return (l_imbalance() + v_imbalance())/2.; }

	double ce_v() const 
	{
		return _e_v*l_match();
	}

	double eng_value(double eng_) const;
	double eng_from_value(double eng_v_) const;

	double	_s_eng;

	double	_p;
	double	_l;
	double	_v;
	double	_s;
	// expected lvs
	double	_e_p;
	double	_e_l; 
	double	_e_v;
	double	_e_s;
	double	_ce_v;
	double	_ce_p;
// unconfirmed
	double	_u_p;
	double	_u_eng;
	double	_uv_eng;

	double _e_den_v;
	double _S_p;
	double _S_eng;
	double _S_p_logr;
	double _S_eng_logr;

	// current lvs
	double	_c_p;
	double	_c_l; 
	double	_c_v;
	double	_c_s;

	double	_eng;
	double	_eng_v;
	double	_den_v;
	double	_c_eng;
	double	_c_eng_v;

	bool	_exceeded;

	static const char* header();
};
typedef Z::smart_ptr<LFSlope> LFSlopePtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFSlope)
//typedef LF::Pair<LFSlope> LFSlopePair;
//======================================================================
struct LEAF_Export LFSlopePair : public LF::Pair<LFSlope>
{
	typedef LF::Pair<LFSlope> Papa;
	LFSlopePair() : Papa(LFSlope(), LFSlope()) {}


	void reset() { Papa::reset(LFSlope(), LFSlope()); }
};

typedef std::list<LFSlope> LFSlopeList;
typedef Z::smart_ptr<LFSlopeList> LFSlopeListPtr;



//======================================================================
struct LEAF_Export LFPeak : public LFSlope
{
	LFPeak(const std::string& key_ = "");
	explicit LFPeak(const LFSlope& s_);
	virtual ~LFPeak();

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFPeak& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFPeak& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFPeak& d_);

	virtual void reset(const LFSide* side_ = 0, const ACE_Time_Value& t_ = ACE_Time_Value::zero, double p_ = 0, double eng_ = 0, 
				double e_l_= 0., double e_v_ = 0., double tick_size_ = 0.);

	virtual void reset(const LFPeak& from_);
	virtual void reset(const LFSlope& from_);

	void update_peak(const LFPeak& prev_, bool confirmed_, double tick_size_, double min_first_peak_ = 0);

	void clear_just_flags();

	double	_pp_v;
	double	_pp_eng_v;
	double	_s_p_eng;

	double	_vp_v;
	double	_vp_eng_v;
	double	_vp_den_v;
	double	_vp_l;
	double	_pv_v;
	double	_pv_eng_v;
	double	_pv_den_v;
	double	_pv_l;

	bool	_confirmed;
	bool	_just_confirmed;
	bool    _just_unconfirmed;
	bool	_just_born;
	bool    _formed;
	bool    _just_formed;

	static const char* header();
};
typedef Z::smart_ptr<LFPeak> LFPeakPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFPeak)

//======================================================================
#endif /*LF_SLOPE_H*/
