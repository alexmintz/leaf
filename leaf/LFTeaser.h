/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_TEASER_H
#define LF_TEASER_H

#include "znet/ZNet.h"
#include "leaf/LFPoint.h"

//======================================================================
struct LEAF_Export LFTeaser : public Z::Point
{
	LFTeaser(const std::string& key_ = "");
	virtual ~LFTeaser();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) { _key = k_; }

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFTeaser& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFTeaser& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFTeaser& d_);

	virtual void reset(const LFSide* side_ = 0, const ACE_Time_Value& t_ = ACE_Time_Value::zero, double p_ = 0, double q_ = 0);
	virtual bool valid() const { return _side != 0 && _t != ACE_Time_Value::zero; }
	double psign() const
	{
		if (_side == 0)
			return 0;
		return _side->sign();
	}

	std::string _key;
	ACE_Time_Value _s_t;
	std::string _s_t_str;
	const LFSide*	_side;
	double	_p;
	double	_q;
	bool	_processed;
	double	_fume;
	double	_exit;
	std::string		_comment;

	static const char* header();
};
typedef Z::smart_ptr<LFTeaser> LFTeaserPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFTeaser)
typedef LF::Pair<LFTeaser> LFTeaserPair;
typedef std::vector<LFTeaser> LFTeaserVec;
typedef std::map<ACE_Time_Value, LFTeaserPtr> LFTeaserPtrMap;

//======================================================================
#endif /*LF_TEASER_H*/
