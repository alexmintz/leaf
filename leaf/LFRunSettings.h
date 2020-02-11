#ifndef _LFRUN_SETTINGS_H_
#define _LFRUN_SETTINGS_H_

#include "znet/ZNet.h"

struct RunSettings
{
	RunSettings()
	{}
	static RunSettings* instance()
	{
		return ACE_Singleton <RunSettings, ACE_Thread_Mutex>::instance();
	}
	void init()
	{
		set_rerun_backtest(false);
		set_rerun_live(false);
		set_live_backtest(false);
		set_live_run(false);
		set_backtest(false);
		set_trading(false);
		set_instance_disabled(false);
	}
	bool get_rerun_backtest()	{ return Z::get_setting_bool("LFStrategy:rerun_backtest", false); }
	bool get_rerun_live()		{ return Z::get_setting_bool("LFStrategy:rerun_live", false); }
	bool get_live_run()			{ return Z::get_setting_bool("LFStrategy:live", false); }
	bool get_live_backtest()	{ return Z::get_setting_bool("LFStrategy:live_backtest", false); }
	bool get_backtest()			{ return Z::get_setting_bool("LFStrategy:backtest", false); }
	bool get_trading()			{ return Z::get_setting_bool("LFStrategy:trading", false); }
	bool get_instance_disabled(){ return Z::get_setting_bool("LFStrategy:instance_disabled", false); }
	
	void set_rerun_backtest(bool b_)	{ Z::add_setting("LFStrategy:rerun_backtest", b_ ? "true" : "false"); }
	void set_rerun_live(bool b_)		{ Z::add_setting("LFStrategy:rerun_live", b_ ? "true" : "false"); }
	void set_live_backtest(bool b_)		{ Z::add_setting("LFStrategy:live_backtest", b_ ? "true" : "false"); }
	void set_live_run(bool b_)			{ Z::add_setting("LFStrategy:live", b_ ? "true" : "false"); }
	void set_backtest(bool b_)			{ Z::add_setting("LFStrategy:backtest", b_ ? "true" : "false"); }
	void set_trading(bool b_)			{ Z::add_setting("LFStrategy:trading", b_ ? "true" : "false"); }
	void set_instance_disabled(bool b_)	{ Z::add_setting("LFStrategy:instance_disabled", b_ ? "true" : "false"); }
};

#endif