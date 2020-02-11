/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "leaf/LFMarketState.h"

//======================================================================
int main(int argc, char* argv[])
{
	
	try {
		Z::init(argc, argv);
		
		BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_MIXER;
		BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_MIXER;

		LFMarketState::instance()->open();

		BM_MIXER->subscribe<LFDepthData>(*LFMarketState::instance(), Z::get_setting("LFStrategy:leader"));
		BM_MIXER->subscribe<LFTradeData>(*LFMarketState::instance(), Z::get_setting("LFStrategy:leader"));
		BM_MIXER->subscribe<LFDepthData>(*LFMarketState::instance(), Z::get_setting("LFStrategy:follower"));
		BM_MIXER->subscribe<LFTradeData>(*LFMarketState::instance(), Z::get_setting("LFStrategy:follower"));

		BM_MIXER->start(TCBusinessTimeRange(Z::get_setting("md_publisher:date_range")), 0, Z::get_setting_integer("md_publisher:in_run"));
		BM_MIXER->wait();
		BM_MIXER->stop(false);

		BM_MIXER->unsubscribe<LFDepthData>();
		BM_MIXER->unsubscribe<LFTradeData>();
		LFMarketState::instance()->close();

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

