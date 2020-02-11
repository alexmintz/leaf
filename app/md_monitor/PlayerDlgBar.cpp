// PlayerDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "md_monitor.h"
#include "PlayerDlgBar.h"
#include "znet/BMModel.h"
#include "md_graphView.h"
#include "md_monitorView.h"
#include "p_RTPosView.h"
#include "p_OrderView.h"
#include "p_OrderFillView.h"
#include "runDialog.h"
#include "modelDialog.h"
#include <iomanip>

IMPLEMENT_DYNAMIC(PlayerDlgBar, CDialogBar)

PlayerDlgBar::PlayerDlgBar():
_play(false), _start(true), _pos(1.0), _mpos(1.0), _run(-1)
{}

PlayerDlgBar::~PlayerDlgBar()
{
}

void PlayerDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PlayerDlgBar, CDialogBar)
	//{{AFX_DATA_INIT(PlayerDlgBar)
	//}}AFX_DATA_INIT
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &PlayerDlgBar::OnStop)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &PlayerDlgBar::OnPlay)
	ON_BN_CLICKED(IDC_BUTTON_RUN,  &PlayerDlgBar::OnRun) 
	ON_BN_CLICKED(IDC_BUTTON_MODEL1,&PlayerDlgBar::OnModel) 
	ON_NOTIFY(NM_RELEASEDCAPTURE,	IDC_SLIDER, &PlayerDlgBar::OnSliderSet)
	ON_NOTIFY(NM_CUSTOMDRAW,		IDC_SLIDER, &PlayerDlgBar::OnSliderDraw) 
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMEPICKER1, &PlayerDlgBar::OnDtnDatetimechangeTimepicker)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIMEPICKER2, &PlayerDlgBar::OnDtnDatetimechangeTimepicker)
	ON_CBN_SELCHANGE(IDC_COMBO_BAR, &PlayerDlgBar::OnCbnSelchangeComboBar)
END_MESSAGE_MAP()


// PlayerDlgBar message handlers
LRESULT PlayerDlgBar::OnInitDialog ( WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = (BOOL)(HandleInitDialog(wParam, lParam));

	if (!UpdateData(FALSE))	
		Z::report_error("%s", "Warning: UpdateData failed during dialog init.");

	_slider		= (CSliderCtrl*)GetDlgItem(IDC_SLIDER);		
	_slider->SetRange(0, 100, 1);
	_slider->SetPos((int)(10*_pos));
	_slider->SetTicFreq(10);

	if(BMModel::instance()->is_live()) // Live
	{
		//GetDlgItem(IDC_BUTTON_MODEL1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RUN)->ShowWindow(SW_HIDE);
		if (RunSettings::instance()->get_live_backtest())
			GetDlgItem(IDC_EDIT_RUN)->SetWindowText(_T("Live Run"));
		else
			GetDlgItem(IDC_EDIT_RUN)->SetWindowText(_T("MD LIVE"));
		GetDlgItem(IDC_EDIT_RUN)->ShowWindow(SW_SHOW);
	}
	else if (!RunSettings::instance()->get_rerun_backtest()) // New Run
	{
		GetDlgItem(IDC_BUTTON_RUN)->ShowWindow(SW_HIDE);;
		GetDlgItem(IDC_EDIT_RUN)->SetWindowText(_T("New Run"));
		GetDlgItem(IDC_EDIT_RUN)->ShowWindow(SW_SHOW);
	}
	_bar_picker = (CComboBox*)GetDlgItem(IDC_COMBO_BAR);	
	std::string bar_time = TCSettings::get("LFStrategy:bar_time", "100 msec,1   sec,30  sec,1   min,5   min,15  min"); 
	std::vector<std::string> v;	tc_token_vec(bar_time, v, ',');
	int s = (int)(v.size());
	for(int i = 0; i < s; ++i)
	{
		_bar_picker->AddString(CA2T(v[i].c_str()));
		_bar_time.push_back(convert_bar(v[i]));
	}
	int default_bar = TCSettings::get_integer("LFStrategy:default_bar_time_index", 1); 
	_bar_picker->SetCurSel(s>default_bar?default_bar:0);
	
	if (BMModel::instance()->is_live() || RunSettings::instance()->get_rerun_backtest())
		gray_out(FALSE, false);
	
	return bRet;
}

BOOL PlayerDlgBar::Create(CWnd* pParent, UINT nIDTemplate, UINT nStyle, UINT nID) 
{	
	BOOL bReturn = CDialogBar::Create(pParent, nIDTemplate, nStyle, nID);

	if(bReturn)
	{
		_play_btn.AutoLoad(IDC_BUTTON_PLAY, this);
		_stop_btn.AutoLoad(IDC_BUTTON_STOP, this);
	}
	if (BMModel::instance()->is_live() || RunSettings::instance()->get_rerun_backtest())
	{
		set_play(false);
		set_stop(false);
	}
	else
	{
		set_btr(true);
		gray_out(true, true);
		set_play(true);
		set_stop(true);
	}
	return bReturn;
}
void PlayerDlgBar::OnPlay()
{ 	
	if(!_play)
		Play();
	else
		Pause();
}
void PlayerDlgBar::OnStop()
{
	if(_start)   // no action at the begining
		return;

	Z::report_debug("PlayerDlgBar::OnStop -- stopping mixer...");
	BM_MIXER->stop(true, true);
	Z::report_debug("PlayerDlgBar::OnStop -- mixer stopped");

	_start = true;
	
	set_play(true);
	gray_out(TRUE, true); 
	theApp.mainframe()->get_monitor()->enable_buy_sell_buttons(FALSE);
	theApp.strategy()->report_run();
}
void PlayerDlgBar::Pause()
{
	BM_MIXER->hold_play();
	set_play(true);

}
void PlayerDlgBar::Play()
{	
	if(_start || BM_MIXER->is_closed())
	{
		get_btr();
		
		if(!_btr.is_valid())
		{
			set_play(false);
			return;
		}
		_start = false;

		theApp.mainframe()->get_graph()->set_play(_bar_time, get_bar(), _btr);
		theApp.mainframe()->get_monitor()->change_run();
		theApp.mainframe()->get_fill()->ctrl()->change_run();
		BM_MIXER->set_time_factor(_mpos);
		if (BM_MIXER->start(_btr, RunSettings::instance()->get_rerun_live() ? -1 : 0, _run) < 0)
		{
			AfxMessageBox(CA2T("There is no recorded data for the requested interval."), MB_ICONSTOP);
			return;
		}
		Z::report_info("BM_MIXER started");
		gray_out(FALSE, true); 
		theApp.mainframe()->get_monitor()->init_buttons();
		if (!RunSettings::instance()->get_rerun_backtest()) // New Run
		{
			std::ostringstream os; os << BM_MIXER->run_id();
			GetDlgItem(IDC_BUTTON_RUN)->SetWindowText(CA2T(os.str().c_str()));
			//GetDlgItem(IDC_BUTTON_RUN)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_EDIT_RUN)->SetWindowText(CA2T(os.str().c_str()));
		}
		theApp.mainframe()->get_graph()->change_run();
	}
	else
	{
		BM_MIXER->set_time_factor(_mpos);
		BM_MIXER->unhold_play();
	}
	set_pause(true);
}

void PlayerDlgBar::OnSliderSet(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(_play)
		BM_MIXER->set_time_factor(_mpos);
}

void PlayerDlgBar::OnSliderDraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	_slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER);
	_pos = _slider->GetPos()/10.;
	set_slider(false);
}
void PlayerDlgBar::set_slider(bool running_)
{
	std::ostringstream os;
	os << std::setprecision(2) << std::fixed;

	if(running_)
	{
		if(_play && _pos < 1.0)
		{
			double tf = BM_MIXER->get_time_factor();
//			double tf = 0;
			if(tf != 0)
				os << " x " << 1/tf;
			else
				os << "FASTEST";
			GetDlgItem(IDC_EDIT_F)->SetWindowText(CA2T(os.str().c_str()));
		}
	}
	else
	{
		_mpos = _pos;
		if(_pos == 1)
			os << "Real Time";
		else if(_pos  == 0)
			os << "FASTEST";
		else if(_pos > 1)
			os  << "1/" << _pos;
		else if(_pos > 0 && _pos <= 0.1)
		{
			os << " x " << 250;
			_mpos = 0.004;
		}
		else if(_pos > 0.1 && _pos <= 0.2)
		{
			os << " x " << 50;
			_mpos = 0.02;
		}
		else if(_pos > 0.2 && _pos <= 0.3)
		{
			os << " x " << 40;
			_mpos = 1./40.;
		}
		else if(_pos > 0.3 && _pos <= 0.4)
		{
			os << " x " << 30;
			_mpos = 1./30.;
		}
		else if(_pos > 0.4 && _pos <= 0.5)
		{
			os << " x " << 20;
			_mpos = 1./20.;
		}
		else if(_pos > 0.5 && _pos <= 0.6)
		{
			os << " x " << 15;
			_mpos = 1./15.;
		}
		else if(_pos > 0.6 && _pos <= 0.7)
		{
			os << " x " << 10;
			_mpos = 0.1;
		}
		else if(_pos > 0.7 && _pos <= 0.8)
		{
			os << " x " << 5;
			_mpos = 0.2;
		}
		else if(_pos > 0.8 && _pos <= 0.9)
		{
			os << " x " << 3;
			_mpos = 1./3.;
		}
		else if(_pos > 0.9 && _pos <= 1.0)
		{
			os << " x " << 2;
			_mpos  = 0.5;
		}
		GetDlgItem(IDC_EDIT_F)->SetWindowText(CA2T(os.str().c_str()));
	}
}

void PlayerDlgBar::OnDtnDatetimechangeTimepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	bool prev_btr = _btr.is_valid();
	get_btr();
	if(!prev_btr && _btr.is_valid() && !_play)
	{
		set_play(true);
		set_stop(true);
	}
}
long PlayerDlgBar::get_bar()
{
	_bar_picker = (CComboBox*)GetDlgItem(IDC_COMBO_BAR);	
	if(_bar_picker == 0)
		return 0;
	return _bar_picker->GetCurSel();
}
long PlayerDlgBar::convert_bar(const std::string& t_)
{
	std::vector<std::string> v;
	tc_token_vec(t_, v, ' ');	

	long mseconds;
	tc_token(v[0], mseconds);

	if(v[1] == "msec")
		return	mseconds;
	else if(v[1] == "sec")
		return mseconds*1000;
	else if(v[1] == "min")
		return mseconds*60000;
	return 1;
}
void PlayerDlgBar::gray_out(BOOL enable_, bool date_only_)
{
	GetDlgItem(IDC_DATEPICKER)->EnableWindow(enable_);
	GetDlgItem(IDC_TIMEPICKER1)->EnableWindow(enable_);
	GetDlgItem(IDC_TIMEPICKER2)->EnableWindow(enable_);
	if(!BMModel::instance()->is_live())
		GetDlgItem(IDC_BUTTON_MODEL1)->ShowWindow(enable_); 
	if(!date_only_)
		GetDlgItem(IDC_SLIDER)->EnableWindow(enable_);
}
void PlayerDlgBar::get_btr()
{
	_date_picker= (CDateTimeCtrl*)GetDlgItem(IDC_DATEPICKER);
	_time_from	= (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER1);
	_time_to	= (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER2);

	CTime d; 
	struct tm dpr, tmf, tmt;
	
	_date_picker->GetTime(d);			d.GetLocalTm(&dpr);
	_time_from->GetTime(d);				d.GetLocalTm(&tmf);
	_time_to->GetTime(d);				d.GetLocalTm(&tmt);

	TCDate date(dpr);
	TCDateTime	from(date, TCTime(tmf));
	TCDateTime	to(date, TCTime(tmt));

	_btr.init(from.get_ace_time(), to.get_ace_time());
}
void PlayerDlgBar::set_btr(bool init)
{
	_date_picker= (CDateTimeCtrl*)GetDlgItem(IDC_DATEPICKER);
	_time_from	= (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER1);
	_time_to	= (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER2);

	struct tm dpr;//, tmf, tmt;

	TCDate		date;
	TCDateTime	fr;
	TCDateTime	to;

	if(init)
	{
		date.set_str(Z::get_setting("LFModel:date"));
	}
	else
	{
		CTime d; 
		_date_picker->GetTime(d);			
		d.GetLocalTm(&dpr);
		date.set_tm(dpr);
	}	
	fr.set_dt(date, TCTime(Z::get_setting("LFModel:start_time", "09:15:00")));
	to.set_dt(date, TCTime(Z::get_setting("LFModel:stop_time", "14:15:00")));
		
	CTime ctf(fr.get_date().year(), fr.get_date().month(), fr.get_date().day(),
		      fr.get_time().hour(), fr.get_time().minute(), fr.get_time().sec());
	_time_from->SetTime(&ctf);

	CTime ctt(to.get_date().year(), to.get_date().month(), to.get_date().day(),
		     to.get_time().hour(), to.get_time().minute(), to.get_time().sec());
	_time_to->SetTime(&ctt);

	if(init)
		_date_picker->SetTime(&ctt);
}
void PlayerDlgBar::set_play(bool _visible)
{
	_play = false;
	if(_visible)
		_play_btn.LoadBitmaps(CA2T("PLAYU"), CA2T("PLAYD"));
	else
		_play_btn.LoadBitmaps(CA2T("PLAYX"), CA2T("PLAYX"));
	_play_btn.SizeToContent();
	_play_btn.RedrawWindow();
}
void PlayerDlgBar::set_stop(bool _visible)
{
  	if(_visible)
		_stop_btn.LoadBitmaps(CA2T("STOPU"), CA2T("STOPD"));
	else
		_stop_btn.LoadBitmaps(CA2T("STOPX"), CA2T("STOPX"));
	_stop_btn.SizeToContent();
	_stop_btn.RedrawWindow();
}
void PlayerDlgBar::set_pause(bool _visible)
{
	_play = true;
	if(_visible)
		_play_btn.LoadBitmaps(CA2T("PAUSEU"), CA2T("PAUSED"));
	else
		_play_btn.LoadBitmaps(CA2T("PAUSEX"), CA2T("PAUSEX"));
	_play_btn.SizeToContent();
	_play_btn.RedrawWindow();
}
void PlayerDlgBar::set_time_from(const ACE_Time_Value& _tv)
{
	if(!_play && _start)
		set_btr();
	else
	{
		_time_from	= (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER1);
		TCDateTime dt(_tv);
		CTime ct(dt.get_date().year(), dt.get_date().month(), dt.get_date().day(),
			     dt.get_time().hour(), dt.get_time().minute(), dt.get_time().sec());
		_time_from->SetTime(&ct);
	}
	set_slider(true);
}


void PlayerDlgBar::OnCbnSelchangeComboBar()
{
		theApp.mainframe()->get_graph()->set_dt(_bar_time, get_bar());	
}

bool PlayerDlgBar::process_inactive()
{
	Z::report_debug("-->PlayerDlgBar::process_inactive");
	OnStop();
	Z::report_debug("<--PlayerDlgBar::process_inactive");
	return true;
}
void PlayerDlgBar::OnModel()
{
	model_Dlg dlg(IDD_MODEL_DIALOG, !BMModel::instance()->is_live());
	dlg.DoModal();
}
void PlayerDlgBar::OnRun()
{
	if (!RunSettings::instance()->get_rerun_backtest() || BMModel::instance()->is_live())
		return;

	OnStop();

	_time_from = (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER1);
	_time_to = (CDateTimeCtrl*)GetDlgItem(IDC_TIMEPICKER2);
	_date_picker = (CDateTimeCtrl*)GetDlgItem(IDC_DATEPICKER);

	gray_out(TRUE, false);
	set_play(true);
	set_stop(true);

	long run = -1;
	std::ostringstream os;
	TCDateTime dt_from;
	TCDateTime dt_to;

	if (!RunSettings::instance()->get_rerun_live())
	{
		run_Dlg rd;
		run = (long)(rd.DoModal());
		if (run == -1)
			return;

		os << _run;

		std::vector<std::string> v;
		tc_token_vec(rd.get_info()->_from, v, '_');

		dt_from.set_date(v[0]);
		dt_from.set_time(v[1], '\0');

		v.clear();
		tc_token_vec(rd.get_info()->_to, v, '_');

		dt_to.set_date(v[0]);
		dt_to.set_time(v[1], '\0');
	}
	else
	{
		std::string start_t = Z::get_setting("LFModel:start_time", "07:00:00");
		std::string stop_t = Z::get_setting("LFModel:stop_time", "14:00:00");
		std::string day_t = Z::get_setting("LFModel:date");

		dt_from.set_dt(TCDate(day_t), TCTime(start_t));
		dt_to.set_dt(TCDate(day_t), TCTime(stop_t));

		os << "replay " << Z::get_setting("LFStrategy:strategy_id");
	}

	_run = run;
	GetDlgItem(IDC_BUTTON_RUN)->SetWindowText(CA2T(os.str().c_str()));
	
	CTime ct(dt_from.get_date().year(), dt_from.get_date().month(), dt_from.get_date().day(),
		dt_from.get_time().hour(), dt_from.get_time().minute(), dt_from.get_time().sec());

	CTime tct(dt_to.get_date().year(), dt_to.get_date().month(), dt_to.get_date().day(),
		dt_to.get_time().hour(), dt_to.get_time().minute(), dt_to.get_time().sec());

	_date_picker->SetTime(&ct);
	_time_from->SetTime(&ct);
	_time_to->SetTime(&tct);
}

