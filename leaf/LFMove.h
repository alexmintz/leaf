/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MOVE_H
#define LF_MOVE_H

#include "znet/ZNet.h"
#include "leaf/LFWave.h"

//======================================================================
struct LEAF_Export LFMove
{
	LFMove(LFMarketModel* l_)
		: _w(l_)
	{
		reset();
	}
	virtual ~LFMove() {}

	void init() {_w.init(); }

	void reset(const LFMove& a_move_, const LFSide* side_, const ACE_Time_Value& start_t_, double start_p_, double eng_, const LFMove& c_move_);
	void reset(const LFSide* side_ = 0, const ACE_Time_Value& start_t_ = ACE_Time_Value::zero, double start_p_ = 0., double eng_ = 0);
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFMove& d_);

	bool valid() const { return _w.valid(); }
	bool ended() const { return _end_t != ACE_Time_Value::zero; }

	void update(double m_, const ACE_Time_Value& t_, double p_, double eng_);
	void ended(const ACE_Time_Value& t_, double p_);
	const LFSide* side() const { return _w.side(); }

	static bool move_confirmed(const LFSide* side_, const LFMove& cur_move_, const LFMove& prev_move_, bool on_ended_);

	double psign() const
	{
		return _w.psign();
	}

	long	_move_id;
	double _m_strength;
	double _m; 
	
	LFWave _w;

	double _end_p;
	ACE_Time_Value _end_t;


};
typedef Z::smart_ptr<LFMove> LFMovePtr;

//======================================================================
#endif /*LF_MOVE_H*/
