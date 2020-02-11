/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MATCH_H
#define LF_MATCH_H

#include "znet/ZNet.h"
#include "leaf/LFBox.h"

//======================================================================
//======================================================================
struct LFMomentumData;
//======================================================================
struct LEAF_Export LFMatch : public LFPoint
{
	LFMatch(const std::string& name_ = "", const std::string& key_ = "", bool is_enter_ = false);
	virtual ~LFMatch();

	LEAF_Export friend bool operator<<(ACE_OutputCDR& strm, const LFMatch& d_);
	LEAF_Export friend bool operator>>(ACE_InputCDR& strm, LFMatch& d_);
	///Writes data in csv format
	LEAF_Export friend std::ostream& operator<<(std::ostream& os, const LFMatch& d_);

//	void reset(const LFSide* side_ = 0, const ACE_Time_Value& t_ = ACE_Time_Value::zero, double p_ = 0);
	virtual void reset(const LFSide* side_ = 0, const LFDataPointPtr& dp_ = LFDataPointPtr());

	bool					_is_enter;
	std::string				_m_name;
	LF::MatchPatternMask	_mp_mask;
	LF::MatchMask			_m_mask;
	LF::MatchExecMask		_me_mask;
	double					_q_multiplier;
	double					_qty;

	double	_m_loss_v;
	double	_m_profit_v;
	double	_m_l;
	double	_m_v;
	double	_m_loss_l;
	double	_m_profit_l;

	double	_m_scale;
	double	_res_max_v;
	double	_res_max_l;
	double	_res_min_v;
	double	_res_min_l;
	double	_res_end_v;
	double	_res_end_l;
	bool	_m_success;

	LFBox	_box;
	double	_peak_slope;
	double	_peak_tip;
	double	_peak_v;

	double _peak_vol;
	double _peak_trn;
	double _peak_den;
	double _peak_frq;

	double _m_vol;
	double _m_trn;
	double _m_den;
	double _m_frq;

	double _m_vol_pct;

	double	_exp_v;
	double	_band_v;
	double	_band_f;
	double	_peak_f;
	double	_bounce_f;
	double	_test_f;

	double	_m_b_pct;
	double	_m_ratio;
	double	_m_repeat_charge;

	static const char* header();
};
typedef Z::smart_ptr<LFMatch> LFMatchPtr;
Z_EVENT_DECLARE_MODEL(LEAF, LFMatch)
typedef std::vector<LFMatch> LFMatchVec;
typedef std::list<LFMatchPtr> LFMatchPtrList;

//======================================================================
#endif /*LF_MATCH_H*/
