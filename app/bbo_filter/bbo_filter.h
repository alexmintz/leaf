/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/
#ifndef BBO_FILTER_H_
#define BBO_FILTER_H_


#include "znet/BMReader.h"
#include "leaf/LFMarketData.h"

class LFReaderQueueFilter: public BMReaderQueue
{
public:
	LFReaderQueueFilter(const std::string& tag_)
		: BMReaderQueue(1, BMTS<LFDepthData>::instance(), tag_, new BMEventReader_T<LFDepthData>(*this))
	{
		register_handler(this, &LFReaderQueueFilter::process_depth);
	}
	virtual ~LFReaderQueueFilter()
	{
		unregister_handler<LFDepthData>();
	}
	virtual void process_depth(LFDepthData* edp_); 
private:
	LFDepthData _prev_rec;
};
typedef Z::smart_ptr<LFReaderQueueFilter> LFReaderQueueFilterPtr;
#endif //BBO_FILTER_H_