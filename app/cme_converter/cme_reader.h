/*
*
*	� Copyright 2012 by TAF Co. All rights reserved.
*
*/
#ifndef CME_READER_H_
#define CME_READER_H_

#include "znet/ZNet.h"
#include "znet/TCActiveQueue.h"
#include "znet/BMModel.h"
#include "znet/BMReader.h"
#include "leaf/LFData.h"


//======================================================================

class CMEReader: public BMSequenceReader
{
public:
	CMEReader(const std::string& dir_, bool trades_only_);
	~CMEReader();
	void run();

private:
	void init();
	virtual void process_hdr(ACE_Message_Block* mb_);
	virtual void process_eof(bool full_); 
	virtual void process_rec(ACE_Message_Block* mb_); 

	void process(ACE_Message_Block* mb_);
	void process_group(int action_, 
		const ACE_Time_Value& tv_, 
		const std::string& sec_,
		std::map<int, std::string>& m_, 
		std::set<std::string>& depth_report_,
		std::set<std::string>& trade_report_,
		size_t group_left_);

	void process_trade_group(int action_, const std::string& sec_, const ACE_Time_Value& tv_, std::map<int, std::string>& m_, std::set<std::string>& report_);
	void process_quote_group(int action_, const std::string& sec_, int side_, int level_, const ACE_Time_Value& tv_, std::map<int, std::string>& m_, std::set<std::string>& report_, size_t group_left_);


	ACE_Time_Value get_ace_time(const std::string& timestamp_, bool& same_day_);

	typedef std::pair < LFDepthData, LFDepthDataReader > DepthPair;
	typedef std::map<std::string, DepthPair> DepthMap;
	typedef std::map<std::string, LFTradeData> TradeMap;
	typedef std::map<double, double>		PQPMap;
	typedef std::map<std::string, PQPMap>	VWAPMap;

	DepthMap					_dm;
	TradeMap					_tm;
	VWAPMap						_vm;

	Z::ActiveQueue				_worker;
	std::vector<std::string>	_fvec;

	size_t						_f_ind;	

	bool						_cme_fix_out_file;
	bool						_report_error_only;
	CMEDataPoint				_cme_data;
	TCBusinessTimeRange			_back_trace;
	bool						_trades_only;

	bool						_repeat_bbo;
	bool						_vwap_over;
};


#endif 


 