/*
*
*	© Copyright 2003 - 2016 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "leaf/LFData.h"
#include "leaf/Version.h"
#include <iostream>

int main(int argc, char** argv)
{
	try {
		Z::init(argc, argv);

		TCFiler::instance()->set_mode(ios::app);
		std::string range = Z::get_setting("LFPosition:range", "20160216:20160216");
		TCBusinessDateRange t_range(range);

		std::string bmts_dir = Z::get_setting("LFPosition:dir", "P:");
		if (bmts_dir.length() == 0)
			return 0;

		typedef std::map<TCDate, LFDailyPosition> PositionMap;
		PositionMap pm;

		const TCDateVec& rv = t_range.vec();
		for (size_t i = 0; i < rv.size() && tc_b_date->is_business(rv[i]); ++i)
		{
			std::string pos_file = bmts_dir + "/" + rv[i].as_string() + "_Position.cfg";
			LFDailyPosition& dp = pm[rv[i]];
			dp.restore_position(rv[i],pos_file);
		}
		BMPointWriter<LFDailyPosition>	_daily_pos_writer("DAILY_POS_" + std::string(LEAF_VERSION));
		
		for (PositionMap::const_iterator i = pm.begin(); i != pm.end(); ++i)
		{
			auto_ptr<LFDailyPosition> pd(new LFDailyPosition(i->second));
			_daily_pos_writer.send(pd.release());
		}
	}
	catch (Z::Exception& e)
	{
		std::cout << e << std::endl;
		return -1;
	}

	Z::fini();
	return 0;
}