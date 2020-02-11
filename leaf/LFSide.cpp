/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/
#include "leaf/LFSide.h"

//======================================================================
const LFSideBuy LFSideBuy::_i;
const LFSideSell LFSideSell::_i;
//======================================================================
LF::Side LFSide::contra(LF::Side v_)
{
	switch(v_) {
	case LF::s_BUY:
		return LF::s_SELL;
	case LF::s_SELL:
		return LF::s_BUY;
	default:
		return LF::s_UNKNOWN;
	};
	z_throw("LFSide::contra -- Unknown Verb.");
}
//======================================================================
LF::Side LFSide::by_sign(double sgn_)
{
	if(sgn_ == 0.)
		return LF::s_UNKNOWN;
	return (sgn_ > 0.)? LF::s_BUY:LF::s_SELL;
}

//======================================================================
const LFSide* LFSide::s_by_sign(double sgn_)
{
	return s(by_sign(sgn_));
}
//======================================================================
//======================================================================
const LFSide* LFSide::s(LF::Side v_)
{
	switch(v_) {
	case LF::s_BUY:
		return &LFSideBuy::_i;
	case LF::s_SELL:
		return &LFSideSell::_i;
	default:
		return 0;
	};
	return 0;
}
//======================================================================
LFSide* LFSide::make_side(LF::Side v_)
{
	switch(v_) {
	case LF::s_BUY:
		return new LFSideBuy;
	case LF::s_SELL:
		return new LFSideSell;
	default:
		z_throw("LFSide::make_side -- Unknown Side.");
	};
	z_throw("LFSide::make_side -- Unknown Side.");
}
//======================================================================
//======================================================================
bool LFSide::b_or_eq(double p_, double point_p_) const
{
	if(TCCompare::is_NaN(p_))
		return false;
	if (TCCompare::is_NaN(point_p_))
		return true;
	return b_diff(p_, point_p_) >= 0;
}
//======================================================================
double LFSide::mod_tick(double p_, double tick_, double* ticks_)
{
	if(tick_ == 0)
	{
		ticks_ = 0;
		return 0;
	}
	double fract = ::modf(p_/tick_, ticks_);
	if(TCComparePrice().diff(fract, 0) == 0)
	{
		fract = 0;
	}
	else if(TCComparePrice().diff(fract, 1) == 0)
	{
		(*ticks_)++;
		fract = 0;
	}
	else if(TCComparePrice().diff(fract, -1) == 0)
	{
		(*ticks_)--;
		fract = 0;
	}
	return fract;
}

//======================================================================
//======================================================================
const LFSide* LFSideBuy::contra_s() const 
{ 
	return &LFSideSell::_i; 
}
//======================================================================
void LFSideBuy::fill_contra(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqp_.reset();
	if(!md_._ask[0].good())
		return;
	pqp_._q = adjust_q(md_._ask[0]._q, adjust_0_q_, scale_);
	if(pqp_._q > 0)
		pqp_._p = md_._ask[0]._p;
}
//======================================================================
void LFSideBuy::fill_same(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqp_.reset();
	if(!md_._bid[0].good())
		return;
	pqp_._q = adjust_q(md_._bid[0]._q, adjust_0_q_, scale_);
	if(pqp_._q > 0)
		pqp_._p = md_._bid[0]._p;
}
//======================================================================
void LFSideBuy::fill_contra(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqpv_.clear();
	TC_pqp pqp;
	double tick = 0;

	fill_contra(pqp, md_);
	if(!pqp.good())
		return;
	
	pqpv_.push_back(pqp);
	
	for(int i = 1; i < md_.DEPTH_SIZE; ++i)
	{
		pqp.reset();
		LFSide::mod_tick(md_._ask[i]._p - md_._ask[i-1]._p, md_._tick_size, &tick);
		if(!md_._ask[i].good() || tick != 1)
			continue;
		pqp._q = adjust_q(md_._ask[i]._q, adjust_0_q_, scale_);
		if(pqp._q > 0)
			pqp._p = md_._ask[i]._p;
		pqpv_.push_back(pqp);
	}
}
//======================================================================
void LFSideBuy::fill_same(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqpv_.clear();
	TC_pqp pqp;
	double tick = 0;

	fill_same(pqp, md_);
	if(!pqp.good())
		return;
	
	pqpv_.push_back(pqp);
	
	for(int i = 1; i < md_.DEPTH_SIZE; ++i)
	{
		pqp.reset();
		LFSide::mod_tick(md_._bid[i-1]._p - md_._bid[i]._p, md_._tick_size, &tick);
		if(!md_._bid[i].good() || tick != 1)
			continue;
		pqp._q = adjust_q(md_._bid[i]._q, adjust_0_q_, scale_);
		if(pqp._q > 0)
			pqp._p = md_._bid[i]._p;
		pqpv_.push_back(pqp);
	}
}
//======================================================================
double LFSideBuy::b_diff(double p_, double point_p_) const
{
	return TCComparePrice().diff(point_p_, p_);
}
//======================================================================
double LFSideBuy::round(double p_, double tick_)
{
	if(tick_ == 0)
		return p_;

	double ticks = 0;
	mod_tick(p_, tick_, &ticks);
	return ticks * tick_;
}
//======================================================================
//======================================================================
const LFSide* LFSideSell::contra_s() const 
{ 
	return &LFSideBuy::_i; 
}
//======================================================================
void LFSideSell::fill_contra(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqp_.reset();
	if(!md_._bid[0].good())
		return;
	pqp_._q = adjust_q(md_._bid[0]._q, adjust_0_q_, scale_);
	if(pqp_._q > 0)
		pqp_._p = md_._bid[0]._p;
}
//======================================================================
void LFSideSell::fill_same(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqp_.reset();
	if(!md_._ask[0].good())
		return;
	pqp_._q = adjust_q(md_._ask[0]._q, adjust_0_q_, scale_);
	if(pqp_._q > 0)
		pqp_._p = md_._ask[0]._p;
}
//======================================================================
void LFSideSell::fill_same(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqpv_.clear();
	TC_pqp pqp;
	double tick = 0;

	fill_same(pqp, md_);
	if(!pqp.good())
		return;
	
	pqpv_.push_back(pqp);
	
	for(int i = 1; i < md_.DEPTH_SIZE; ++i)
	{
		pqp.reset();
		LFSide::mod_tick(md_._ask[i]._p - md_._ask[i-1]._p, md_._tick_size, &tick);
		if(!md_._ask[i].good() || tick != 1)
			continue;
		pqp._q = adjust_q(md_._ask[i]._q, adjust_0_q_, scale_);
		if(pqp._q > 0)
			pqp._p = md_._ask[i]._p;
		pqpv_.push_back(pqp);
	}
}
//======================================================================
void LFSideSell::fill_contra(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_, double scale_) const
{
	pqpv_.clear();
	TC_pqp pqp;
	double tick = 0;

	fill_contra(pqp, md_);
	if(!pqp.good())
		return;
	
	pqpv_.push_back(pqp);
	
	for(int i = 1; i < md_.DEPTH_SIZE; ++i)
	{
		pqp.reset();
		LFSide::mod_tick(md_._bid[i-1]._p - md_._bid[i]._p, md_._tick_size, &tick);
		if(!md_._bid[i].good() || tick != 1)
			continue;
		pqp._q = adjust_q(md_._bid[i]._q, adjust_0_q_, scale_);
		if(pqp._q > 0)
			pqp._p = md_._bid[i]._p;
		pqpv_.push_back(pqp);
	}
}
//======================================================================
double LFSideSell::b_diff(double p_, double point_p_) const
{
	return TCComparePrice().diff(p_, point_p_);
}
//======================================================================
double LFSideSell::round(double p_, double tick_)
{
	if(tick_ == 0)
		return p_;

	double ticks = 0;
	if(mod_tick(p_, tick_, &ticks) > 0)
		ticks++;
	return ticks * tick_;
}
//======================================================================
//======================================================================
bool operator<<(ACE_OutputCDR& strm, const LFSide* s)
{
	ACE_CDR::ULong x(s==0?LF::s_UNKNOWN:s->value());
	return strm << x;
}
bool operator>>(ACE_InputCDR& strm, const LFSide*& s)
{
	ACE_CDR::ULong x;
	bool r = strm >> x;
	s = LFSide::s((LF::Side)x);
	return r;
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFSide* s)
{
	os	<< long(s==0?LF::s_UNKNOWN:s->value());
	return os;
}
//======================================================================

