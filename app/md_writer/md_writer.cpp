/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "znet/TCActiveQueue.h"
#include "znet/BMModel.h"
#include "leaf/LFMarketData.h"

//======================================================================
//class LFDataSubscriber : public TCActiveQueue
//{
//public:
//	LFDataSubscriber() 
//		: TCActiveQueue(1, "LFDataSubscriber"),
//		_depth_dir(Z::get_setting("BMTS:dir", Z::get_setting("LOG:dir", ".")) + "/depth"),
//		_trade_dir(Z::get_setting("BMTS:dir", Z::get_setting("LOG:dir", ".")) + "/trade")
//
//	{}
//	virtual ~LFDataSubscriber() {}
//
//	int open()
//	{
//		TCFiler::instance()->set_mode(ios::app);
//		subscribe_handler_i(this, &LFDataSubscriber::process_depth);
//		subscribe_handler_i(this, &LFDataSubscriber::process_trade);
//
//		return TCActiveQueue::open();
//	}
//
//	virtual void thr_close() 
//	{ 
//		unsubscribe_handler_i<LFDepthData>();
//		unsubscribe_handler_i<LFTradeData>();
//		_depth_writers.clear();
//		_trade_writers.clear();
//	}
//
//	void process_depth(LFDepthData* d_)
//	{
//		auto_ptr<LFDepthData> d(d_);
////		Z::report_info("DOM %s", d->as_string().c_str());
//		DepthWriterPtr& w = _depth_writers[d->_instr_key];
//		if(w.get() == 0)
//			w.reset(new DepthWriter(_depth_dir, d->_instr_key));
//		w->send(d.release());
//	}
//	void process_trade(LFTradeData* d_)
//	{
//		auto_ptr<LFTradeData> d(d_);
////		Z::report_info("TRADE %s", d->as_string().c_str());
//		TradeWriterPtr& w = _trade_writers[d->_instr_key];
//		if(w.get() == 0)
//			w.reset(new TradeWriter(_trade_dir, d->_instr_key));
//		w->send(d.release());
//	}
//
//private:
//	std::string _depth_dir;
//	std::string _trade_dir;
//	typedef BMPointWriter<LFDepthData> DepthWriter;
//	typedef Z::smart_ptr<DepthWriter> DepthWriterPtr;
//	typedef std::map<std::string, DepthWriterPtr> DepthWriterMap;
//	DepthWriterMap _depth_writers;
//	typedef BMPointWriter<LFTradeData> TradeWriter;
//	typedef Z::smart_ptr<TradeWriter> TradeWriterPtr;
//	typedef std::map<std::string, TradeWriterPtr> TradeWriterMap;
//	TradeWriterMap _trade_writers;
//
//};

//======================================================================
int main(int argc, char* argv[])
{
	
	try {
		Z::init(argc, argv);
		TCFiler::instance()->set_mode(ios::app);

		BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		BMTS<LFDepthData>::instance()->_out_file = true;
		BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		BMTS<LFTradeData>::instance()->_out_file = true;
		BMTS<LFVWAPData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		BMTS<LFVWAPData>::instance()->_out_file = true;
		BMTS<LFVWAPData>::instance()->_fios = std::ios_base::trunc;
		BMTS<LFVWAPData>::instance()->_close_after_write = true;

		BMModel::instance()->subscribe<LFDepthData>();
		BMModel::instance()->subscribe<LFTradeData>();
		BMModel::instance()->subscribe<LFVWAPData>();

		BMModel::instance()->open();
		Z::wait();
		BMModel::instance()->close();

		Z::fini();
	}
	catch(Z::Exception& e)
	{
		std::cout << e << std::endl;
		return -1;
	}
	return 0;
}

//======================================================================

