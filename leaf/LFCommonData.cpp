/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFCommonData.h"
#include <cstdlib> 
#include <set>
#include <cfloat>
//======================================================================
//======================================================================
double  LF::length_from_msec(double msec_)
{
	return msec_/ 60000.;
}
//======================================================================
double LF::length_from_t(const ACE_Time_Value& t_)
{
	return  LF::length_from_msec(double(t_.msec()));
}
//======================================================================
ACE_Time_Value LF::t_from_length(double l_)
{
	ACE_Time_Value t;
	t.msec(long(l_*60000.));
	return t;
}
//======================================================================
double LF::ratio_in_range(double v_, double from_, double to_)
{
	if(from_ > to_)
		return 0;
	if(v_ < from_)
		return 0;
	if(v_ >= to_)
		return 1.;
	return (v_ - from_)/(to_- from_);
}
//======================================================================
double LF::value_in_range(double ratio_, double from_, double to_)
{
	return (to_ - from_)*ratio_;
}

//======================================================================
//======================================================================
std::string LF::Pattern::to_alfa() const
{
//	char buf[24];
//	ACE_OS::sprintf(buf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 

	std::string s;

	s += test(LF::pb_ACCELERATED) ? 'A' : '_';
	s += test(LF::pb_COUNTINUOUS) ? 'C' : '_';
	s += test(LF::pb_STRAIGHT) ?	'S' : '_';
	s += test(LF::pb_INTERRUPTED) ? 'i' : '_';

		
	return s;
}
std::string LF::TEPattern::to_alfa() const
{
	//std::ostringstream os;
	std::string s;
	s += test(LF::te_ROLL_ENTER) ? 'x' : '_';
	

	return s;
}
std::string LF::MatchPatternMask::to_alfa() const
{
	std::string s;
	//s+= test(LF::mp_SUPPORT)?		'S':'_';
	//s+= test(LF::mp_NO_SUPPORT)?	's':'_';
	s += test(LF::mp_PIAS) ?		'a' : '_';
	s += test(LF::mp_DENSITY) ? 'd' : '_';
	s += test(LF::mp_SLOPE) ?	's' : '_';
	
	//s+= test(LF::mp_A_0)?			'0':'_';
	//s+= test(LF::mp_A_1)?			'1':'_';
	//s+= test(LF::mp_A_2)?			'2':'_';
	//s+= test(LF::mp_P_IN)?			'I':'_';
	//s+= test(LF::mp_P_MIN)?			'm':'_';
	//s+= test(LF::mp_P_WEAK)?		'p':'_';
	//s+= test(LF::mp_P_BUST)?		'P':'_';
	//s+= test(LF::mp_N_MAX)?			'M':'_';
	//s+= test(LF::mp_N_WEAK)?		'n':'_';
	//s+= test(LF::mp_N_BUST)?		'N':'_';
	//s+= test(LF::mp_B_WEAK)?		'b':'_';
	//s+= test(LF::mp_B_BUST)?		'B':'_';
	//s+= test(LF::mp_T_WEAK)?		't':'_';
	//s+= test(LF::mp_T_BUST)?		'T':'_';
	return s;
}
std::string LF::MatchMask::to_alfa() const
{
	std::string s;
	s+= test(LF::mb_CHARGE)?			'C':'_';
	s+= test(LF::mb_CHARGE_DIFF)?		'c':'_';
	s+= test(LF::mb_T_DIFF)?			't':'_';
	s+= test(LF::mb_ROLL)?				'r':'_';
	s+= test(LF::mb_BLOCK)?				'b':'_';
	s += test(LF::mb_NO_EXP) ?			'e' : '_';
	s += test(LF::mb_VOLUME) ?			'V' : '_';
	s += test(LF::mb_IN_VOLUME) ?		'v' : '_';
	//s+= test(LF::mb_PIAS)?				'P':'_';
	//s+= test(LF::mb_RISK)?				'R':'_';
	//s+= test(LF::mb_TNT)?				'T':'_';
	s+= test(LF::mb_INBAND)?			'I':'_';

	return s;
}
std::string LF::MatchExecMask::to_alfa() const
{
	std::string s;
	s+= test(LF::me_CUTLOSS)?	'L':'_';
	s+= test(LF::me_PROFIT)?	'P':'_';
	s+= test(LF::me_SECONDARY)?	'2':'_';
	s+= test(LF::me_SINGLEENTRY)?	'S':'_';
	s+= test(LF::me_CROSS)?		'x':'_';
	
	return s;
}

//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LF::Id& d_)
{
	if (!(strm << d_._scale))
		return false;
	if (!(strm << d_._roll))
		return false;
	if (!(strm << d_._child))
		return false;
	if (!(strm << d_._top_scale))
		return false;
	return true;
}
//======================================================================
bool operator>>(ACE_InputCDR& strm, LF::Id& d_)
{
	if (!(strm >> d_._scale))
		return false;
	if (!(strm >> d_._roll))
		return false;
	if (!(strm >> d_._child))
		return false;
	if (!(strm >> d_._top_scale))
		return false;
	return true;
}
//======================================================================
void LF::Id::reset()
{
	_scale = -1;
	_roll = -1;
	_child = -1;
	_top_scale = -1;
}
//======================================================================
std::string LF::Id::to_alfa() const
{
	std::ostringstream os;
	os << _scale << "_" << _roll << "_" << _child << "_" << _top_scale;
	return os.str();
}

//======================================================================
LFPeakAction::LFPeakAction() 
: Papa(LFEnterExit(), LFEnterExit()),
_need_exit(false)
{
}
//======================================================================
void LFPeakAction::reset() 
{ 
	Papa::reset(LFEnterExit(), LFEnterExit());
	_need_exit = false;
} 
//======================================================================
void LFPeakAction::set(const LFSide* side_, double q_, bool a_)
{
	if(q_ > 0)
	{
		(*this)[side_->value()].first = a_;
		if(a_)
			(*this)[side_->c_value()].second = false;
	}
	else if(q_ == 0)
	{
		(*this)[side_->value()].second = a_;
		//if(a_)
		//	(*this)[side_->c_value()].first = false;
	}
	else
	{
		(*this)[side_->c_value()].second = a_;
	}
}
//======================================================================
bool LFPeakAction::test(const LFSide* side_, double q_) const
{
	if(q_ > 0)
		return (*this)[side_->value()].first;
	return (*this)[side_->value()].second;
}
//======================================================================
std::string LFPeakAction::to_string() const
{
	char buf[8];
	ACE_OS::sprintf(buf, "%c%c%c%c%c", _v[0].first?'1':'0', _v[0].second?'1':'0', _v[1].first?'1':'0', _v[1].second?'1':'0', _need_exit?'1':'0');
	return std::string(buf);
}
//======================================================================
//======================================================================
LFCutLoss::LFCutLoss()
: Papa(0., 0.), _action(LF::paUndefined)
{
	_pre_cl_ticks = Z::get_setting_double("LFModel:HighLow:follower:pre_cl_ticks", 3);
}
//======================================================================
void LFCutLoss::reset()
{
	Papa::reset(0., 0.);
	_action = LF::paUndefined;
}
//======================================================================
void LFCutLoss::reset(const LFSide* side_)
{
	double& p = (*this)[side_->value()];
	p = 0;
	_action = LF::paUndefined;
}
//======================================================================
void LFCutLoss::set(const LFSide* side_, double p_, bool force_)
{
	double& p = (*this)[side_->value()];
	if(p == 0. || force_ || side_->b_diff(p_, p) > 0)
		p = p_;
	cross_reset(side_->contra_s(), p_);
}
//======================================================================
bool LFCutLoss::cross(const LFSide* side_, double p_) const
{
	const double& p = (*this)[side_->value()];
	if(p != 0. && side_->b_diff(p_, p) < 0)
		return true;
	return false;
}
//======================================================================
bool LFCutLoss::cross_reset(const LFSide* side_, double p_)
{
	double& p = (*this)[side_->value()];
	if(p != 0.)
	{
		double clt = side_->b_diff(p_, p);
		if(clt < 0)
		{
			p = 0.;
			_action = LF::paCutLoss;
			return true;
		}
		if(clt < _pre_cl_ticks)
			_action = LF::paPreCutLoss;
	}
	return false;
}
//======================================================================
const LFSide* LFCutLoss::cross_reset(double p_)
{
	const LFSide* s = LFSide::s(LF::s_BUY);
	if(cross_reset(s, p_))
		return s;
	s = LFSide::s(LF::s_SELL);
	if(cross_reset(s, p_))
		return s;
	return 0;
}
//======================================================================
std::string LFCutLoss::to_string() const
{
	char buf[256];
	ACE_OS::sprintf(buf, "%.2f_%.2f", _v[0], _v[1]);
	return std::string(buf);
}
//======================================================================
