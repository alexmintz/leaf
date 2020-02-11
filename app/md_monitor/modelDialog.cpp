#include "stdafx.h"
#include "p_format.h"
#include "modelDialog.h"


IMPLEMENT_DYNCREATE(model_Dlg, ETSLayoutDialog)

BEGIN_MESSAGE_MAP(model_Dlg, ETSLayoutDialog)
	ON_BN_CLICKED(IDOK,			OnBnClickedOk)
	ON_BN_CLICKED(ID_SAVE_AS,	OnBnClickedSave)
	ON_BN_CLICKED(ID_OPEN,	OnBnClickedOpen)
END_MESSAGE_MAP()

//======================================================================
model_Dlg::Ctrl::Ctrl(bool edit_)//const std::string& base_resource_):_base_resource(base_resource_)
{
//	_run_data.init();
	add_column("Name",	f_loc(&Record::_name));
	add_column(new EColumn<std::string>("Value", f_loc(&Record::_value), _run_data._m_base, edit_));
}
//======================================================================
model_Dlg::model_Dlg(UINT nID, bool edit_)
: ETSLayoutDialog(nID), _ctrl(new Ctrl(edit_))
{ 
}

//======================================================================
model_Dlg::~model_Dlg()
{
}
//======================================================================
void model_Dlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUN, *_ctrl);
}
//======================================================================
BOOL model_Dlg::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();
	update_layout();
	ctrl()->fill_run();
	SetWindowText(_T("Model Parameters"));
	return TRUE;
}
//======================================================================
void model_Dlg::update_layout()
{
	UpdateLayout(
	pane(VERTICAL)<<
	(pane(HORIZONTAL)
		<< (pane(VERTICAL) << item(IDC_LIST_RUN)))
	<< (pane(HORIZONTAL, ABSOLUTE_VERT) 
		<< item( paneNull, GREEDY)
		<< item(ID_OPEN, NORESIZE | ALIGN_CENTER)
		<< item(paneNull, GREEDY)
		<< item(IDOK, NORESIZE | ALIGN_CENTER)
		<< item( paneNull, GREEDY)
		<< item(ID_SAVE_AS, NORESIZE | ALIGN_CENTER)
		<< item(paneNull, GREEDY))
	);
}
//======================================================================

void model_Dlg::Ctrl::fill_run()
{
	_run_data.init(true);
	vec.clear();
	for(size_t i = 0; i < _run_data._model.size(); ++i)
	{
		vec.push_back(BASE::R_Ptr(new ModelResource(_run_data._model[i].first, _run_data._model[i].second)));
	}

	mark_for_delete();
	update_data(vec, (INT_PTR)0, TC_ListCtrlBase::SORT_DSC);

 	SetColumnWidth(0, LVSCW_AUTOSIZE);
	SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
}
//======================================================================

void model_Dlg::Ctrl::save(const std::string& cf_)
{
	fill_run();
	std::ofstream l_stream(cf_.c_str());
	if (l_stream.good())
		for(size_t i = 0; i < vec.size(); ++i)
			l_stream << _run_data._m_base << ":" << vec[i]->_name << "\t" << vec[i]->_value << std::endl;
}
//======================================================================
void model_Dlg::OnBnClickedOk()
{
	EndDialog(0);
}
//======================================================================
void model_Dlg::OnBnClickedSave()
{
	char strFilter[] = { "Config Files (*.cfg)|*.cfg|" }; 
	CFileDialog FileDlg(FALSE, CString(".cfg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(strFilter));

	if (FileDlg.DoModal() == IDOK)  
	{  
		std::string pn = CT2A(FileDlg.GetPathName());
		_ctrl->save(pn);
	}
}
//======================================================================
void model_Dlg::OnBnClickedOpen()
{
	char strFilter[] = { "Config Files (*.cfg)|*.cfg|" }; 
	CFileDialog FileDlg(TRUE, CString(".cfg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CString(strFilter));

	if (FileDlg.DoModal() == IDOK)  
	{  
		std::string pn = CT2A(FileDlg.GetPathName());
		std::string st = Z::get_setting("COMMAND:config");
		Z::add_setting("COMMAND:config", pn);
		TCSettings::rescan();	
		Z::add_setting("COMMAND:config", st);
		_ctrl->fill_run();
	}
}
//======================================================================
