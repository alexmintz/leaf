/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMove.h"

//======================================================================
std::ostream& operator<<(std::ostream& os, const LFMove& d_)
{
	os << d_._move_id;
//	os << "," << d_._m_strength ;
	os << "," << d_._w._in._side;
	os << "," << d_._m;
	os << "," << d_._w._in._v;
	os << "," << d_._w._in._eng_v;
	os << "," << d_._end_p;
//	os << "," << d_._end_t.sec();

	return os;
}
//======================================================================
void LFMove::reset(const LFMove& a_move_, const LFSide* side_, const ACE_Time_Value& start_t_, double start_p_, double eng_, const LFMove& c_move_)
{
	if(!c_move_.valid())
	{
		// first move
		_w.reset(side_, start_t_, start_p_, eng_);
	}
	else if(!c_move_._w._in.working())
	{
		// conta slope is complete, start new slope
		_w.reset(c_move_._w._out, start_t_, start_p_, eng_, c_move_._w._in);
	}
	else if(!a_move_._w._in.working())
	{
		// allied slope is not valid or complete, start new slope
		_w.reset(c_move_._w._out, start_t_, start_p_, eng_, c_move_._w._in);
	}
	else
	{
		// continue allied slope
		_w.reset(a_move_._w._in, start_t_, start_p_, eng_, c_move_._w._in);
	}

}
//======================================================================
void LFMove::reset(const LFSide* side_, const ACE_Time_Value& start_t_, double start_p_, double eng_)
{
	_w.reset(side_, start_t_, start_p_, eng_);
	_move_id = 0;
	_m_strength = 0.;
	_m = 0.;

	_end_p = 0;
	_end_t = ACE_Time_Value::zero;
}
//======================================================================
void LFMove::update(double m_, const ACE_Time_Value& t_, double p_, double eng_)
{
	if(!valid())
		return;
	_w.update(t_, p_, eng_);

	if(_w._l->psign(m_)*_w.psign() > 0)
	{
		double s = _w._l->abs_strength(m_);
		if(s > _m_strength)
			_m_strength = s;
		if(::fabs(m_) > _m)
			_m = ::fabs(m_);
	}
}
//======================================================================
void LFMove::ended(const ACE_Time_Value& t_, double p_) 
{ 
	_end_p = p_;
	_end_t = t_; 
}

//======================================================================
bool LFMove::move_confirmed(const LFSide* side_, const LFMove& cur_move_, const LFMove& prev_move_, bool on_ended_)
{
	if(side_->sign()*cur_move_.psign() > 0 && cur_move_.ended())
		return !cur_move_._w._in.working() || on_ended_;
	return false;
}

//======================================================================
