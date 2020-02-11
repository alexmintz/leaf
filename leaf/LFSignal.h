/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_SIGNAL_H
#define LF_SIGNAL_H

#include "znet/ZNet.h"
#include "leaf/LFData.h"

//======================================================================
class LFStrategy;

class LEAF_Export LFSignal
{
public:
	LFSignal(LFStrategy& strat_, const std::string& type_, const std::string& market_ = "");
	virtual ~LFSignal();

	const std::string& get_name() const { return _name; }
	const std::string& get_type() const { return _type; }
	const std::string& get_market() const { return _market; }

	bool is_configured_on() const { return _on; }
	bool is_trading_on() const { return _trading_on; }

	bool is_position_owner() const { return _position_owner; }

	void set_position_owner(double pnl_);
	void reset_position_owner(double pnl_);

	virtual void update_momentum_data() {}
	virtual void update() = 0;
	virtual void close();

	const LFTeaserPair& get_teaser() const { return _teaser; }
	LFTeaserPair& get_teaser() { return _teaser; }
	const LFMatchPtrList& get_history() const { return _history; }
	
	const LFPeakId&	get_active_peak() const { return _active_peak_id ; }
	const LFCutLoss& get_cut_loss() const { return _cut_loss; }
	const LF::PositionAction& get_action() const { return _action; }

	virtual void process_request(const LFStrategyRequest& r_) = 0;

protected:
	LFStrategy&		_strat;
	std::string				_name;
	std::string				_type;
	std::string				_market;
	std::string				_resource_base;
	bool					_on;
	bool					_trading_on;
	bool					_position_owner;
	double					_start_pnl;
	double					_pre_pnl;
	LFTeaserPair			_teaser;
	LFMatchPtrList			_history;
	LFPeakId				_active_peak_id;
	LFCutLoss				_cut_loss;
	LF::PositionAction		_action;

	virtual void reconcile_pos() {}

};
typedef Z::smart_ptr<LFSignal> LFSignalPtr;

//======================================================================
class LEAF_Export LFSignal_Market : public LFSignal
{
public:
	LFSignal_Market(LFStrategy& strat_, LFMomentumData& d_, const std::string& type_); 
	virtual ~LFSignal_Market();

	virtual void update();
	virtual void process_request(const LFStrategyRequest& r_) {}
	virtual void update_momentum_data();

	virtual void close();

protected:
	LFMomentumData& _d;
	TCBusinessTimeRange _trace_r;

	double	_cut_loss_unrealized;
	bool	_cut_loss_suspend;
	double	_cut_loss_total;

	double		_box_report_profit;
	bool		_ematches_out_file;
	bool		_xmatch_out_file;
	double		_box_X_Profit_pct;
	double		_box_X_cut_loss_ticks;


	LFBox	_bias_bc;	// Action
	LFBox	_bias_bx;	// Trigger

	LFMatchPtrList _bias_enter_matches;
	LFEntry	_entry;

	bool check_cut_loss();


	virtual const LFSide* cut_loss();

	virtual void trading() = 0;

	void trade(const LFSide* t_side_, double q_, const std::string& comment_);

	void report_trigger(const LFBox& b_, const std::string& comment_, const std::string& tr_comment_, LF::StrategyActionType action_);
	void report_action(const LFBox& b_, const std::string& comment_);

	virtual void reconcile_pos();
	void exit_profit_loss();

	void close_bias_matches(const LFSide* side_);
	void close_bias_match(LFMatch& m_);
	void update_matches();
	bool update_match(LFMatch& match_);

	bool trade_exit(const LFSide* pos_side_, double pos_pct_, const std::string& comment_);

};

//======================================================================
typedef std::map<std::string, LFSignalPtr> LFSignalMap;
typedef std::list<LFSignalPtr> LFSignalList;
//======================================================================
#endif /*LF_SIGNAL_H*/
