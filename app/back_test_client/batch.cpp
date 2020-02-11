/*
*
*	© Copyright 2003-2012 by TAF Co. All rights reserved.
*
*/

#include "znet/ZNet.h"
#include "leaf/LFData.h"
#include <iostream>

//======================================================================
struct RunReply : public Z::ReplyFunctor<LFRunEvent>
{
	RunReply():_writer("BATCH_STATS"){}

	void operator()(LFRunEvent* result_, const Z::Param& param_)
	{
		auto_ptr<LFRunEvent> r(result_);
		std::cout << r->as_string() << std::endl;
		_writer.send(r.release());
	};

	Z::ReplyFunctor<LFRunEvent>* clone() const { return new RunReply(*this); }

	BMPointWriter<LFRunEvent> _writer;
};
/*
void callback(LFRunEvent* result_)
{
	auto_ptr<LFRunEvent> r(result_);
	std::cout << r->as_string() << std::endl;
	_writer.send(r.release()));
};
*/
int main(int argc, char** argv)
{
	try {
		Z::init(argc, argv);
		// Add user options
		Z::add_command_line_option("engines",
			"Number of available zEngines to wait before start.");
		
		TCFiler::instance()->set_mode(ios::app);
		// Obtain user inputs
		int engines = Z::get_setting_integer("COMMAND:engines", 1);

		LFRunEvent rd;
		std::vector<LFRunEvent*> rdv;
		rd.init_backtest(rdv);
		RunReply rr;
		Z::register_reply_handler(rr);
		for(size_t i = 0; i < rdv.size(); ++i)
		{
			Z::report_debug("zcall -- LFRunData[%d]", i);
			Z::z_call(rdv[i]);
		}
		Z::process_all();
	}
	catch(Z::Exception& e)
	{
		std::cout << e << std::endl;
		return -1;
	}
	Z::fini();
	return 0;
}
