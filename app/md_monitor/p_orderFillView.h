/*
*
*	© Copyright 2003-2005 by TAF Co. All rights reserved.
*
*/

#ifndef _P_ORDER_FILL_VIEW_H_
#define _P_ORDER_FILL_VIEW_H_

#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "Resource.h"
#include "znet/BMStrategy.h"
#include "leaf/LFData.h"

//======================================================================
class Cp_OrderFillView : public ETSLayoutFormView
{
public:	
	virtual ~Cp_OrderFillView();
	virtual void OnInitialUpdate();

protected: // create from serialization only
	Cp_OrderFillView(UINT nID = IDD_REPORT);
	DECLARE_DYNCREATE(Cp_OrderFillView)

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void update_layout();

	class Ctrl : public TC_DataListCtrl<LFOrderFill, std::string>
	{
		public:
			Ctrl();
			void process_order(LFOrderFillPtr*);
			bool process_run();
			bool change_run();
			virtual std::string filter(std::string& rec_) const { return rec_; } 
		private:
			typedef TC_DataListCtrl<LFOrderFill, std::string>		BASE;
			typedef BASE::Record Record;
			
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._fill_id; return true; }

			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
			virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;

			BASE::R_Vec r_vec;
	};

	Ctrl* ctrl() {return (Ctrl*)_ctrl.get(); }

	auto_ptr<Ctrl>			_ctrl;
	auto_ptr<BMStrategy>	_s;
	DECLARE_MESSAGE_MAP()
};

#endif 