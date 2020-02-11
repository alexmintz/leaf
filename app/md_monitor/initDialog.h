/*
*
*	© Copyright 2003-2012 by TAF Co. All rights reserved.
*
*/

#ifndef _INIT_DLG_H_
#define _INIT_DLG_H_

#pragma once

#include "znet/TCDefs.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "resource.h"
#include "leaf/LFData.h"
#include "leaf/LFRunSettings.h"


struct Security
{	
	Security(const std::string& s_):_sec(s_){}
	std::string _sec;
};

class init_Dlg : public CDialog
{
	DECLARE_DYNCREATE(init_Dlg)

public:	
	init_Dlg(UINT nID = IDD_DIALOG_INIT);
	virtual ~init_Dlg();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadio();
//	afx_msg void OnBnClickedUnderlyingRadio();
	afx_msg void OnBnClickedRunDir();
	afx_msg void OnBnClickedSettings();
	afx_msg void OnFollowerPicker();

//	afx_msg void OnBnClickedModel();

protected:
	class Ctrl : public TC_DataListCtrl<Security, std::string>
	{

		public:
			Ctrl();
			virtual std::string filter(std::string& rec_) const { return rec_; } 
			void highlight(const std::string& item);
			typedef TC_DataListCtrl<Security, std::string>		BASE;
			typedef BASE::Record Record;
			BASE::R_Vec r_vec;
		private:
			
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._sec; return true; }
			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
	};
	DECLARE_MESSAGE_MAP()

	void fill_leader(const std::string& l_ = "");
	void fill_follower(const std::string& f_ = "");
	void fill_leaf();
	void fill_config(const std::string& under_);

	auto_ptr<Ctrl>		_ctrl_l;
	auto_ptr<Ctrl>		_ctrl_f;
//	CButton				_rundir_btn;
//	CButton				_settings_btn;
	CButton				_ok_btn;
	CDateTimeCtrl*		_date_picker;
	CComboBox*			_cfg_picker;
	CComboBox*			_fwr_picker;
	TCDate				_date;

	int _selection;
//	int	_under;
	int _auto_trading;
	int _model_rerun;
	std::vector<std::string>	_config_v;
	RunSettings					_run_s;
public:
//	afx_msg void OnLvnItemchangedListLeader(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDateChange(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif //_INIT_DLG_H_