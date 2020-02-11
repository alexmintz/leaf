/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#include "bbo_filter.h"
#include "znet/TCIdGenerator.h"

void LFReaderQueueFilter::process_depth(LFDepthData* edp_)
{
	auto_ptr<LFDepthData> edp(edp_);

	if(	TCComparePrice::diff(edp->_ask[0]._p, _prev_rec._ask[0]._p) != 0 ||
		TCComparePrice::diff(edp->_bid[0]._p, _prev_rec._bid[0]._p) != 0)
	{
		_prev_rec = *edp;
		BMModel::instance()->send(edp.release());
	}
}

int main(int argc, char* argv[])
{
	try {
		Z::add_command_line_option("range", "date_time range, e.g 20111201_000000:20120730_160000");
		Z::init(argc, argv);

		std::string date_range = Z::get_setting("COMMAND:range");
		if(!date_range.empty())
			TCSettings::set_resource("BBO_Filter:date_range", date_range);
		else
			date_range = TCSettings::get("BBO_Filter:date_range");

		if(date_range.empty())
			z_throw("bbo_filter -- business date_time range is not set");
		
		TCBusinessTimeRange	btr(date_range);

		BMTS<LFDepthData>::instance()->_out_file = true;
		TCFiler::instance()->set_mode(ios::app);
		BMModel::instance()->subscribe<LFDepthData>();
		BMModel::instance()->open();

		long run = TCIdGenerator::instance()->id("Run");
		LFReaderQueueFilterPtr rp_cle;
		LFReaderQueueFilterPtr rp_dxe;
		LFReaderQueueFilterPtr rp_smp;
		LFReaderQueueFilterPtr rp_brn;
		TCDateVec dv = btr.vec();

		bool process_dxe = TCSettings::get_bool("BBO_Filter:process_dxe", "false");
		bool process_cle = TCSettings::get_bool("BBO_Filter:process_cle", "false");
		bool process_smp = TCSettings::get_bool("BBO_Filter:process_smp", "false");
		bool process_brn  = TCSettings::get_bool("BBO_Filter:process_brn", "false");

		for(size_t i = 0; i < dv.size(); ++i)
		{	
			std::string key_cle = LFSecurityMaster::instance()->get_cle(dv[i]);
			std::string key_dxe = LFSecurityMaster::instance()->get_dxe(dv[i]);
			std::string key_smp = LFSecurityMaster::instance()->get_smp(dv[i]);
			std::string key_brn = LFSecurityMaster::instance()->get_brn(dv[i]);

			TCDateTime dt1(dv[i], TCTime(1,  0, 0));
			TCDateTime dt2(dv[i], TCTime(16, 0, 0));
			TCBusinessTimeRange	range(dt1.get_ace_time(), dt2.get_ace_time());
			
			if(process_cle)
			{
				rp_cle.reset(new LFReaderQueueFilter(key_cle));
				rp_cle->start(range, run);
				rp_cle->wait();
			}
			if(process_dxe)
			{
				rp_dxe.reset(new LFReaderQueueFilter(key_dxe));
				rp_dxe->start(range, run);
				rp_dxe->wait();
			}
			if(process_smp)
			{
				rp_smp.reset(new LFReaderQueueFilter(key_smp));
				rp_smp->start(range, run);
				rp_smp->wait();
			}
			if(process_brn)
			{
				rp_brn.reset(new LFReaderQueueFilter(key_brn));
				rp_brn->start(range, run);
				rp_brn->wait();
			}
		}

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