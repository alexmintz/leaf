/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFTeaser.h"

//======================================================================
//======================================================================
//======================================================================
const char* LFTeaser::header()
{
	return ",S_KEY,S_T,COM,SIDE,P,Q,PROCESSED,FUME,EXIT";
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFTeaser& d_)
{
	d_.to_stream(os);

	os << "," << d_._key;
	os << "," << d_._s_t_str;
	os << "," << d_._comment;
	os << "," << d_._side;
	os << "," << d_._p;
	os << "," << d_._q;
	os << "," << (long)d_._processed;
	os << "," << d_._fume;
	os << "," << d_._exit;
	return os;

}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFTeaser& d_)
{
	if (!d_.to_cdr(strm))
		return false;
	if (!(strm << d_._key))
		return false;
	if (!(strm << d_._s_t))
		return false;
	if (!(strm << d_._s_t_str))
		return false;
	if (!(strm << d_._side))
		return false;
	if (!(strm << d_._p))
		return false;
	if (!(strm << d_._q))
		return false;
	if (!(strm << d_._processed))
		return false;
	if (!(strm << d_._fume))
	  return false;
	if (!(strm << d_._exit)) 
	  return false;
	if (!(strm << d_._comment))
		return false;
	return true;
}
bool operator>>(ACE_InputCDR& strm, LFTeaser& d_)
{
	if (!d_.from_cdr(strm))
		return false;
	if (!(strm >> d_._key))
		return false;
	if (!(strm >> d_._s_t))
		return false;
	if (!(strm >> d_._s_t_str))
		return false;
	if (!(strm >> d_._side))
		return false;
	if (!(strm >> d_._p))
		return false;
	if (!(strm >> d_._q))
		return false;
	if (!(strm >> d_._processed))
	  return false;
	if (!(strm >> d_._fume)) 
	  return false;
	if (!(strm >> d_._exit)) 
	  return false;
	if (!(strm >> d_._comment))
		return false;
	return true;
}
//======================================================================
LFTeaser::LFTeaser(const std::string& key_)
 : _key(key_),
 _side(0),
 _p(0),
_q(0),
_processed(false),
_fume(0),
_exit(0)
{
}
//======================================================================
LFTeaser::~LFTeaser() 
{
}
//======================================================================
//======================================================================
void LFTeaser::reset(const LFSide* side_, const ACE_Time_Value& t_, double p_, double q_)
{
	_side = side_;
	_s_t = t_;
	_s_t_str = TCTimestamp(_s_t).c_str();
	_t = t_;
	_run = BMModel::instance()->run_num();
	_p = p_;
	_q = q_;
	_processed = false;
	_fume = 0;
	_exit = 0;
	_comment.clear();
}

//======================================================================
