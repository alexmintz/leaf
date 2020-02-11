/*
*
*	© Copyright 2003-2012 by TAF Co. All rights reserved.
*
*/

#ifndef _MODEL_DLG_H_
#define _MODEL_DLG_H_

#pragma once

#include "znet/TCDefs.h"
#include "znet/TCSettings.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "resource.h"

struct ModelResource
{
	ModelResource(const std::string& name_, const std::string& value_)
		:_name(name_), _value(value_){}
	std::string _name;
	std::string _value;
};

class model_Dlg : public ETSLayoutDialog
{
	DECLARE_DYNCREATE(model_Dlg)

public:	
	model_Dlg(UINT nID = IDD_MODEL_DIALOG, bool edit_ = true);
	virtual ~model_Dlg();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV support
	virtual void update_layout();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedOpen();
protected:
	class Ctrl : public TC_DataListCtrl<ModelResource, std::string>
	{
		typedef TC_DataListCtrl<ModelResource, std::string>		BASE;
		typedef BASE::Record Record;
		public:
			template<class FIELD>
			class EColumn: public FColumn<FIELD>
			{
			public:
				EColumn(const std::string& title_, Loc<FIELD>* lf_, const std::string& base_, bool edit_ = true)
					: FColumn(title_, lf_), _base(base_), _edit(edit_)
				{}
				virtual ~EColumn() {}
				
				virtual bool may_edit(Record*, INT_PTR) const { return _edit; }
				virtual void edited(Record& rec_, INT_PTR row_, std::string new_value_) const 
				{ 
					rec_._value = new_value_;
					std::string res = _base + ":" + rec_._name;
					Z::add_setting(res, new_value_);
				}
				std::string _base;
				bool		_edit;
			};
			Ctrl(bool edit_);
			virtual std::string filter(std::string& rec_) const { return rec_; } 
			void fill_run();
			void save(const std::string& cf_);
		private:		
			virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._name; return true; }
			int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
			LFRunData _run_data;
			R_Vec vec;
	};

	DECLARE_MESSAGE_MAP()
	
	Ctrl* ctrl() {return (Ctrl*)_ctrl.get(); }
	auto_ptr<Ctrl>			_ctrl;
};

#endif //_MODEL_DLG_H_