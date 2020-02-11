/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMatch.h"
#include "leaf/LFMomentumData.h"

//======================================================================
struct LFMatchHeader
{
	LFMatchHeader()
	{
		_h += ",S_KEY,S_T,S_P,SIDE,COMMENT,P_MASK,MASK,E_MASK,QTY";
		_h += ",SCALE,MAX_V,MAX_L,MIN_V,MIN_L,END_V,END_L,SUCCESS,LOSS_V,LOSS_L,PROFIT_V,PROFIT_L,M_L,M_V";
		_h += ",PEAK_V,PEAK_SLOPE,PEAK_TIP,PEAK_VOL,PEAK_TRN,PEAK_DEN,PEAK_FRQ";
		_h += ",M_VOL,M_TRN,M_DEN,M_FRQ,M_VOL_PCT,EXP_V,BAND_V,BAND_F,PEAK_F,BOUNCE_F,TEST_F,M_B_PCT,M_RATIO,M_REPEAT";
//		_h += LFBox::summary_header();
		_h += LFBox::header();
	}
	std::string _h;
};

//======================================================================
//======================================================================
const char* LFMatch::header()
{
	return ACE_Singleton <LFMatchHeader, ACE_Thread_Mutex>::instance()->_h.c_str();
}

//======================================================================
std::ostream& operator<<(std::ostream& os, const LFMatch& d_)
{
	d_.to_stream(os);

	os	<< "," << d_._key;
	LF::summary_to_stream(os, d_._dp);
	os << "," << d_._side ;
	os << "," << d_._comment ;
	os << "," << d_._mp_mask.to_alfa() ;
	os << "," << d_._m_mask.to_alfa() ;
	os << "," << d_._me_mask.to_alfa() ;
	os << "," << d_._qty ;

	os << "," << d_._m_scale ;
	os << "," << d_._res_max_v ;
	os << "," << d_._res_max_l;
	os << "," << d_._res_min_v;
	os << "," << d_._res_min_l ;
	os << "," << d_._res_end_v ;
	os << "," << d_._res_end_l ;

	os << "," << d_._m_success?1:0;
	os << "," << d_._m_loss_v ;
	os << "," << d_._m_loss_l ;
	os << "," << d_._m_profit_v ;
	os << "," << d_._m_profit_l ;

	os << "," << d_._m_l ;
	os << "," << d_._m_v ;
	os << "," << d_._peak_v ;
	os << "," << d_._peak_slope ;
	os << "," << d_._peak_tip ;
	
	os << "," << d_._peak_vol;
	os << "," << d_._peak_trn;
	os << "," << d_._peak_den;
	os << "," << d_._peak_frq;

	os << "," << d_._m_vol;
	os << "," << d_._m_trn;
	os << "," << d_._m_den;
	os << "," << d_._m_frq;

	os << "," << d_._m_vol_pct;
	os << "," << d_._exp_v ;
	os << "," << d_._band_v ;
	os << "," << d_._band_f ;
	os << "," << d_._peak_f ;
	os << "," << d_._bounce_f ;
	os << "," << d_._test_f ;
	os << "," << d_._m_b_pct ;
	os << "," << d_._m_ratio ;
	os << "," << d_._m_repeat_charge ;

//	d_._box.summary_to_stream(os);
	d_._box.body_to_stream(os);

	return os;

}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFMatch& d_)
{
	const LFPoint& s = d_;

	if (!(strm << s)) 
	  return false;

	if (!(strm << d_._is_enter)) 
	  return false;
	if (!(strm << d_._m_name)) 
	  return false;
	if (!(strm << d_._qty)) 
	  return false;
	if (!(strm << d_._mp_mask.to_ulong())) 
	  return false;
	if (!(strm << d_._m_mask.to_ulong())) 
	  return false;
	if (!(strm << d_._me_mask.to_ulong())) 
	  return false;
	if (!(strm << d_._m_loss_v)) 
	  return false;
	if (!(strm << d_._m_profit_v)) 
	  return false;
	if (!(strm << d_._m_l)) 
	  return false;
	if (!(strm << d_._m_v)) 
	  return false;
	if (!(strm << d_._peak_slope)) 
	  return false;
	if (!(strm << d_._m_b_pct)) 
	  return false;
	if (!(strm << d_._m_ratio)) 
	  return false;
	if (!(strm << d_._m_loss_l)) 
	  return false;
	if (!(strm << d_._m_profit_l)) 
	  return false;

	if (!(strm << d_._m_scale)) 
	  return false;
	if (!(strm << d_._res_max_v)) 
	  return false;
	if (!(strm << d_._res_max_l)) 
	  return false;
	if (!(strm << d_._res_min_v)) 
	  return false;
	if (!(strm << d_._res_min_l)) 
	  return false;
	if (!(strm << d_._res_end_v)) 
	  return false;
	if (!(strm << d_._res_end_l)) 
	  return false;
	if (!(strm << d_._m_success)) 
	  return false;
	if (!(strm << d_._box)) 
	  return false;
	if (!(strm << d_._peak_tip)) 
	  return false;

	if (!(strm << d_._peak_vol))
		return false;
	if (!(strm << d_._peak_trn))
		return false;
	if (!(strm << d_._peak_den))
		return false;
	if (!(strm << d_._peak_frq))
		return false;

	if (!(strm << d_._m_vol))
		return false;
	if (!(strm << d_._m_trn))
		return false;
	if (!(strm << d_._m_den))
		return false;
	if (!(strm << d_._m_frq))
		return false;

	if (!(strm << d_._m_vol_pct))
		return false;

	if (!(strm << d_._peak_v)) 
	  return false;
	if (!(strm << d_._exp_v)) 
	  return false;
	if (!(strm << d_._band_v)) 
	  return false;
	if (!(strm << d_._band_f)) 
	  return false;
	if (!(strm << d_._peak_f)) 
	  return false;
	if (!(strm << d_._bounce_f)) 
	  return false;
	if (!(strm << d_._test_f)) 
	  return false;
	if (!(strm << d_._m_repeat_charge)) 
	  return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFMatch& d_)
{
	LFPoint& s = d_;

	if (!(strm >> s)) 
	  return false;
	if (!(strm >> d_._is_enter)) 
	  return false;
	if (!(strm >> d_._m_name)) 
	  return false;
	if (!(strm >> d_._qty)) 
	  return false;
	unsigned long mask = 0;
	if (!(strm >> mask)) 
	  return false;
	d_._mp_mask = LF::MatchPatternMask(mask);
	mask = 0;
	if (!(strm >> mask)) 
	  return false;
	d_._m_mask = LF::MatchMask(mask);
	mask = 0;
	if (!(strm >> mask)) 
	  return false;
	d_._me_mask = LF::MatchExecMask(mask);
	if (!(strm >> d_._m_loss_v)) 
	  return false;
	if (!(strm >> d_._m_profit_v)) 
	  return false;
	if (!(strm >> d_._m_l)) 
	  return false;
	if (!(strm >> d_._m_v)) 
	  return false;
	if (!(strm >> d_._peak_slope)) 
	  return false;
	if (!(strm >> d_._m_b_pct)) 
	  return false;
	if (!(strm >> d_._m_ratio)) 
	  return false;
	if (!(strm >> d_._m_loss_l)) 
	  return false;
	if (!(strm >> d_._m_profit_l)) 
	  return false;
	if (!(strm >> d_._m_scale)) 
	  return false;
	if (!(strm >> d_._res_max_v)) 
	  return false;
	if (!(strm >> d_._res_max_l)) 
	  return false;
	if (!(strm >> d_._res_min_v)) 
	  return false;
	if (!(strm >> d_._res_min_l)) 
	  return false;
	if (!(strm >> d_._res_end_v)) 
	  return false;
	if (!(strm >> d_._res_end_l)) 
	  return false;
	if (!(strm >> d_._m_success)) 
	  return false;
	if (!(strm >> d_._box)) 
	  return false;
	if (!(strm >> d_._peak_tip)) 
	  return false;

	if (!(strm >> d_._peak_vol))
		return false;
	if (!(strm >> d_._peak_trn))
		return false;
	if (!(strm >> d_._peak_den))
		return false;
	if (!(strm >> d_._peak_frq))
		return false;

	if (!(strm >> d_._m_vol))
		return false;
	if (!(strm >> d_._m_trn))
		return false;
	if (!(strm >> d_._m_den))
		return false;
	if (!(strm >> d_._m_frq))
		return false;

	if (!(strm >> d_._m_vol_pct))
		return false;

	if (!(strm >> d_._peak_v)) 
	  return false;
	if (!(strm >> d_._exp_v)) 
	  return false;
	if (!(strm >> d_._band_v)) 
	  return false;
	if (!(strm >> d_._band_f)) 
	  return false;
	if (!(strm >> d_._peak_f)) 
	  return false;
	if (!(strm >> d_._bounce_f)) 
	  return false;
	if (!(strm >> d_._test_f)) 
	  return false;
	if (!(strm >> d_._m_repeat_charge)) 
	  return false;
	return true;
}
//======================================================================
LFMatch::LFMatch(const std::string& name_, const std::string& key_, bool is_enter_)
 : LFPoint(key_),
	_is_enter(is_enter_),
	_m_name(name_),
	_q_multiplier(0),
	_qty(0),
	_m_loss_v(0),
	_m_profit_v(0),
	_m_l(0),
	_m_v(0),
	_m_loss_l(0),
	_m_profit_l(0),
	_m_scale(0),
	_res_max_v(0),
	_res_max_l(0),
	_res_min_v(0),
	_res_min_l(0),
	_res_end_v(0),
	_res_end_l(0),
	_m_success(false),
	_peak_slope(0),
	_peak_tip(0),
	_peak_vol(0),
	_peak_trn(0),
	_peak_den(0),
	_peak_frq(0),
	_m_vol(0),
	_m_trn(0),
	_m_den(0),
	_m_frq(0),
	_m_vol_pct(0),
	_peak_v(0),
	_exp_v(0),
	_band_v(0),
	_band_f(0),
	_peak_f(0),
	_bounce_f(0),
	_test_f(0),
	_m_b_pct(0),
	_m_ratio(0),
	_m_repeat_charge(0)

{
}
//======================================================================
LFMatch::~LFMatch() 
{
}
//======================================================================
//======================================================================
void LFMatch::reset(const LFSide* side_, const LFDataPointPtr& dp_)
{
	LFPoint::reset(side_, dp_);
	_q_multiplier = 0;
	_qty = 0;
	_mp_mask.reset();
	_m_mask.reset();
	_me_mask.reset();
	_m_loss_v = 0;
	_m_profit_v = 0;
	_m_l = 0;
	_m_v = 0;
	_m_loss_l = 0;
	_m_profit_l = 0;
	_m_scale = 0;
	_res_max_v = 0;
	_res_max_l = 0;
	_res_min_v = 0;
	_res_min_l = 0;
	_res_end_v = 0;
	_res_end_l = 0;
	_m_success = false;
	_box.reset();
	_peak_slope = 0;
	_peak_tip = 0;
	_peak_vol = 0;
	_peak_trn = 0;
	_peak_den = 0;
	_peak_frq = 0;

	_m_vol = 0;
	_m_trn = 0;
	_m_den = 0;
	_m_frq = 0;

	_m_vol_pct = 0;

	_peak_v = 0;
	_exp_v = 0;
	_band_v = 0;
	_band_f = 0;
	_peak_f = 0;
	_bounce_f = 0;
	_test_f = 0;
	_m_b_pct = 0;
	_m_ratio = 0;
	_m_repeat_charge = 0;
}
//======================================================================
