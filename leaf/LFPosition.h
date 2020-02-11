/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef _LF_POSITION_H_
#define _LF_POSITION_H_

#include "znet/ZNet.h"
#include "znet/BMStrategy.h"
#include "leaf/LFData.h"

//======================================================================
//======================================================================
class LEAF_Export LFPositionImpl
{
public:
	LFPositionImpl(const std::string& instr_, long strategy_id_);
	virtual ~LFPositionImpl();

	virtual void reset();
	virtual void reset(double open_p_, double open_q_,double realized_pnl_);
	LFPositionData& data() { return _d; }

	virtual void set_market_data(const LFDepthData& md_);
	virtual const LFDepthData& get_market_data() { return _md; }

	// returns order id
	virtual long new_order(LF::Side side_, double qty_, LF::OrderType type_ = LF::otMarket, long sub_strategy_id_ = 0, double limit_price_ = 0., double stop_price_ = 0.);
	// 0 - cancel all
	virtual void cancel_order(long order_id_);
	virtual std::string account() {std::ostringstream os;
									os<< _cqg_AccountID;
									return os.str();}

	void set_input_time(const ACE_Time_Value& t_)
	{
		if (_input_time == ACE_Time_Value::zero || _input_time < t_)
			_input_time = t_;
	}


protected:
	LFPositionData	_d;
	LFDepthData		_md;
	long			_cqg_InstrumentID;
	long			_cqg_AccountID;

	LFOrderMap		_orders;

	ACE_Time_Value	_input_time;

	virtual void place_order(LFNewOrder* o_) = 0;
	virtual void book_order(LFNewOrder* o_);
	virtual void cancel_order(LFCancelOrder* c_) = 0;

	void calc_in_progress();

};

//======================================================================
class LEAF_Export LFLifePositionImpl : public LFPositionImpl
{
public:
	LFLifePositionImpl(const std::string& instr_, long strategy_id_, BMStrategy& strategy_);
	virtual ~LFLifePositionImpl();

	//void order_fill(LFOrderFillPtr*);
	void order_update(LFOrderUpdatePtr*);
private:
	BMStrategy& _strategy; // to subscribe order updates and fills

	virtual void place_order(LFNewOrder* o_);
	virtual void cancel_order(LFCancelOrder* c_);
};

//======================================================================
class LEAF_Export LFMockPositionImpl : public LFPositionImpl
{
public:
	LFMockPositionImpl(const std::string& instr_, long strategy_id_);
	virtual ~LFMockPositionImpl();

private:
	virtual void place_order(LFNewOrder* o_);
	virtual void cancel_order(LFCancelOrder* c_);

};
//======================================================================
// Facade object
//======================================================================
class LEAF_Export LFPosition
{
public:
	LFPosition(const std::string& instr_, long strategy_id_);
	LFPosition(const std::string& instr_, long strategy_id_, BMStrategy& strategy_);
	virtual ~LFPosition();

	virtual void reset();
	virtual void reset(double open_p_, double open_q_,double realized_pnl_);
	LFPositionData& data() { return _impl->data(); }

	virtual void set_market_data(const LFDepthData& md_) { _impl->set_market_data(md_); }
	virtual const LFDepthData& get_market_data() { return _impl->get_market_data();  }

	void set_input_time(const ACE_Time_Value& t_) { _impl->set_input_time(t_); }

	// returns order id
	virtual long new_order(LF::Side side_, double qty_, LF::OrderType type_ = LF::otMarket, long sub_strategy_id_ = 0, double limit_price_ = 0., double stop_price_ = 0.)
	{
		return _impl->new_order(side_, qty_, type_, sub_strategy_id_, limit_price_, stop_price_);
	}
	// 0 - cancel all
	virtual void cancel_order(long order_id_)
	{
		return _impl->cancel_order(order_id_);
	}
	virtual std::string account()
	{
		return _impl->account();
	}
private:
	auto_ptr<BMStrategy>	_s;
	auto_ptr<LFPositionImpl> _impl;

};
//======================================================================
#endif // _LF_POSITION_H_
