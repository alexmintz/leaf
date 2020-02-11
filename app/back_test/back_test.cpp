/*
*
*	� Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "leaf/LFStrategy.h"
#include "leaf/LFData.h"
#include "leaf/LFRunSettings.h"

class LFBackTest : public Z::UserComponent
{
public:
	virtual ~LFBackTest() {}

	void run(LFRunEvent* d_)
	{
		Z::report_info("run ----->>>>>>>>>>>>>");
		
		d_->_run_data.set();

		std::string day_t = Z::get_setting("LFModel:date");
		std::string under = Z::get_setting("LFStrategy:follower_underlying");
		std::string follower = LFSecurityMaster::instance()->get_instr(under, day_t);
		std::string dxe = LFSecurityMaster::instance()->get_dxe(day_t);

		Z::add_setting("LFStrategy:leader", dxe);
		Z::add_setting("LFStrategy:follower", follower);

		LFSecurityMaster::instance()->strategy_id(Z::get_setting("LFStrategy:follower"));

		Z::report_info("run -- constructing strategy...");
		LFStrategy s(Z::get_setting("LFStrategy:leader"), Z::get_setting("LFStrategy:follower"), d_);

		RunSettings::instance()->init(); // all false
		RunSettings::instance()->set_backtest(true);
		RunSettings::instance()->set_trading(true);
		
		TCDate cut_off(Z::get_setting("LFModel:cutoff_date"));
		if(cut_off.empty())
			cut_off = tc_b_date->prev_business_date(TCDateTime::now().get_date());
		
		TCDate cur_date(day_t);
		if (cur_date.get_ymd() > cut_off.get_ymd())
		{
			BMTS<LFDepthData>::instance()->_table = "depth";
			BMTS<LFTradeData>::instance()->_table = "trade";
		}
		else
		{
			BMTS<LFDepthData>::instance()->_table = "depth_backtest";
			BMTS<LFTradeData>::instance()->_table = "trade_backtest";
		}

		BMTS<LFInfo_Position>::instance()->_in = BMTS_Properties::IN_NONE;
		BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFInfo_Position>::instance()->_event_type_name;
		BMTS<LFInfo_Position>::instance()->_out_file = true;

		std::string r_dir = Z::get_setting("LFStrategy:LFInfo_Position:root_dir", "");

		if (Z::get_setting_bool("LFStrategy:LFInfo_Position:in", false))
		{
			BMTS<LFInfo_Position>::instance()->_in = BMTS_Properties::IN_MIXER;
			BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFDepthData>::instance()->_worker_name;
			BMTS<LFInfo_Position>::instance()->_out_file = false;
			if (!r_dir.empty())
				BMTS<LFInfo_Position>::instance()->_root_dir = r_dir;
		}
		else if (Z::get_setting_bool("LFStrategy:LFInfo_Position:out", false))
		{
			BMTS<LFInfo_Position>::instance()->_out_file = true;
			BMTS<LFInfo_Position>::instance()->_use_root_dir = true;
			if (!r_dir.empty())
				BMTS<LFInfo_Position>::instance()->_root_dir = r_dir;
		}

		Z::report_info("run -- opening strategy...");
		s.open();
		Z::report_info("run -- opening model...");
		BMModel::instance()->open();

		d_->_run_stats = LFRunStats();
		d_->_run_stats._c_data._open_max = -1;

		std::string start_t = Z::get_setting("LFModel:start_time",	"09:15:00");
		std::string stop_t = Z::get_setting("LFModel:stop_time", "14:15:00");
		TCBusinessTimeRange btr(TCDateTime(TCDate(day_t), TCTime(start_t, '\0')).get_ace_time(), 
									 TCDateTime(TCDate(day_t), TCTime(stop_t, '\0')).get_ace_time());

		Z::report_info("run -- starting mixer...");
		BM_MIXER->start(btr, 0, -1, Z::get_setting_bool("back_test:out_file", false));
		Z::report_info("run -- waiting on mixer...");
		BM_MIXER->wait();
		d_->_run_stats._run = BM_MIXER->run_id();
		Z::report_info("run -- stopping mixer...");
		BM_MIXER->stop(false);
		
		Z::report_info("run -- sleeping 2 sec ...");
		ACE_OS::sleep(Z::get_setting_integer("back_test:sleep_before_close", 2));
		Z::report_info("run -- closing model...");
		BMModel::instance()->close(false);
		Z::report_info("run -- closing strategy...");
		s.close(false);
		Z::report_info("run <<<<<<<<<<<----- ");
	}
	virtual void init()
	{
		Z::report_info("backtest server component init.");
		Z::add_setting("BMTS:root_dir", Z::get_setting("BMTS:dir", Z::get_setting("LOG:dir", ".")));
		BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_MIXER;
		BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_MIXER;
		bool in_file = Z::get_setting_bool("back_test:out_file", false);
		BMTS<LFStrategyData>::instance()->_out_file = in_file;
		BMTS<LFNewOrder>::instance()->_out_file = in_file;
		BMTS<LFOrderUpdate>::instance()->_out_file = in_file;
		BMTS<LFOrderFill>::instance()->_out_file = in_file;
		BMTS<LFPositionData>::instance()->_out_file = in_file;
		BMTS<LFTeaser>::instance()->_out_file = in_file;
		BMTS<LFMatch>::instance()->_out_file = in_file;
		BMTS<LFTradeStats>::instance()->_out_file = in_file;
		BMTS<LFCluster>::instance()->_out_file = in_file;
		BMTS<LFBox>::instance()->_out_file = in_file;
		BMTS<LFDataPoint>::instance()->_out_file = in_file;
		BMTS<LFPoint>::instance()->_out_file = in_file;
		BMTS<LFEntry>::instance()->_out_file = in_file;
		BMTS<LFRunStats>::instance()->_out_file = true;
		BMTS<LFRunStats>::instance()->_run_disabled = true;
		Z::register_function(this, &LFBackTest::run);
	}
	virtual void fini()
	{
		Z::report_info("backtest server component fini.");
		Z::unregister_function<LFRunEvent>();
	}
};
//======================================================================
int main(int argc, char* argv[])
{
	try {
		LFBackTest comp;
		Z::init(argc, argv, &comp);
		Z::wait();
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

