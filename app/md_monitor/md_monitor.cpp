// md_monitor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "md_monitor.h"
#include "ChildFrm.h"
#include "md_monitorDoc.h"
#include "md_monitorView.h"
#include "md_graphView.h"
#include "p_orderView.h"
#include "p_orderFillView.h"
#include "p_RTPosView.h"
#include "batchView.h"
#include "VersionInfo.h"
#include "znet/ZActiveQueue.h"
#include "znet/TCIdGenerator.h"
#include "znet/ZFunctor.h"

#include "initDialog.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//======================================================================
void CALLBACK EXPORT tcm_timer_proc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	ACE_Time_Value tv;
	theApp.star().process_queue(&tv);
}
// Cmd_monitorApp

BEGIN_MESSAGE_MAP(Cmd_monitorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &Cmd_monitorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &Cmd_monitorApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	//ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_BN_CLICKED(IDC_BUTTON_PLAY,		OnPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP,		OnStop)
	ON_BN_CLICKED(IDC_BUTTON_RUN,		OnRun)
	ON_BN_CLICKED(IDC_BUTTON_MODEL1,	OnModel)
END_MESSAGE_MAP()


// Cmd_monitorApp construction

Cmd_monitorApp::Cmd_monitorApp()
:	_md(0),
	_gd(0),
	_orders(0),
	_exec(0),
	_pos(0),
	_fill(0),
	_bt(0),
	pMainFrame(0)
{
}
Cmd_monitorApp::~Cmd_monitorApp()
{
	_star.reset();
}
// The one and only Cmd_monitorApp object

Cmd_monitorApp theApp;


// Cmd_monitorApp initialization

BOOL Cmd_monitorApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
//ZNET
	try {

		Z::add_command_line_option("live", "If TRUE start md_monitor in LIVE trading");
		Z::add_command_line_option("strat_config", "strategy config XXX_stratname, where XXX is underlier");

		Z::init(std::string(CT2A(GetCommandLine())));


		TCFiler::instance()->set_mode(ios::app);
		Z::set_ignore_order(true);

		Z::add_setting("BMTS:root_dir", Z::get_setting("BMTS:dir", Z::get_setting("LOG:dir", ".")));
		_star.reset(new Z::ActiveQueue(0, "MonitorQueue"));

		if (Z::get_setting_bool("COMMAND:live"))
		{
			std::string config_file = Z::get_setting("COMMAND:strat_config");
			if (config_file.empty())
			{
				std::string err = "Absent -strat_config \"XXX_stratname\", where XXX is underlier, exiting...";
				z_throw(err);
			}
			std::size_t s = config_file.find_first_of('_');
			if (s == std::string::npos)
			{
				std::string err = "Wrong strat_config name format: " + config_file + " exiting...";
				z_throw(err);
			}

			TCSettings::rescan(config_file);

			if (Z::get_setting_double("LFModel:cut_loss_total", 0.) <= 0)
			{
				z_throw("attempt to start LIVE session without cut_loss_total, exit...");
			}

			RunSettings::instance()->set_live_run(true);
			RunSettings::instance()->set_trading(false);
			RunSettings::instance()->set_instance_disabled(false);

			TCTime endofday_t(Z::get_setting("LFStrategy:endofday", "17:00:00"));
			TCDateTime dt = TCDateTime::now();
			TCDate date = dt.get_date();
			if(dt.get_time() > endofday_t)
				date = tc_b_date->next_business_date(dt.get_date());

			std::string under = config_file.substr(0, s); // follower underlyer
			TCJulianDate jd(date);
			if (!tc_b_date->is_business(jd, under))
			{
				z_throw("TODAY IS EXCHANGE HOLIDAY, NO LIVE TRADING, exit...");
			}
			
			// Security setup
			_follower_sec = LFSecurityMaster::instance()->get_instr(under, date);
			_leader_sec = Z::get_setting("LFStrategy:leader", LFSecurityMaster::instance()->get_dxe(date));

			// Account setup
			long strategy_id = LFSecurityMaster::instance()->strategy_id(_follower_sec);
			long account_id = LFSecurityMaster::instance()->account_id(strategy_id);
			if (account_id < 0)
				z_throw("Account invalid");

			Z::add_setting("LFModel:date", date.as_string());
			Z::add_setting("LFStrategy:leader", _leader_sec);
			Z::add_setting("LFStrategy:follower", _follower_sec);



			if (AfxMessageBox(_T("LIVE TRADING, ARE YOU SURE?"), MB_OKCANCEL) != IDOK)
				z_throw("starting LIVE session by mistake, exit...");
		}
		else
		{
			init_Dlg idg;
			if(idg.DoModal() == -1)
				exit(0);
		}

		_leader_sec = TCSettings::get("LFStrategy:leader");
		_follower_sec = TCSettings::get("LFStrategy:follower");

		if (RunSettings::instance()->get_live_run())
		{
			std::string dir = Z::get_setting("BMTS:run_dir", Z::get_setting("LOG:dir", "."))
				+ "/" + Z::get_app_name();
			ACE_Dirent de;
			if(de.open(dir.c_str()) == -1)
				ACE_OS::mkdir(dir.c_str());
			de.close();
			Z::add_setting("BMTS:dir", dir);
			TCResourceMap	mmap;
			std::string base = "LFModel:";
			TCSettings::get_resource_match(base + "*", mmap);

			std::string fname = dir + "/model.cfg";
			std::ofstream l_stream(fname.c_str());
			if(l_stream.good())
			{
				for(TCResourceMap::iterator i = mmap.begin(); i != mmap.end(); ++i)
					l_stream << base << (*i).second->get_sub_name(1) << "\t" << (*i).second->get_value() << std::endl;
			}

			BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
			BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_CHANNEL;

			BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFInfo_Position>::instance()->_event_type_name;

			if (Z::get_setting_bool("LFStrategy:LFInfo_Position:in", false))
			{
				BMTS<LFInfo_Position>::instance()->_in = BMTS_Properties::IN_CHANNEL;
				BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFDepthData>::instance()->_worker_name;
//				BMTS<LFInfo_Position>::instance()->_out_file = false;
			}
			else if (Z::get_setting_bool("LFStrategy:LFInfo_Position:out", false))
			{
				BMTS<LFInfo_Position>::instance()->_out = BMTS_Properties::OUT_CHANNEL;
				//BMTS<LFInfo_Position>::instance()->_out_file = true;
			}

			BMTS<LFNewOrder>::instance()->_out_file = true;
			BMTS<LFOrderUpdate>::instance()->_out_file = true;
			BMTS<LFOrderFill>::instance()->_out_file = true;
			BMTS<LFPositionData>::instance()->_out_file = true;
			BMTS<LFRunStats>::instance()->_out_file = true;
			BMTS<LFRunStats>::instance()->_run_disabled = true;
			if (!RunSettings::instance()->get_live_backtest())
			{
				// live trading upates
				BMTS<LFOrderFill>::instance()->_in = BMTS_Properties::IN_CHANNEL;
				BMTS<LFOrderUpdate>::instance()->_in = BMTS_Properties::IN_CHANNEL;
				// live trading new/cancel/amend
				BMTS<LFNewOrder>::instance()->_out = BMTS_Properties::OUT_SENDER;
				BMTS<LFCancelOrder>::instance()->_out = BMTS_Properties::OUT_SENDER;
			}
		}
		else
		{
			BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_MIXER;
			BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_MIXER;

			BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFInfo_Position>::instance()->_event_type_name;

			std::string r_dir = Z::get_setting("LFStrategy:LFInfo_Position:root_dir", "");
			if (Z::get_setting_bool("LFStrategy:LFInfo_Position:in", false))
			{
				BMTS<LFInfo_Position>::instance()->_in = BMTS_Properties::IN_MIXER;
				BMTS<LFInfo_Position>::instance()->_worker_name = BMTS<LFDepthData>::instance()->_worker_name;
				BMTS<LFInfo_Position>::instance()->_out_file = false;
				if (!r_dir.empty())
					BMTS<LFInfo_Position>::instance()->_root_dir = r_dir;
			}
			else if (Z::get_setting_bool("LFStrategy:LFInfo_Position:out", false))
			{
				BMTS<LFInfo_Position>::instance()->_out_file = true;
				BMTS<LFInfo_Position>::instance()->_use_root_dir = true;
				if (!r_dir.empty())
					BMTS<LFInfo_Position>::instance()->_root_dir = r_dir;
			}

			BMTS<LFNewOrder>::instance()->_out_file = !RunSettings::instance()->get_rerun_backtest();
			BMTS<LFOrderUpdate>::instance()->_out_file = !RunSettings::instance()->get_rerun_backtest();
			BMTS<LFOrderFill>::instance()->_out_file = !RunSettings::instance()->get_rerun_backtest();
			BMTS<LFPositionData>::instance()->_out_file = !RunSettings::instance()->get_rerun_backtest();
			BMTS<LFRunStats>::instance()->_out_file = !RunSettings::instance()->get_rerun_backtest();
			BMTS<LFRunStats>::instance()->_run_disabled = true;
			if (RunSettings::instance()->get_rerun_backtest())
			{
				BMTS<LFNewOrder>::instance()->_in = BMTS_Properties::IN_MIXER;
				BMTS<LFOrderUpdate>::instance()->_in = BMTS_Properties::IN_MIXER;
				BMTS<LFOrderFill>::instance()->_in = BMTS_Properties::IN_MIXER;
				BMTS<LFPositionData>::instance()->_in = BMTS_Properties::IN_MIXER;
			}
		}

		//instance of lfstrategy
		if(!RunSettings::instance()->get_instance_disabled())
		{
			_lf_strategy.reset(new LFStrategy(_leader_sec, _follower_sec));
			_lf_strategy->open();
		}
		else
		{
			if (RunSettings::instance()->get_live_run())
				BMTS<LFStrategyData>::instance()->_in = BMTS_Properties::IN_CHANNEL;
			else
				BMTS<LFStrategyData>::instance()->_in = BMTS_Properties::IN_MIXER;
		}
	}
	catch(Z::Exception& e)
	{
		AfxMessageBox(CA2T(e.get_error_msg()), MB_ICONSTOP);
		return FALSE;
	}
//ZNET
	SetRegistryKey(_T("LEAF"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views


	_md = new CMultiDocTemplate(IDR_md_monitorTYPE,
		RUNTIME_CLASS(DOM_Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(Cmd_monitorView));
	if (!_md)
		return FALSE;
	AddDocTemplate(_md);

	_gd = new CMultiDocTemplate(IDD_GRAPH,
		RUNTIME_CLASS(GRAPH_Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(Cmd_graphView));
	if (!_gd)
		return FALSE;
	AddDocTemplate(_gd);

	_fill = new CMultiDocTemplate(IDR_md_monitorTYPE,
		RUNTIME_CLASS(FILL_Doc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(Cp_OrderFillView));
	if (!_fill)
		return FALSE;
	AddDocTemplate(_fill);
	if (!RunSettings::instance()->get_live_run() && !RunSettings::instance()->get_live_backtest())
	{
		_bt = new CMultiDocTemplate(IDR_md_monitorTYPE,
									RUNTIME_CLASS(BATCH_Doc),
									RUNTIME_CLASS(CChildFrame), // custom MDI child frame
									RUNTIME_CLASS(Cp_BatchView));
		if (!_bt)
			return FALSE;
		AddDocTemplate(_bt);
	}
	// create main MDI Frame window
	pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	_star->open();

	_timer = SetTimer(0,0, Z::get_setting_integer("LFStrategy:refresh_gui", 100), tcm_timer_proc);
	pMainFrame->get_player().OnCbnSelchangeComboBar();

	start_model();
	//_model_callback.reset(new Z::ActiveCallback(Z::make_callback(this, &Cmd_monitorApp::start_model), *_star));
	//_model_callback->open();
	//_model_callback->notify();


	return TRUE;
}

//======================================================================
bool Cmd_monitorApp::start_model()
{
	TCTime live_bootstrap_from = TCTime(Z::get_setting("LFStrategy:live_bootstrap_from", "6:00:00"));
	TCDateTime dt_now = TCDateTime::now();
	if (!RunSettings::instance()->get_live_run()
		|| live_bootstrap_from.as_hms() == 0
		|| dt_now.get_time() < live_bootstrap_from
	)
	{
		BMModel::instance()->open();
	}
	else
	{
		Z::register_local_handler(this, &Cmd_monitorApp::start_model_bootstrap);
		Z::l_send(new Cmd_Bootstrap(live_bootstrap_from));
	}
	return TRUE;
}
//======================================================================
void Cmd_monitorApp::start_model_bootstrap(Cmd_Bootstrap* b_)
{
	auto_ptr<Cmd_Bootstrap> b(b_);
	BMTS_BackupMap bmts;
	LF::backup_all_bmts(bmts);
	for(BMTS_BackupMap::iterator i = bmts.begin(); i != bmts.end(); ++i)
	{
		(*i).second->_p->_in = BMTS_Properties::IN_NONE;
		(*i).second->_p->_out = BMTS_Properties::OUT_NONE;
		(*i).second->_p->_out_file = false;
	}

	BMTS<LFDepthData>::instance()->_in = BMTS_Properties::IN_MIXER;
	BMTS<LFDepthData>::instance()->_root_dir = Z::get_setting("BMTS:root_dir");

	BMTS<LFTradeData>::instance()->_in = BMTS_Properties::IN_MIXER;
	BMTS<LFTradeData>::instance()->_root_dir = Z::get_setting("BMTS:root_dir");


	Z::report_info("bootstrap -- opening model...");
	BMModel::instance()->open(true);
	TCDateTime dt_now = TCDateTime::now();
	//ACE_Time_Value t_now = dt_now.get_ace_time();
	//ACE_Time_Value t_from = t_now - LF::t_from_length(b->_bootstrap_l);

	TCDateTime dt_from(dt_now.get_date(), b->_bootstrap_from);
	ACE_Time_Value t_from = dt_from.get_ace_time();

	TCDateTime dt_to(dt_now.get_date(), TCTime(235959));
	ACE_Time_Value t_to = dt_to.get_ace_time();

	TCBusinessTimeRange btr(t_from, t_to);
	if(btr.vec().size() > 1)
		btr.init(TCDateTime(dt_now.get_date(), TCTime()).get_ace_time(), t_to);

	Z::report_info("bootstrap -- starting mixer...");
	BM_MIXER->start(btr, -1, -1, false);
	Z::report_info("bootstrap -- waiting on mixer...");
	BM_MIXER->wait();
	Z::report_info("bootstrap -- stopping mixer...");
	BM_MIXER->stop(false);

	Z::report_info("bootstrap -- sleeping 2 sec ...");
	ACE_OS::sleep(2);
	Z::report_info("bootstrap -- closing model...");
	BMModel::instance()->close(false, false);


	for(BMTS_BackupMap::iterator i = bmts.begin(); i != bmts.end(); ++i)
	{
		*((*i).second->_p) = (*i).second->_backup;
	}


	Z::report_info("life -- opening model...");
	BMModel::instance()->open();

}

//======================================================================
int Cmd_monitorApp::ExitInstance()
{
	if(_timer != 0)
	{
		KillTimer(0, _timer);
		_timer = 0;
	}
	if(!BMModel::instance()->is_live())
	{
		Z::report_debug("---stopping MIXER");
		BM_MIXER->stop();
	}
	else
	{
		Z::unregister_local_handler<Cmd_Bootstrap>();
	}
	Z::report_debug("---closing BMModel");
	BMModel::instance()->close();
	Z::report_debug("---closing star");
	_star->close();
	if(_model_callback.get() != 0)
		_model_callback->close(true);
	if(_lf_strategy.get() != 0)
	{
		Z::report_debug("---closing lf_strategy");
		_lf_strategy->close();
		_lf_strategy.reset();
	}
	Z::report_debug("---reseting star");
	_star.reset();
	Z::report_debug("---finilizing");
	Z::fini();
	return CWinApp::ExitInstance();
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
	CStatic about_label;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CVersionInfo verInfo;

	CString s;
	s.Format(_T("%s\r\n%s\r\n%s"),
		verInfo.GetProductName().operator LPCTSTR(),
		verInfo.GetLegalCopyright().operator LPCTSTR(),
		verInfo.GetProductVersionAsString().operator LPCTSTR());
	about_label.SetWindowText( s );

    UpdateData(FALSE);

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT, about_label);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void Cmd_monitorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
//======================================================================
void Cmd_monitorApp::OnFileNew()
{
	std::string title;

	CDocument* fill = _fill->OpenDocumentFile(NULL);
	title = "FILL " + _leader_sec + " : " + _follower_sec;
	fill->SetTitle(CA2T(title.c_str()));

	CDocument* dog = _gd->OpenDocumentFile(NULL);
	std::string& id = Z::get_setting("LFStrategy:strategy_id");
	long cqg_AccountID = LFSecurityMaster::instance()->account_id(atoi(id.c_str()));

	std::ostringstream os;
	os << cqg_AccountID << " "
		<< _leader_sec << " : "
		<< _follower_sec << " (" << id << ") max qty "
		<< Z::get_setting("LFModel:position_limit");

	dog->SetTitle(CA2T(os.str().c_str()));

	CDocument* doc = _md->OpenDocumentFile(NULL);
	title = "DOM " + _leader_sec + " : " + _follower_sec;
	doc->SetTitle(CA2T(title.c_str()));

	if (!RunSettings::instance()->get_live_run() && !RunSettings::instance()->get_live_backtest())
	{
		CDocument* bat = _bt->OpenDocumentFile(NULL);
		title = "BATCH " + _leader_sec + " : " + _follower_sec;
		bat->SetTitle(CA2T(title.c_str()));
	}
	if (RunSettings::instance()->get_rerun_backtest())
	{
		if (RunSettings::instance()->get_rerun_live())
		{
			std::string live_dir = "/MD_LIVE_" + Z::get_setting("LFStrategy:follower_underlying");

			BMTS<LFNewOrder>::instance()->_root_dir = BMTS<LFNewOrder>::instance()->_run_dir+live_dir;
			BMTS<LFOrderUpdate>::instance()->_root_dir = BMTS<LFNewOrder>::instance()->_run_dir + live_dir;
			BMTS<LFOrderFill>::instance()->_root_dir = BMTS<LFNewOrder>::instance()->_run_dir + live_dir;
			BMTS<LFPositionData>::instance()->_root_dir = BMTS<LFNewOrder>::instance()->_run_dir + live_dir;
		}
		mainframe()->get_player().OnRun();
		return;
	}
}
//======================================================================
void Cmd_monitorApp::OnPlay()
{
}
//======================================================================
void Cmd_monitorApp::OnStop()
{
}
//======================================================================
void Cmd_monitorApp::OnRun()
{
}
//======================================================================
void Cmd_monitorApp::OnModel()
{
}
//======================================================================
//======================================================================
LeafSound* LeafSound::instance()
{
	return ACE_Singleton <LeafSound, ACE_Thread_Mutex>::instance();
}
//======================================================================
LeafSound::LeafSound()
: _action(LF::paUndefined),
  _play_always(false)
{
	if(_smap.size() > 0)
		return;

	std::string cdir = TCSettings::get("COMMAND:z_config_dir");

	_smap[LF::paSharp] = cdir + TCSettings::get("LFSound:Sharp");
	_smap[LF::paPreEnter] = cdir + TCSettings::get("LFSound:PreEnter");
	_smap[LF::paEnter] = cdir + TCSettings::get("LFSound:Enter");

	_smap[LF::paPreAverage] = cdir + TCSettings::get("LFSound:PreAverage");
	_smap[LF::paAverage] = cdir + TCSettings::get("LFSound:Average");

	_smap[LF::paPreCutLoss] = cdir + TCSettings::get("LFSound:PreCutLoss");
	_smap[LF::paCutLoss] = cdir + TCSettings::get("LFSound:CutLoss");

	_smap[LF::paPreProfit] = cdir + TCSettings::get("LFSound:PreProfit");
	_smap[LF::paProfit] = cdir + TCSettings::get("LFSound:Profit");

	_smap[LF::paPreTrendEnd] = cdir + TCSettings::get("LFSound:PreTrendEnd");
	_smap[LF::paTrendEnd] = cdir + TCSettings::get("LFSound:TrendEnd");

	_play_always = TCSettings::get_bool("LFSound:play_always", false)? true:(theApp.mainframe()->get_player().get_mpos() == 1);

}
//======================================================================
void LeafSound::play(LF::PositionAction pa_)
{
	if(_smap.size() == 0)
		return;
	if(pa_ == _action)
		return;
	_action = pa_;
	if(pa_ != LF::paSharp)
		return;
	std::string& s = _smap[pa_];
	if(!s.empty() && _play_always)
		PlaySound(CA2T(s.c_str()), NULL, SND_NODEFAULT | SND_ASYNC | SND_FILENAME | SND_NOSTOP | SND_NOWAIT);
}
//======================================================================