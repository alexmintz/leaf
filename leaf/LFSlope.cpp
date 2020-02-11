/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFSlope.h"

//======================================================================
//======================================================================
LFSlope::LFSlope(const std::string& key_)
 :LFPoint(key_),
	_s_eng(0),
	_p(0),
	_l(0),
	_v(0),
	_s(0),
	_e_p(0),
	_e_l(0),
	_e_v(0),
	_e_s(0),
	_ce_v(0),
	_ce_p(0),
	_u_p(0),
	_u_eng(0),
	_uv_eng(0),
	_e_den_v(0),
	_S_p(0),
	_S_eng(0),
	_S_p_logr(0),
	_S_eng_logr(0),
	_c_p(0),
	_c_l(0),
	_c_v(0),
	_c_s(0),
	_eng(0),
	_eng_v(0),
	_den_v(0),
	_c_eng(0),
	_c_eng_v(0),
	_exceeded(false)
{
}
//======================================================================
LFSlope::~LFSlope()
{
}
//======================================================================
struct LFSlopeHeader
{
	LFSlopeHeader()
		:
	_h(LFPoint::header())
	{
		_h += ",S_ENG,P,L,V,S,E_P,E_L,E_V,E_S,CE_V,CE_P,U_P,U_ENG,UV_ENG,E_DEN_V,\
SUM_P,SUM_ENG,SUM_P_LOGR,SUM_ENG_LOGR,C_P,C_L,C_V,C_S,ENG,ENG_V,DEN_V,C_ENG,C_ENG_V,\
EXCEEDED";
	}
	std::string _h;
};
//======================================================================
//======================================================================
const char* LFSlope::header()
{
	return ACE_Singleton <LFSlopeHeader, ACE_Thread_Mutex>::instance()->_h.c_str();
}

//======================================================================
std::ostream& operator<<(std::ostream& os, const LFSlope& d_)
{
	const LFPoint& cl = d_;
	os << cl;

	os << "," << d_._s_eng;
	os << "," << d_._p;
	os << "," << d_._l ;
	os << "," << d_._v ;
	os << "," << d_._s ;
	os << "," << d_._e_p ;
	os << "," << d_._e_l ;
	os << "," << d_._e_v ;
	os << "," << d_._e_s ;
	os << "," << d_._ce_v ;
	os << "," << d_._ce_p ;
	os << "," << d_._u_p ;
	os << "," << d_._u_eng ;
	os << "," << d_._uv_eng ;
	os << "," << d_._e_den_v ;
	os << "," << d_._S_p ;
	os << "," << d_._S_eng ;
	os << "," << d_._S_p_logr ;
	os << "," << d_._S_eng_logr ;
	os << "," << d_._c_p ;
	os << "," << d_._c_l ;
	os << "," << d_._c_v ;
	os << "," << d_._c_s ;
	os << "," << d_._eng ;
	os << "," << d_._eng_v ;
	os << "," << d_._den_v ;
	os << "," << d_._c_eng ;
	os << "," << d_._c_eng_v ;
	
	os << "," << d_._exceeded?1:0 ;
	return os;
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFSlope& d_)
{
	const LFPoint& cl = d_;
	if (!(strm << cl)) 
	  return false;
	if (!(strm << d_._s_eng))
		return false;
	if (!(strm << d_._p))
	  return false;
	if (!(strm << d_._l)) 
	  return false;
	if (!(strm << d_._v)) 
	  return false;
	if (!(strm << d_._s)) 
	  return false;
	if (!(strm << d_._e_p)) 
	  return false;
	if (!(strm << d_._e_l)) 
	  return false;
	if (!(strm << d_._e_v)) 
	  return false;
	if (!(strm << d_._e_s)) 
	  return false;
	if (!(strm << d_._ce_v)) 
	  return false;
	if (!(strm << d_._ce_p)) 
	  return false;
	if (!(strm << d_._u_p)) 
	  return false;
	if (!(strm << d_._u_eng)) 
	  return false;
	if (!(strm << d_._uv_eng)) 
	  return false;
	if (!(strm << d_._e_den_v)) 
	  return false;
	if (!(strm << d_._S_p)) 
	  return false;
	if (!(strm << d_._S_eng)) 
	  return false;
	if (!(strm << d_._S_p_logr)) 
	  return false;
	if (!(strm << d_._S_eng_logr)) 
	  return false;

	if (!(strm << d_._c_p)) 
	  return false;
	if (!(strm << d_._c_l)) 
	  return false;
	if (!(strm << d_._c_v)) 
	  return false;
	if (!(strm << d_._c_s)) 
	  return false;
	if (!(strm << d_._eng)) 
	  return false;
	if (!(strm << d_._eng_v)) 
	  return false;
	if (!(strm << d_._den_v)) 
	  return false;
	if (!(strm << d_._c_eng)) 
	  return false;
	if (!(strm << d_._c_eng_v)) 
	  return false;
	if (!(strm << d_._exceeded)) 
	  return false;
	return true;
}

bool operator>>(ACE_InputCDR& strm, LFSlope& d_)
{
	LFPoint& cl = d_;
	if (!(strm >> cl)) 
	  return false;
	if (!(strm >> d_._s_eng))
		return false;
	if (!(strm >> d_._p))
	  return false;
	if (!(strm >> d_._l)) 
	  return false;
	if (!(strm >> d_._v)) 
	  return false;
	if (!(strm >> d_._s)) 
	  return false;
	if (!(strm >> d_._e_p)) 
	  return false;
	if (!(strm >> d_._e_l)) 
	  return false;
	if (!(strm >> d_._e_v)) 
	  return false;
	if (!(strm >> d_._e_s)) 
	  return false;
	if (!(strm >> d_._ce_v)) 
	  return false;
	if (!(strm >> d_._ce_p)) 
	  return false;
	if (!(strm >> d_._u_p)) 
	  return false;
	if (!(strm >> d_._u_eng)) 
	  return false;
	if (!(strm >> d_._uv_eng)) 
	  return false;
	if (!(strm >> d_._e_den_v)) 
	  return false;
	if (!(strm >> d_._S_p)) 
	  return false;
	if (!(strm >> d_._S_eng)) 
	  return false;
	if (!(strm >> d_._S_p_logr)) 
	  return false;
	if (!(strm >> d_._S_eng_logr)) 
	  return false;


	if (!(strm >> d_._c_p)) 
	  return false;
	if (!(strm >> d_._c_l)) 
	  return false;
	if (!(strm >> d_._c_v)) 
	  return false;
	if (!(strm >> d_._c_s)) 
	  return false;
	if (!(strm >> d_._eng)) 
	  return false;
	if (!(strm >> d_._eng_v)) 
	  return false;
	if (!(strm >> d_._den_v)) 
	  return false;
	if (!(strm >> d_._c_eng)) 
	  return false;
	if (!(strm >> d_._c_eng_v)) 
	  return false;
	if (!(strm >> d_._exceeded)) 
	  return false;

	return true;
}
//======================================================================
void LFSlope::reset(const LFSide* side_, const ACE_Time_Value& t_, double p_, double eng_,
	double e_l_, double e_v_, double tick_size_)
{
	LFPoint::reset(side_, t_, p_);
	_s_eng = eng_;
	_e_p = p_;
	_e_l = 0;
	_e_v = 0;
	_e_s = 0;
	_p = p_;
	_l = 0;
	_v = 0;
	_s = 0;
	_ce_v = 0;
	_e_den_v = 0;
	_S_p = 0;
	_S_eng = 0;
	_S_p_logr = 0;
	_S_eng_logr = 0;
	_e_p = 0;
	_ce_p = p_;
	_u_p = 0;
	_u_eng = 0;
	_uv_eng = 0;
	_c_p = p_;
	_c_l = 0;
	_c_v = 0;
	_c_s = 0;
	_eng = eng_;
	_eng_v = 0;
	_den_v = 0;
	_c_eng = eng_;
	_c_eng_v = 0;
	_exceeded = false;
	if(valid())
	{
		_e_l = e_l_;
		_e_v = e_v_;
		_e_p = _s_p - _side->sign()*_e_v*tick_size_;
		if(_e_l > 0)
			_e_s = _e_v/_e_l;
	}
}
//======================================================================
void LFSlope::reset(const LFSlope& from_)
{
	std::string k = get_key();
	*this = from_;
	_exceeded = false;
	_comment.clear();
	set_key(k);
}
//======================================================================
double LFSlope::eng_value(double eng_) const
{
	if (!valid())
		return 0.;
	return _side->b_diff(eng_, _s_eng);
}
//======================================================================
double LFSlope::eng_from_value(double eng_v_) const
{
	if (!valid())
		return 0.;
	return _s_eng - _side->sign()*eng_v_;
}
//======================================================================
void LFSlope::adjust_e_l(const ACE_Time_Value& to_t_, double tick_size_)
{
	if(valid())
	{
		_e_l = _e_l + LF::length_from_t(to_t_ - _s_t);
		if(_e_l > 0)
			_e_s = _e_v/_e_l;
		_ce_v = ce_v();
		_ce_p = price_from_value(_ce_v, tick_size_);
	}
}
//======================================================================
void LFSlope::update(const ACE_Time_Value& t_, double p_, double eng_, double tick_size_, bool reset_max_, bool reset_max_eng_)
{
	if(!valid())
		return;
	bool was_working = working();
	_c_p = p_;
	double old_c_l = _c_l;
	_c_l = LF::length_from_t(t_ - _s_t);
	_c_v = value(_c_p, tick_size_);
	if(_e_v > 0 && _c_v >= _e_v)
		_exceeded = true;

	_c_s = 0;
	if(_c_l > 0)
		_c_s = _c_v/_c_l;

	_c_eng = eng_;
	_c_eng_v = eng_value(_c_eng);

	double old_S_p = _S_p;
	double old_S_eng = _S_eng;
	_S_p -= _c_v*(_c_l - old_c_l)*psign();
	_S_eng -= _c_eng_v*(_c_l - old_c_l)*psign();

	//_S_p_logr = (TCComparePrice::z(old_S_p)== 0 || TCComparePrice::z(_S_p)== 0)? 0: ::log(_S_p/old_S_p);
	//_S_eng_logr = (TCComparePrice::z(old_S_eng)== 0 || TCComparePrice::z(_S_eng)==0)? 0: ::log(_S_eng/old_S_eng);
		
	if(reset_max_ || _c_v > _v)
	{
//		_l = _c_l;
		_v = _c_v;
		_p = _c_p;
		_s = _c_s;
		reset_max_eng_ = true;
	}
	if(reset_max_eng_)
	{
		_l = _c_l;
		_eng = _c_eng;
		_eng_v = _c_eng_v;
		_den_v = _v > 0.? _eng_v/_v : 0.;
	}

	_ce_v = ce_v();
	_ce_p = price_from_value(_ce_v, tick_size_);

}

//======================================================================
void LFSlope::reset_s_eng(const ACE_Time_Value& t_, double eng_)
{
	_s_eng = eng_;
	_eng = eng_;
	_eng_v = 0;
	_den_v = 0;
	_c_eng = eng_;
	_c_eng_v = 0;
}
//======================================================================
void LFSlope::adjust_max(const LFSlope& from_)
{
	_v = from_._v;
	_p = from_._p;
	_s = from_._s;
	_l = from_._l;
	_eng = from_._eng;
	_eng_v = from_._eng_v;
	_den_v = from_._den_v;
}
//======================================================================
struct LFPeakHeader
{
	LFPeakHeader()
		:
	_h(LFSlope::header())
	{
		_h += ",PP_V,PP_ENGV,SP_ENG,\
VP_V,VP_ENGV,VP_DENV,VP_L,PV_V,PV_ENGV,PV_DENV,PV_L,\
CONFIRMED,J_CONFIRMED,J_BORN,FORMED,J_FORMED";
	}
	std::string _h;
};
//======================================================================
//======================================================================
const char* LFPeak::header()
{
	return ACE_Singleton <LFPeakHeader, ACE_Thread_Mutex>::instance()->_h.c_str();
}

//======================================================================
std::ostream& operator<<(std::ostream& os, const LFPeak& d_)
{
	const LFSlope& s = d_;
	os	<< s;

	os << "," << d_._pp_v;
	os << "," << d_._pp_eng_v;
	os << "," << d_._s_p_eng;
	
	os << "," << d_._vp_v;
	os << "," << d_._vp_eng_v;
	os << "," << d_._vp_den_v;
	os << "," << d_._vp_l;
	os << "," << d_._pv_v;
	os << "," << d_._pv_eng_v;
	os << "," << d_._pv_den_v;
	os << "," << d_._pv_l;
	
	os << "," << (long)d_._confirmed;
	os << "," << (long)d_._just_confirmed;
	os << "," << (long)d_._just_born;
	os << "," << (long)d_._formed;
	os << "," << (long)d_._just_formed;

	return os;

}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFPeak& d_)
{
	const LFSlope& s = d_;

	if (!(strm << s)) 
	  return false;
	
	if (!(strm << d_._pp_v)) 
	  return false;
	if (!(strm << d_._pp_eng_v)) 
	  return false;
	if (!(strm << d_._s_p_eng)) 
	  return false;
	if (!(strm << d_._vp_v)) 
	  return false;
	if (!(strm << d_._vp_eng_v)) 
	  return false;
	if (!(strm << d_._vp_den_v)) 
	  return false;
	if (!(strm << d_._pv_l)) 
	  return false;
	if (!(strm << d_._pv_v)) 
	  return false;
	if (!(strm << d_._pv_eng_v)) 
	  return false;
	if (!(strm << d_._pv_den_v)) 
	  return false;
	if (!(strm << d_._pv_l)) 
	  return false;
	if (!(strm << d_._confirmed)) 
	  return false;
	if (!(strm << d_._just_confirmed)) 
	  return false;
	if (!(strm << d_._just_born)) 
	  return false;
	if (!(strm << d_._formed)) 
	  return false;
	return true;

 }

bool operator>>(ACE_InputCDR& strm, LFPeak& d_)
{
	LFSlope& s = d_;

	if (!(strm >> s)) 
	  return false;
	if (!(strm >> d_._pp_v)) 
	  return false;
	if (!(strm >> d_._pp_eng_v)) 
	  return false;
	if (!(strm >> d_._s_p_eng)) 
	  return false;
	if (!(strm >> d_._vp_v)) 
	  return false;
	if (!(strm >> d_._vp_eng_v)) 
	  return false;
	if (!(strm >> d_._vp_den_v)) 
	  return false;
	if (!(strm >> d_._vp_l)) 
	  return false;
	if (!(strm >> d_._pv_v)) 
	  return false;
	if (!(strm >> d_._pv_eng_v)) 
	  return false;
	if (!(strm >> d_._pv_den_v)) 
	  return false;
	if (!(strm >> d_._pv_l)) 
	  return false;
	if (!(strm >> d_._confirmed)) 
	  return false;
	if (!(strm >> d_._just_confirmed)) 
	  return false;
	if (!(strm >> d_._just_born)) 
	  return false;
	if (!(strm >> d_._formed)) 
	  return false;
	return true;
}
 
//======================================================================
LFPeak::LFPeak(const std::string& key_)
 : LFSlope(key_),
	_pp_v(0),
	_pp_eng_v(0),
	_s_p_eng(0),
	_vp_v(0),
	_vp_eng_v(0),
	_vp_den_v(0),
	_vp_l(0),
	_pv_v(0),
	_pv_eng_v(0),
	_pv_den_v(0),
	_pv_l(0),
	_confirmed(false),
	_just_confirmed(false),
	_just_unconfirmed(false),
	_just_born(false),
	_formed(false),
	_just_formed(false)

{
}
//======================================================================
LFPeak::LFPeak(const LFSlope& s_)
: LFSlope(s_),
	_pp_v(0),
	_pp_eng_v(0),
	_s_p_eng(0),
	_vp_v(0),
	_vp_eng_v(0),
	_vp_den_v(0),
	_vp_l(0),
	_pv_v(0),
	_pv_eng_v(0),
	_pv_den_v(0),
	_pv_l(0),
	_confirmed(false),
	_just_confirmed(false),
	_just_unconfirmed(false),
	_just_born(false),
	_formed(false),
	_just_formed(false)
{
}
//======================================================================
LFPeak::~LFPeak() 
{
}
//======================================================================
//======================================================================
void LFPeak::reset(const LFSide* side_, const ACE_Time_Value& t_, double p_, double eng_,
	double e_l_, double e_v_, double tick_size_)
{
	LFSlope::reset(side_, t_, p_, eng_, e_l_, e_v_, tick_size_);
	_pp_v = 0;
	_pp_eng_v = 0;
	_s_p_eng = eng_;
	_vp_v = 0;
	_vp_eng_v = 0;
	_vp_den_v = 0;
	_vp_l = 0;
	_pv_v = 0;
	_pv_eng_v = 0;
	_pv_den_v = 0;
	_pv_l = 0;
	_confirmed = false;
	_just_confirmed = false;
	_just_unconfirmed = false;
	_just_born = false;
	_formed = false;
	_just_formed = false;
}

//======================================================================
void LFPeak::reset(const LFPeak& from_)
{
	std::string k = get_key();
	*this = from_;
	_comment.clear();
	set_key(k);
}
//======================================================================
void LFPeak::reset(const LFSlope& from_)
{
	reset();
	std::string k = get_key();
	LFSlope& s = *this;
	s = from_;
	_comment.clear();
	set_key(k);
}
//======================================================================
void LFPeak::clear_just_flags()
{
	_just_confirmed = false;
	_just_unconfirmed = false;
	_just_born = false;
	_just_formed = false;
}

//======================================================================
void LFPeak::update_peak(const LFPeak& prev_, bool confirmed_, double tick_size_, double min_first_peak_)
{
	if(!valid())
		return;

	bool was_confirmed = _confirmed;
	if(!_confirmed)
	{
		if(prev_.valid())
		{
			_pp_v = value(prev_._s_p, tick_size_);
			_pp_eng_v = eng_value(prev_._s_eng);

			_pv_v = prev_._v;
			_pv_eng_v = prev_._eng_v;
			_pv_den_v = prev_._den_v;
			_pv_l = prev_._l;

			_vp_v = _pp_v + _pv_v;
			_vp_eng_v = _pp_eng_v + _pv_eng_v;
			_vp_den_v = _vp_v > 0.?_vp_eng_v/_vp_v:0.;
			_vp_l = LF::length_from_t(_s_t - prev_._s_t) - _pv_l;
			
		}
	}

	if(confirmed_ && _vp_v >= min_first_peak_)
	{
		_confirmed = true;
	}

	_just_confirmed = !was_confirmed && _confirmed;
}

//======================================================================
