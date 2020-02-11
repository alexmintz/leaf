/*
*
*	© Copyright 2003-2005 by TAF Co. All rights reserved.
*
*/

#ifndef _P_RTPOS_VIEW_H_
#define _P_RTPOS_VIEW_H_

#include "znet/ZNet.h"
#include "znet/BMStrategy.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "Resource.h"
#include "leaf/LFData.h"

//======================================================================
class Cp_RTPosView : public ETSLayoutFormView
{
public:

	virtual ~Cp_RTPosView();
	virtual void OnInitialUpdate();

	class Ctrl : public TC_DataListCtrl<LFPositionData, std::string>
	{
	public:
		Ctrl();
		bool process_run();
		bool change_run();
		void process_rtpos(LFPositionDataPtr*);
		void process_data(LFDepthDataPtr*);

		virtual std::string filter(std::string& rec_) const { return rec_; } 

	private:
		typedef TC_DataListCtrl<LFPositionData, std::string>		BASE;
		typedef BASE::Record Record;
		
		virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._strategy_key+r_._instr; return true; }

		int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
		virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;

		typedef std::map<std::string, BASE::R_Ptr>	ByStrategyMap;
		typedef std::map<std::string, ByStrategyMap>	ByInstrMap;
		ByInstrMap _p_map;
		typedef std::map<std::string, LFDepthDataPtr>	DepthByInstrMap;
		DepthByInstrMap _d_map;
		
		auto_ptr<LFStrategyRequest>	_req;
	};
	
protected: // create from serialization only
	Cp_RTPosView();
	DECLARE_DYNCREATE(Cp_RTPosView)

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void update_layout();

	Ctrl* ctrl() {return (Ctrl*)_ctrl.get(); }

	DECLARE_MESSAGE_MAP()

private:
	auto_ptr<Ctrl>				_ctrl;
	auto_ptr<BMStrategy>		_s;
};

#endif // _P_RTPOS_VIEW_H_
