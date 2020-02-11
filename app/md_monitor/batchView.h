/*
*
*	© Copyright 2003-2012 by TAF Co. All rights reserved.
*
*/

#ifndef _Cp_BatchView_H_
#define _Cp_BatchView_H_

#pragma once

#include "znet/TCDefs.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "leaf/LFData.h"
#include "Resource.h"
#include <map>

typedef std::pair<long, double> LFDayPnL;
typedef std::vector<LFDayPnL> LFDayPnLVec;

class Cp_BatchView : public ETSLayoutFormView
{
public:	
	virtual ~Cp_BatchView();
	virtual void OnInitialUpdate();

	afx_msg void OnBnClickedBatch();
	void process_result(LFRunEvent* result_, const Z::Param& param_);

protected:
	Cp_BatchView();
	DECLARE_DYNCREATE(Cp_BatchView)

	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
	virtual void update_layout();
	
	class Ctrl1 : public TC_DataListCtrl<LFTradeStats, std::string>
	{
			typedef TC_DataListCtrl<LFTradeStats, std::string>		BASE;
			typedef BASE::Record Record;
	public:
			Ctrl1();
			virtual std::string filter(std::string& rec_) const { return rec_; } 
			void process_result(LFTradeStats* result_);
			void init();
//			BASE::R_Vec r_vec;
			BASE::R_Ptr _tot;
		private:
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._s_t_str; return true; }
			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
			virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;
		
	};

	class Ctrl : public TC_DataListCtrl<LFRunEvent, long>
	{
			typedef TC_DataListCtrl<LFRunEvent, long>		BASE;
			typedef BASE::Record Record;
		public:
			Ctrl();
			virtual std::string filter(std::string& rec_) const { return rec_; } 
			void process_result(LFRunEvent* result_, const Z::Param& param_, bool last_ = false);
			void init(Ctrl1* matches_);
			//BASE::R_Vec r_vec;
			BASE::R_Ptr _tot;

		private:
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._run_stats._run; return true; }
			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
			virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;
			std::string sharpe(const LFTradeStats& _p_data);
			std::string drawdown();

			void update_totals(LFTradeStats& tdata_, const LFTradeStats& pdata_);
			void update_sub_totals(const std::string& sub_date_, const LFTradeStats& pdata_);

			typedef std::map<std::string, LFRunEventPtr> LFRunEventPtrMap;

			LFRunEventPtrMap	_sub_tot;
			void make_tot_dates(std::string& month_, std::string& quarter_, std::string& year_, const std::string& date_);

			BMPointWriter<LFRunEvent>	_writer;

			BMPointWriter<LFTotalEvent>	_writer_tot;
			typedef Z::smart_ptr<BMPointWriter<LFMatch> > SlopeWriterPtr;
			typedef std::map<std::string, SlopeWriterPtr> SlopeWriterMap;
			SlopeWriterMap _slope_writers;
			BMPointWriter<LFTradeStats>	_trades_writer;
			BMPointWriter<LFTradeStats>	_trades_writer_tot;
			size_t _last;
			
			double _risk_free_rate;
			double _initial_margin;
			std::vector<double> _daily_returns_v;
			
			LFDayPnLVec _daily_pnl;

			Ctrl1*	_trades;
			std::string _E_match;
	};



	DECLARE_MESSAGE_MAP()
	
//	Ctrl* ctrl() {return (Ctrl*)_ctrl.get(); }
	auto_ptr<Ctrl>	_ctrl;
	auto_ptr<Ctrl1>	_ctrl1;
	UINT_PTR		_timer;
};

#endif //_Cp_BatchView_H_