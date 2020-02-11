/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFPoint.h"
#include <cstdlib> 
#include <set>
#include <cfloat>

//======================================================================
//======================================================================
LFDataPoint::LFDataPoint(const std::string& key_)
	:_key(key_),
	_l(0),
	_p(0),
	_p_activity(0),
	_v_activity(0),
	_corr(0),
	_u_vol(0),
	_u_tran(0),
	_d_vol(0),
	_d_tran(0),
	_obv(0),
	_vma(0),
	_mfi(0)
{
}
LFDataPoint::LFDataPoint(const ACE_Time_Value& t_, double p_, double u_vol_, double u_tran_, double d_vol_, double d_tran_)
	: _l(0),
	_p(p_),
	_p_activity(0),
	_v_activity(0),
	_corr(0),
	_u_vol(u_vol_),
	_u_tran(u_tran_),
	_d_vol(d_vol_),
	_d_tran(d_tran_),
	_obv(0),
	_vma(0),
	_mfi(0)
{
	_s_t = t_;
	_s_t_str = TCTimestamp(_s_t).c_str();
	_t = t_;
	_run = BMModel::instance()->run_num();
}
//======================================================================
LFDataPoint::~LFDataPoint()
{
}

//======================================================================
const char* LFDataPoint::header()
{
	return ",S_KEY,S_T,L,P,P_ACTIVITY,V_ACTIVITY,CORR,U_VOL,D_VOL,U_TRAN,D_TRAN,OBV,VMA,MFI,COM";
}
//======================================================================
void LFDataPoint::body_to_stream(std::ostream& os) const
{
	os << "," << _key;
	os << "," << _s_t_str;
	os << "," << _l;
	os << "," << _p;
	os << "," << _p_activity;
	os << "," << _v_activity;
	os << "," << _corr;
	os << "," << _u_vol;
	os << "," << _d_vol;
	os << "," << _u_tran;
	os << "," << _d_tran;
	os << "," << _obv;
	os << "," << _vma;
	os << "," << _mfi;
	os << "," << _comment;

}
//======================================================================
const char* LFDataPoint::summary_header()
{
	return ",T,P";
}
//======================================================================
void LFDataPoint::summary_to_stream(std::ostream& os) const
{
	os << "," << _s_t_str;
	os << "," << _p;
	//os << "," << _u_vol;
	//os << "," << _d_vol;
	//os << "," << _u_tran;
	//os << "," << _d_tran;
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFDataPoint& d_)
{
	d_.to_stream(os);

	d_.body_to_stream(os);
	return os;
}
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFDataPoint& d_)
{
	if (!d_.to_cdr(strm))
		return false;
	if (!(strm << d_._key))
		return false;
	if (!(strm << d_._s_t))
		return false;
	if (!(strm << d_._s_t_str))
		return false;
	if (!(strm << d_._l))
		return false;
	if (!(strm << d_._p))
		return false;
	if (!(strm << d_._p_activity))
		return false;
	if (!(strm << d_._v_activity))
		return false;
	if (!(strm << d_._corr))
		return false;
	if (!(strm << d_._u_vol))
		return false;
	if (!(strm << d_._u_tran))
		return false;
	if (!(strm << d_._d_vol))
		return false;
	if (!(strm << d_._d_tran))
		return false;
	if (!(strm << d_._obv))
		return false;
	if (!(strm << d_._vma))
		return false;
	if (!(strm << d_._mfi))
		return false;
	if (!(strm << d_._comment))
		return false;
	return true;
}

bool operator>>(ACE_InputCDR& strm, LFDataPoint& d_)
{
	if (!d_.from_cdr(strm))
		return false;
	if (!(strm >> d_._key))
		return false;
	if (!(strm >> d_._s_t))
		return false;
	if (!(strm >> d_._s_t_str))
		return false;
	if (!(strm >> d_._l))
		return false;
	if (!(strm >> d_._p))
		return false;
	if (!(strm >> d_._p_activity))
		return false;
	if (!(strm >> d_._v_activity))
		return false;
	if (!(strm >> d_._corr))
		return false;
	if (!(strm >> d_._u_vol))
		return false;
	if (!(strm >> d_._u_tran))
		return false;
	if (!(strm >> d_._d_vol))
		return false;
	if (!(strm >> d_._d_tran))
		return false;
	if (!(strm >> d_._obv))
		return false;
	if (!(strm >> d_._vma))
		return false;
	if (!(strm >> d_._mfi))
		return false;
	if (!(strm >> d_._comment))
		return false;
	return true;
}
//======================================================================
void LFDataPoint::reset(const ACE_Time_Value& t_, double l_,
	double p_, double p_activity_, double v_activity_, double corr_,
	double u_vol_, double u_tran_, double d_vol_, double d_tran_)
{
	_s_t = t_;
	_s_t_str = TCTimestamp(_s_t).c_str();
	_t = t_;
	_run = BMModel::instance()->run_num();
	_l = l_;
	_p = p_;
	_p_activity = p_activity_;
	_v_activity = v_activity_;
	_corr = corr_;
	_u_vol = u_vol_;
	_u_tran = u_tran_;
	_d_vol = d_vol_;
	_d_tran = d_tran_;
	_obv = 0;
	_vma = 0;
	_mfi = 0;
	_comment.clear();
}

//======================================================================
//======================================================================
struct LFPointHeader
{
	LFPointHeader()
	{
		_h = ",S_KEY,ID,SIDE";
		_h += LFDataPoint::summary_header();
		_h += ",STRENGTH,COMMENT";

		_sum_h = ",ID,SIDE";
		_sum_h += LFDataPoint::summary_header();
	}
	std::string _h;
	std::string _sum_h;
};
//======================================================================
void LFPoint::body_to_stream(std::ostream& os) const
{
	os << "," << _key;
	os << "," << _id.to_alfa();
	os << "," << _side;
	LF::summary_to_stream(os, _dp);
	os << "," << _strength;
	os << "," << _comment;

}
//======================================================================
void LFPoint::summary_to_stream(std::ostream& os) const
{
	os << "," << _id.to_alfa();
	os << "," << _side;
	LF::summary_to_stream(os, _dp);

}
//======================================================================
const char* LFPoint::header()
{
	return ACE_Singleton <LFPointHeader, ACE_Thread_Mutex>::instance()->_h.c_str();
}
//======================================================================
const char* LFPoint::summary_header()
{
	return ACE_Singleton <LFPointHeader, ACE_Thread_Mutex>::instance()->_sum_h.c_str();
}
//======================================================================
//======================================================================
LFPoint::LFPoint(const std::string& key_)
 :_key(key_),
 	_side(0),
	_strength(0)
	
{
}
//======================================================================
LFPoint::~LFPoint()
{
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFPoint& d_)
{
	d_.to_stream(os);

	d_.body_to_stream(os);
	return os;
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFPoint& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._key)) 
	  return false;
	if (!(strm << d_._id)) 
	  return false;
	if (!(strm << d_._side))
	  return false;
	if (!(strm << d_._dp)) 
	  return false;
	if (!(strm << d_._strength)) 
	  return false;
	if (!(strm << d_._comment)) 
	  return false;
	return true;
 }

bool operator>>(ACE_InputCDR& strm, LFPoint& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._key)) 
	  return false;
	if (!(strm >> d_._id)) 
	  return false;
	if (!(strm >> d_._side))
	  return false;
	if (!(strm >> d_._dp)) 
	  return false;
	if (!(strm >> d_._strength)) 
	  return false;
	if (!(strm >> d_._comment)) 
	  return false;

return true;
}
//======================================================================
void LFPoint::reset_point(const LFDataPointPtr& dp_)
{
	_dp = dp_;
	_t = LF::get_t(_dp);
}
//======================================================================
void LFPoint::reset(const LFSide* side_, const LFDataPointPtr& dp_)
{
	_id.reset();
	_side = side_;
	_dp = dp_;
	_t = LF::get_t(_dp);
	_run = BMModel::instance()->run_num();
	_strength = 0;
	_comment.clear();
}
//======================================================================
void LFPoint::reset_identity(long scale_id_, long roll_id_, long child_id_, long top_scale_)
{
	_id._scale = scale_id_;
	_id._roll = roll_id_;
	_id._child = child_id_;
	_id._top_scale = top_scale_;
}
//======================================================================
void LFPoint::set_identity(const LFPoint& from_)
{
	_id = from_._id;
}
//======================================================================
bool LFPoint::cross(double p_, double last_p_) const
{
	if(!valid())
		return false;
	return _side->b_diff(p_, _dp->_p) < 0 && _side->b_diff(last_p_, _dp->_p) >= 0;
}
//======================================================================
double LFPoint::value(double p_, double tick_size_) const
{
	if(!valid())
		return 0.;
	return _side->v(p_, _dp->_p, tick_size_);
}
bool LFPoint::in_approach(double p_, double level_p_, double approach_factor_, double tick_size_) const
{
	double level_v = value(level_p_, tick_size_);
	if(level_v <= 0)
		return false;
	double v = value(p_, tick_size_);
	if(v <= 0 || v >= level_v)
		return false;
	return v > level_v*(1. - approach_factor_);

}

//======================================================================
double LFPoint::price_from_value(double s_p_, double v_, double tick_size_) const
{
	if(!valid())
		return 0.;
	return _side->p_from_v(s_p_, v_, tick_size_);
}
//======================================================================
double LFPoint::price_from_value(double v_, double tick_size_) const
{
	if(!valid())
		return 0.;
	return price_from_value(_dp->_p, v_, tick_size_);
}
//======================================================================
double LFPoint::length(const ACE_Time_Value& t_) const
{
	if (!valid() || t_ < _dp->_s_t)
		return 0.;
	return LF::length_from_t(t_ - _dp->_s_t);
}
//======================================================================
ACE_Time_Value LFPoint::t_from_length(double l_) const
{
	if(!valid())
		return ACE_Time_Value::zero;
	return _dp->_s_t + LF::t_from_length(l_);
}
//======================================================================
