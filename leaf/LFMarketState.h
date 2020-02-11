/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MARKET_STATE_H
#define LF_MARKET_STATE_H

#include "znet/ZNet.h"
#include "leaf/LEAF_Export.h"
#include "leaf/LFMarketData.h"
#include "znet/TCTradingTime.h"
#include <map>

//======================================================================
class LEAF_Export LFInstrumentState: public Z::ActiveQueue
{
public:
	LFInstrumentState();
	~LFInstrumentState();

	void process_depth(LFDepthData* d_);
	void process_trade(LFTradeData* d_);
	void process_vwap(LFVWAPData* d_);

protected:
	void publish_depth();
	void publish_trade();
	void publish_vwap();

	virtual ACE_Time_Value* thr_timeout();
	virtual int thr_svc_timeout();

private:
	LFDepthDataPtr _depth;
	LFTradeDataPtr _trade;
	LFVWAPDataPtr  _vwap;
	typedef std::map<double, TC_pqp> VolumeMap;
	VolumeMap _v_map;
	bool	_bbo;
	long	_run;

	ACE_Time_Value	_data_timeout;
	ACE_Time_Value _tv;
	TCTradingTime _t_trading;
};

typedef Z::smart_ptr<LFInstrumentState> LFInstrumentStatePtr;

//======================================================================
class LEAF_Export LFMarketState : public Z::ActiveQueue
{
public:
	LFMarketState():
	  Z::ActiveQueue(1, "LFMarketState")
	{	
		register_handler(this, &LFMarketState::process_depth);
		register_handler(this, &LFMarketState::process_trade);
		register_handler(this, &LFMarketState::process_vwap);
	}
	~LFMarketState()
	{
		unregister_handler<LFDepthData>();
		unregister_handler<LFTradeData>();
		unregister_handler<LFVWAPData>();
	}
	int close(bool force_ = true)
	{
		for(InstrumentMap::iterator it = _i_map.begin(); it != _i_map.end(); ++it)
			(*it).second->close();
		return Z::ActiveQueue::close(force_);
	}
	static LFMarketState* instance()
	{
		return ACE_Singleton <LFMarketState, ACE_Thread_Mutex>::instance();
	}
	void process_depth(LFDepthData* d_)
	{
		LFInstrumentStatePtr& s = _i_map[d_->_cqg_InstrumentID];
		if(s.get() == 0)
		{
			s.reset(new LFInstrumentState);
			s->open();
		}
		s->send(d_);
	}
	void process_trade(LFTradeData* d_)
	{
		LFInstrumentStatePtr& s = _i_map[d_->_cqg_InstrumentID];
		if(s.get() == 0)
		{
			s.reset(new LFInstrumentState);
			s->open();
		}
		s->send(d_);
	}
	void process_vwap(LFVWAPData* d_)
	{
		LFInstrumentStatePtr& s = _i_map[d_->_cqg_InstrumentID];
		if (s.get() == 0)
		{
			s.reset(new LFInstrumentState);
			s->open();
		}
		s->send(d_);
	}
protected:
	typedef std::map<long, LFInstrumentStatePtr> InstrumentMap;
	InstrumentMap _i_map;
};
//======================================================================
#endif /*LF_MARKET_STATE_H*/
