#include "stdafx.h"
#include "p_format.h"
#include "initDialog.h"
#include "modelDialog.h"


IMPLEMENT_DYNCREATE(init_Dlg, CDialog)

BEGIN_MESSAGE_MAP(init_Dlg, CDialog)
	ON_BN_CLICKED(IDOK_I,		OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL_I,	OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_LIVEB,	OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_NBT,	OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_RBT,	OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_LBT, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO_RTR, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_CHECK_AUTO_TRADING, OnBnClickedSettings)
	ON_CBN_SELCHANGE(IDC_FOLLOWER_PICKER, &init_Dlg::OnFollowerPicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER_INI, &init_Dlg::OnDateChange)

END_MESSAGE_MAP()

//======================================================================
init_Dlg::Ctrl::Ctrl()
{
	add_column("Security",	f_loc(&Record::_sec));
}
//======================================================================
void init_Dlg::Ctrl::highlight(const std::string& item_)
{
	for (int i=0; i < GetItemCount(); i++)
	{
		if (CString(CA2T(item_.c_str())) == GetItemText(i, 0))
		{
			int ix = GetNextItem(-1, LVNI_SELECTED);
			if(ix >= 0)
				SetItemState(ix, 0, LVIS_SELECTED);

			SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			EnsureVisible(i, FALSE);
			RECT rect;
			if (GetItemRect(i, &rect, LVIR_LABEL))
			{
				CSize size;
				size.cx = 0;
				size.cy = rect.bottom - rect.top;
				size.cy *= i - GetTopIndex();
				if (i != GetTopIndex())
					Scroll(size);
			}
			break;
		}
   }


}
//======================================================================
init_Dlg::init_Dlg(UINT nID)
: CDialog(nID), _ctrl_l(new Ctrl), _ctrl_f(new Ctrl), _selection(1), 
_auto_trading(0), _model_rerun(1)
{

}

//======================================================================
init_Dlg::~init_Dlg()
{
}
//======================================================================
void init_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LEADER,	*_ctrl_l);
	DDX_Control(pDX, IDC_LIST_FOLLOWER, *_ctrl_f);
	DDX_Radio(pDX, IDC_RADIO_LIVEB, _selection);
}
//======================================================================
BOOL init_Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	_ok_btn.SubclassDlgItem(IDOK_I, this);
	
	_fwr_picker = (CComboBox*)GetDlgItem(IDC_FOLLOWER_PICKER);
	_fwr_picker->ResetContent();

	const std::vector<std::string>& und = LFSecurityMaster::instance()->get_und();
	for (size_t s = 0; s < und.size(); ++s)
		_fwr_picker->AddString(CA2T(und[s].c_str()));

	_fwr_picker->SetCurSel(0);

	fill_leaf();
	fill_config(Z::get_setting("LFStrategy:follower_underlying"));
	SetWindowText(_T("Select Mode and Security"));
	CheckDlgButton(IDC_CHECK_AUTO_TRADING, BST_CHECKED);

	return TRUE;
}


//======================================================================
void init_Dlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	int rowl = _ctrl_l->GetNextItem(-1, LVNI_SELECTED);
	int rowf = _ctrl_f->GetNextItem(-1, LVNI_SELECTED);

	if(rowl == -1 || rowf == -1)
		return;

	_cfg_picker = (CComboBox*)GetDlgItem(IDC_CFGPICKER);
	int sel = _cfg_picker->GetCurSel();
	CString str;
	_cfg_picker->GetLBText(sel, str);

	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);
	Z::add_setting("^LFModel:z:strat_config", strStd);
	TCSettings::rescan(strStd);

	TCDate cut_off(Z::get_setting("LFModel:cutoff_date"));
	if(cut_off.empty())
		cut_off = tc_b_date->prev_business_date(TCDateTime::now().get_date());

	// less than cutoff, use backtest, otherwise CQG record
	std::string d = Z::get_setting("BMTS:LFDepthData:table", "depth");
	std::string t = Z::get_setting("BMTS:LFTradeData:table", "trade");

	if (_date.get_ymd() < cut_off.get_ymd())
	{
		std::string d_b = Z::get_setting("BMTS:LFDepthData_bt:table");
		std::string t_b = Z::get_setting("BMTS:LFTradeData_bt:table");
		if (d_b.empty())
			d_b = d + "_backtest";
		if (t_b.empty())
			t_b = t + "_backtest";

		Z::add_setting("BMTS:LFDepthData:table", d_b);
		Z::add_setting("BMTS:LFTradeData:table", t_b);
	}

	Z::add_setting("LFModel:date", _date.as_string());

	Security* l = _ctrl_l->get_record(rowl);
	Security* f = _ctrl_f->get_record(rowf);

	Z::add_setting("LFStrategy:leader", l->_sec);
	Z::add_setting("LFStrategy:follower", f->_sec);

	RunSettings::instance()->init();
	LFSecurityMaster::instance()->strategy_id(f->_sec);

	OnBnClickedSettings();

	RunSettings::instance()->set_trading(_auto_trading == 1);
	RunSettings::instance()->set_instance_disabled(_model_rerun != 1);

	switch (_selection)
	{
	case 0: // LIVE BACKTEST
		RunSettings::instance()->set_live_run(true);
		RunSettings::instance()->set_live_backtest(true);
		break;

	case 1:	// NEW BACKTEST
		RunSettings::instance()->set_backtest(true);
		break;

	case 2: // RE-RUN BACKTEST
	{
		RunSettings::instance()->set_rerun_backtest(true);
		BMMixer::RunInfoVec vec;
		BM_MIXER->get_runs_info(vec);
		if (vec.size() == 0)
		{
			std::string e = "There are no runs for " + l->_sec + ":" + f->_sec + " pair. \n Choose another once or Cancel";
			AfxMessageBox(CA2T(e.c_str()), MB_ICONSTOP);
			return;
		}
		break;
	}
	case 3: // RE-RUN LIVE
		RunSettings::instance()->set_rerun_backtest(true);
		RunSettings::instance()->set_rerun_live(true);
		break;
	}


	EndDialog(_selection);
}
//======================================================================
void init_Dlg::OnBnClickedCancel()
{
	EndDialog(-1);
}
//======================================================================
void init_Dlg::fill_leader(const std::string& l_)
{
	UpdateData(TRUE);
	_ctrl_l->r_vec.clear();

	const LFSecurityMaster::LFSecurityMap& m = LFSecurityMaster::instance()->get_map();
	for(LFSecurityMaster::LFSecurityMap::const_iterator it = m.begin(); it != m.end(); ++it)
		_ctrl_l->r_vec.push_back(Z::smart_ptr<Security>(new Security(it->first)));

	_ctrl_l->update_data(_ctrl_l->r_vec, (INT_PTR)0, TC_ListCtrlBase::SORT_DSC);
	if(!l_.empty())
		_ctrl_l->highlight(l_);
	else
		_ctrl_l->highlight(Z::get_setting("LFStrategy:leader"));
}
//======================================================================
void init_Dlg::fill_follower(const std::string& f_)
{
	UpdateData(TRUE);

	int row = _ctrl_l->GetNextItem(-1, LVNI_SELECTED);
	if(row == -1)
		return;
	_ctrl_f->r_vec.clear();


	Security* s = _ctrl_l->get_record(row);
	std::string f_stripped = f_.substr(0, f_.size() - 3);
	for (size_t i = 0; i < (_ctrl_l->r_vec).size(); ++i)
	{
		std::string& list_s = _ctrl_l->r_vec[i]->_sec;
		if (f_.empty() ||
			(f_[8] == 'S' && _ctrl_l->r_vec[i]->_sec.find(f_.substr(0, 9)) != std::string::npos) ||
			//(f_[8] != 'S' && _ctrl_l->r_vec[i]->_sec.find(f_.substr(0, 7)) != std::string::npos))
			f_stripped == list_s.substr(0, list_s.size() - 3))
		{
			_ctrl_f->r_vec.push_back(_ctrl_l->r_vec[i]);
		}
	}
	_ctrl_f->mark_for_delete();
	_ctrl_f->do_delete();

	_ctrl_f->update_data(_ctrl_f->r_vec, (INT_PTR)0, TC_ListCtrlBase::SORT_DSC);
	if (!f_.empty())
		_ctrl_f->highlight(f_);
	else
		_ctrl_f->highlight(Z::get_setting("LFStrategy:follower"));
}
//======================================================================
void init_Dlg::OnBnClickedRadio()
{
	UpdateData(TRUE);
	switch(_selection)
	{
	case 0: // LIVE BACKTEST
	case 1:	// NEW BACKTEST
//		GetDlgItem(IDC_CHECK_MODEL)->EnableWindow(FALSE);
		CheckDlgButton(IDC_CHECK_AUTO_TRADING, BST_CHECKED);
//		CheckDlgButton(IDC_CHECK_MODEL, BST_CHECKED);
		break;
	case 3: // RE-RUN Backtest or Live
			OnBnClickedRunDir();
	case 2:
//		GetDlgItem(IDC_CHECK_MODEL)->EnableWindow(TRUE);
		CheckDlgButton(IDC_CHECK_AUTO_TRADING, BST_UNCHECKED);
//		CheckDlgButton(IDC_CHECK_MODEL, BST_CHECKED);
		break;
	}
}
void init_Dlg::OnFollowerPicker()
{
	UpdateData(TRUE);
	fill_leaf();
	fill_config(Z::get_setting("LFStrategy:follower_underlying"));
}

//======================================================================
void init_Dlg::OnBnClickedSettings()
{
	_auto_trading = IsDlgButtonChecked(IDC_CHECK_AUTO_TRADING);
}

//======================================================================
void init_Dlg::OnBnClickedRunDir()
{
//Folder Browser code.
    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(BROWSEINFO));

    bi.hwndOwner = m_hWnd;
    bi.ulFlags   = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    BOOL bRet = FALSE;

    TCHAR szFolder[MAX_PATH*2];
    szFolder[0] = _T('\0');

    if (pidl)
    {
        if (SHGetPathFromIDList(pidl, szFolder))
        {
            bRet = TRUE;
        }
        IMalloc *pMalloc = NULL;
        if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
        {
            pMalloc->Free(pidl);
            pMalloc->Release();
        }
		std::wstring ws(szFolder);
		std::string f(ws.begin(), ws.end()); f += "\\";
		Z::add_setting("BMTS:run_dir", f);
    }
	UpdateData(FALSE);
}
//======================================================================
void init_Dlg::OnDateChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	fill_leaf();
}
void init_Dlg::fill_leaf()
{
	CTime d;
	struct tm dpr;

	_date_picker= (CDateTimeCtrl*)GetDlgItem(IDC_DATEPICKER_INI);
	_date_picker->GetTime(d);
	d.GetLocalTm(&dpr);
	_date.set_tm(dpr);

	_fwr_picker = (CComboBox*)GetDlgItem(IDC_FOLLOWER_PICKER);
	int sel = _fwr_picker->GetCurSel();
	CString str;
	_fwr_picker->GetLBText(sel, str);

	CT2CA pszConvertedAnsiString(str);
	// construct a std::string using the LPCSTR input
	std::string und(pszConvertedAnsiString);
	std::string follower = LFSecurityMaster::instance()->get_instr(und, _date);

	
	Z::add_setting("LFStrategy:follower_underlying", und);
	tc_b_date->set_exchange_for_underlier(und);

	std::string dxe = LFSecurityMaster::instance()->get_dxe(_date);
	
	fill_leader(dxe);
	fill_follower(follower);

}
//======================================================================
void init_Dlg::fill_config(const std::string& under_)
{
	_config_v.clear();
	std::string cdir = TCSettings::get("COMMAND:z_config_dir");
	ACE_Dirent adr(cdir.c_str());
	dirent* dp = 0;

	std::vector<std::string> v;
	std::vector<std::string> vu;
	while ((dp = adr.read()) != 0)
	{
		v.clear();
		vu.clear();
		std::string tn = dp->d_name;
		tc_token_vec(tn, v, '.');
		size_t s = v.size();
		if (s == 0 || v[s - 1] != "cfg")
			continue;

		tc_token_vec(v[0], vu, '_');
		if (vu.size() == 0 || vu[0] != under_)
			continue;

		_config_v.push_back(tn);
	}

	_cfg_picker = (CComboBox*)GetDlgItem(IDC_CFGPICKER);
	_cfg_picker->ResetContent();

	for (std::size_t s = 0; s < _config_v.size(); ++s)
		_cfg_picker->AddString(CA2T(_config_v[s].c_str()));

	_cfg_picker->SetCurSel(0);
	_ok_btn.EnableWindow(!_config_v.empty());
}
//======================================================================

