/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMarketState.h"

//======================================================================
LFInstrumentState::LFInstrumentState():
Z::ActiveQueue(1, "LFMarketState"),
_run(-1) //life
{
	_data_timeout.set(TCSettings::get_double("BMTS:refresh_depth", 0.5));
	_bbo = TCSettings::get_bool("BMTS:LFDepthData:bbo");
	register_handler(this, &LFInstrumentState::process_depth);
	register_handler(this, &LFInstrumentState::process_trade);
	register_handler(this, &LFInstrumentState::process_vwap);

	if( BMTS<LFDepthData>::instance()->_out_file == true ||
		BMTS<LFTradeData>::instance()->_out_file == true ||
		BMTS<LFVWAPData>::instance()->_out_file == true)
	{
		_run = 0; // make data file
	}
}
//======================================================================
LFInstrumentState::~LFInstrumentState()
{
	unregister_handler<LFDepthData>();
	unregister_handler<LFTradeData>();
	unregister_handler<LFVWAPData>();
}
ACE_Time_Value* LFInstrumentState::thr_timeout() 
{ 
	if(_run == 0)
		return 0;

	_tv = ACE_OS::gettimeofday() + _data_timeout;
	return &_tv;
}
int LFInstrumentState::thr_svc_timeout() 
{
	if(_depth.get() != 0)
	{
		_depth->_t = TCTimestamp::stamp();
		publish_depth(); 
	}
	return 0;
}
//======================================================================
void LFInstrumentState::process_depth(LFDepthData* d_)
{
	LFDepthDataPtr d(d_);
	d->_run = _run; // life

	if(_bbo)
	{
		
		if(_run == 0) //reading from file
		{
			if(!_t_trading.is_trading(&d->get_t()))
				return;
			if(_depth.get() != 0)
			{
				ACE_UINT64 d_sec = 0; d->get_t().msec(d_sec);
				ACE_UINT64 e_sec = 0; _depth->get_t().msec(e_sec);		
				ACE_UINT64 interval  = (d_sec - e_sec)/_data_timeout.msec();

				for (int i = 0; i < interval; i++)
				{
					_depth->_t += _data_timeout;
					publish_depth();
				}
			}
		}
		if(	_depth.get() == 0 || 
			 /*
			 d_->_ask[0] != _depth->_ask[0] || 
			 d_->_bid[0] != _depth->_bid[0]*/
			 TCComparePrice::diff(d_->_ask[0]._p, _depth->_ask[0]._p) != 0 ||
			 TCComparePrice::diff(d_->_bid[0]._p, _depth->_bid[0]._p) != 0
			 )		
		{
			for(int i = 1; i < LFDepthData::DEPTH_SIZE; ++i)
			{
				d_->_ask[i].reset();
				d_->_bid[i].reset();
			}
			_depth = d;
			publish_depth();
		}
	}
	else
	{
		_depth = d;
		publish_depth();
	}
}
//======================================================================
void LFInstrumentState::process_trade(LFTradeData* d_)
{
	LFTradeDataPtr d(d_);
	d->_run = _run; // life

//	if(_run == 0 && !_t_trading.is_trading(&d->get_t()))
//			return;
	
	
	TC_pqp& pvol = _v_map[d->_trade.p_p()];
	pvol._q += d->_trade._q;
	d->_p_vol = pvol._q;
	
	_trade = d;
	publish_trade();
}
//======================================================================
void LFInstrumentState::process_vwap(LFVWAPData* d_)
{
	LFVWAPDataPtr d(d_);
	d->_run = _run; // life

	_vwap = d;
	publish_vwap();
}
//======================================================================
void LFInstrumentState::publish_depth()
{
	if(_run == -1)
		Z::publish_event(*_depth, _depth->get_key());
	else
	{
		BMModel::instance()->send(new LFDepthData(*_depth));
	}
}
//======================================================================
void LFInstrumentState::publish_trade()
{
	if(_run == -1)
		Z::publish_event(*_trade, _trade->get_key());
	else
		BMModel::instance()->send(new LFTradeData(*_trade));
}
//======================================================================
void LFInstrumentState::publish_vwap()
{
	Z::report_info("publish vwap %s, run = %d", _vwap->as_string().c_str(), _run);
	if (_run == -1)
		Z::publish_event(*_vwap, _vwap->get_key());
	else
		BMModel::instance()->send(new LFVWAPData(*_vwap));
}
//======================================================================

