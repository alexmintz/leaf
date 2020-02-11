/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_WAVE_H
#define LF_WAVE_H

#include "znet/ZNet.h"
#include "leaf/LFSlope.h"
#include "leaf/LFMarketModel.h"

//======================================================================
struct LFMove;
//======================================================================
struct LEAF_Export LFWave
{
	LFWave(LFMarketModel* l_ = 0)
		:_l(l_),
		_wave_id(0)
	{
		//init();
		//reset();
	}
	/*
	LFWave(LFMarketModel* l_, const LFSide* side_, const ACE_Time_Value& t_, double p_, double eng_)
		:_l(l_)
	{
		init();
		reset(side_, t_, p_, eng_);
	}
	LFWave(LFMarketModel* l_, const LFSlope& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFSlope& prev_s_)
		:_l(l_)
	{
		init();
		reset(s_, t_, p_, eng_, prev_s_);
	}
	LFWave(LFMarketModel* l_, const LFPeak& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFPeak& prev_p_)
		:_l(l_)
	{
		init();
		reset(s_, t_, p_, eng_, prev_p_);
	}
	LFWave(LFMarketModel* l_, const LFWave& from_, const ACE_Time_Value& t_, double p_, double eng_)
		:_l(l_)
	{
		init();
		reset(from_, t_, p_, eng_);
	}
	*/
	virtual ~LFWave() {}

	void init()
	{
		if(_l == 0)
			return;
		std::string key = _l->_name;
		_in.set_key(key);
		_out.set_key(key);
		_prev.set_key(key);
	}

	void reset();
	void reset(const LFSide* side_, const ACE_Time_Value& t_, double p_, double eng_);
	void reset(const LFSlope& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFSlope& prev_s_);
	void reset(const LFPeak& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFPeak& prev_p_);
	void reset(const LFWave& from_, const ACE_Time_Value& t_, double p_, double eng_);

	void reset_all(const ACE_Time_Value& t_);

	bool equal(const LFWave& w_) const { return _in.equal(w_._in); }
	bool older(const LFWave& w_) const { return _in.older(w_._in);	}

	void update(const ACE_Time_Value& t_, double p_, double eng_, bool peak_confirmed_ = false, double min_first_peak_ = 0);
	void update(const ACE_Time_Value& t_, double p_, double eng_, const LFMove& cur_move_, const LFMove& prev_move_, LFSlope& folded_);

	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFWave& d_) { return os; }

	bool valid() const { return _in.valid(); } 
	
	const LFSide* side() const { return _in._side;}
	double psign() const { return _in.psign(); }

	double b_diff(const LFWave& from_) const
	{
		if(!valid() || !from_.valid())
			return 0.;
		return side()->b_diff(_out._s_p, from_._out._s_p);
	}
	bool current() const 
	{ 
		if(!valid())
			return false;
		if(_out.current())
			return true;
		return _in.current() && !_in.exceeded(); 
	}
	bool working() const { return valid() && _out.working(); }
	bool exceeded() const { return valid() && _out.exceeded(); }

	void model_send(const std::string& comment_, const ACE_Time_Value& t_, bool out_file_, const std::string& key_, long in_sub_id_ = 0) const;

	LFMarketModel* _l;
	LFSlope _in;
	LFPeak _out;
	LFPeak _prev;

	typedef Z::smart_ptr<LFWave> WavePtr;
	typedef std::list<WavePtr> WaveList;
	WavePtr _next;
	long	_wave_id;

	void new_peak_i(const ACE_Time_Value& t_, double p_, double eng_, bool same_peak_ = true);

};

typedef Z::smart_ptr<LFWave> LFWavePtr;
typedef std::list<LFWavePtr> LFWaveList;

//======================================================================
#endif /*LF_WAVE_H*/
