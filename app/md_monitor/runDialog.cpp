#include "stdafx.h"
#include "p_format.h"
#include "runDialog.h"


IMPLEMENT_DYNCREATE(run_Dlg, ETSLayoutDialog)

BEGIN_MESSAGE_MAP(run_Dlg, ETSLayoutDialog)
	ON_BN_CLICKED(IDOK,		OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,	OnBnClickedCancel)
END_MESSAGE_MAP()

//======================================================================
run_Dlg::Ctrl::Ctrl()
{
	add_column("Run",				f_loc(&Record::_run));
	add_column("Done by",			f_loc(&Record::_run_name));

	add_column("Started",			f_loc(&Record::_start));
	add_column("From",				f_loc(&Record::_from));
	add_column("To",				f_loc(&Record::_to));

	add_column("Leader:Follower",	f_loc(&Record::_keys));
}
//======================================================================
run_Dlg::run_Dlg(UINT nID)
: ETSLayoutDialog(nID), _ctrl(new Ctrl)
{ 
}

//======================================================================
run_Dlg::~run_Dlg()
{
}
//======================================================================
void run_Dlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUN, *_ctrl);
//	DDX_Control(pDX, IDC_LIST_RUN, _list);
}
//======================================================================
BOOL run_Dlg::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();
	update_layout();
	ctrl()->fill_run();
	SetWindowText(_T("Backtest Run Selection"));
	return TRUE;
}
//======================================================================
void run_Dlg::update_layout()
{
	UpdateLayout(
	pane(VERTICAL)<<
	(pane(HORIZONTAL)
		<< (pane(VERTICAL) << item(IDC_LIST_RUN)))
	<< (pane(HORIZONTAL, ABSOLUTE_VERT) 
		<< item( paneNull, GREEDY)
		<< item(IDOK, NORESIZE | ALIGN_CENTER)
		<< item( paneNull, GREEDY)
		<< item(IDCANCEL, NORESIZE | ALIGN_CENTER)
		<< item(paneNull, GREEDY))
	);
}
//======================================================================

void run_Dlg::Ctrl::fill_run()
{
	BMMixer::RunInfoVec vec;
	BM_MIXER->get_runs_info(vec);
	update_data(vec, (INT_PTR)0, TC_ListCtrlBase::SORT_DSC);

 	SetColumnWidth(0, LVSCW_AUTOSIZE);
	SetColumnWidth(1, LVSCW_AUTOSIZE);
	SetColumnWidth(2, LVSCW_AUTOSIZE);
	SetColumnWidth(3, LVSCW_AUTOSIZE);
	SetColumnWidth(4, LVSCW_AUTOSIZE);
	SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);

	SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

//======================================================================
void run_Dlg::OnBnClickedOk()
{
	int row = _ctrl->GetNextItem(-1, LVNI_SELECTED);
	if(row == -1)
		return;
	_info = ctrl()->get_record(row);
	EndDialog(_info->_run);
}
//======================================================================
void run_Dlg::OnBnClickedCancel()
{
	EndDialog(-1);
}
//======================================================================
