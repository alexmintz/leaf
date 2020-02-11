/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "leaf/LFStrategy.h"


//======================================================================
int main(int argc, char* argv[])
{
	
	try {
		Z::init(argc, argv);
		
		BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		BMTS<LFStrategyData>::instance()->_out_file = true;

		//BMTS<LFOrderStatusData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
		//BMTS<LFOrderRequestData>::instance()->_out = BMTS_Properties::OUT_SENDER;

		LFStrategy s(Z::get_setting("LFStrategy:leader"), Z::get_setting("LFStrategy:follower"));
		s.open();
		BMModel::instance()->open();
		Z::wait();
		BMModel::instance()->close(false);
		s.close(false);
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

