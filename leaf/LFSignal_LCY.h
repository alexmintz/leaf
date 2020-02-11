/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_SIGNAL_LCY_H
#define LF_SIGNAL_LCY_H

#include "znet/ZNet.h"
#include "leaf/LFSignal.h"

//======================================================================
class LEAF_Export LFSignal_LCY : public LFSignal_Market
{
public:
	LFSignal_LCY(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_ = "LCY");
	virtual ~LFSignal_LCY();

	virtual void trading();

	virtual void process_request(const LFStrategyRequest& r_);

protected:
	long		_E_on_lcy;
	double		_E_on_lcy_min_v;
	double		_E_on_lcy_min_vol;
	double		_E_on_lcy_valid_bounce_cnty;
	double		_X_on_lcy_valid_peak_cnty;

	double		_request_qty;

	LFBox		_lcy_bc;
	LFBox		_lcy_enter;
	LFBox		_lcy_bnc;

	void process_lcy();
	void enter_lcy();
	void exit_lcy();
	void report_lcy_trigger(bool active_);

};
//======================================================================
#endif /*LF_SIGNAL_LCY_H*/
