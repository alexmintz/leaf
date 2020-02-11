// md_monitor.h : main header file for the md_monitor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       
#include "znet/TCM_Star.h"
#include "leaf/LFStrategy.h"
#include "MainFrm.h"

// Cmd_monitorApp:
// See md_monitor.cpp for the implementation of this class
//
struct Cmd_Bootstrap
{
	Cmd_Bootstrap(const TCTime& bootstrap_from_) : _bootstrap_from(bootstrap_from_) {}
	TCTime _bootstrap_from;

};

class Cmd_monitorApp : public CWinApp
{
public:
	Cmd_monitorApp();
	virtual ~Cmd_monitorApp();

	Z::ActiveQueue& star()	{ return *_star; }
	CMainFrame*		mainframe() { return pMainFrame;}
	//PlayerDlgBar&	player(){ return pMainFrame->get_player();}
	const std::string& leader() const { return _leader_sec;}
	const std::string& follower() const { return _follower_sec;}
	LFStrategy* strategy() { return _lf_strategy.get(); }
// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	bool start_model();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnRun();
	afx_msg void OnModel();
	afx_msg void OnUpdateModel(CCmdUI* pCmdUI);

DECLARE_MESSAGE_MAP()

private:
	
	auto_ptr<Z::ActiveQueue>	_star;
    UINT_PTR					_timer;

	CMultiDocTemplate*			_md;
	CMultiDocTemplate*			_gd;
	CMultiDocTemplate*			_orders;
	CMultiDocTemplate*			_exec;
	CMultiDocTemplate*			_pos;
	CMultiDocTemplate*			_fill;
	CMultiDocTemplate*			_bt;

	CMainFrame*					pMainFrame;

	std::string					_leader_sec;
	std::string					_follower_sec;

	auto_ptr<LFStrategy>		_lf_strategy;

	auto_ptr<Z::ActiveCallback>	_model_callback;

	void start_model_bootstrap(Cmd_Bootstrap* b_);

};
//======================================================================
struct LeafSound
{
	LeafSound();

	static LeafSound* instance();	

	void play(LF::PositionAction pa_);

	typedef std::map<LF::PositionAction, std::string> SoundMap;
	SoundMap _smap;

	LF::PositionAction _action;
	bool _play_always;
};
//======================================================================
extern Cmd_monitorApp theApp;