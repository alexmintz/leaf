/*
*
*	© Copyright 2003-2012 by TAF Co. All rights reserved.
*
*/

#ifndef _RUN_DLG_H_
#define _RUN_DLG_H_

#pragma once

#include "znet/TCDefs.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "resource.h"


class run_Dlg : public ETSLayoutDialog
{
	DECLARE_DYNCREATE(run_Dlg)

public:	
	run_Dlg(UINT nID = IDD_RUN_DIALOG);
	virtual ~run_Dlg();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
	virtual void update_layout();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	BMMixer::RunInfo* get_info() { return _info;}

protected:
	class Ctrl : public TC_DataListCtrl<BMMixer::RunInfo, long>
	{
		public:
			Ctrl();
			virtual std::string filter(std::string& rec_) const { return rec_; } 
			void fill_run();

		private:
			typedef TC_DataListCtrl<BMMixer::RunInfo, long>		BASE;
			typedef BASE::Record Record;
			
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._run; return true; }
			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
	};

	DECLARE_MESSAGE_MAP()
	
	Ctrl* ctrl() {return (Ctrl*)_ctrl.get(); }
	auto_ptr<Ctrl>			_ctrl;
	BMMixer::RunInfo*		_info;
};

#endif //_RUN_DLG_H_