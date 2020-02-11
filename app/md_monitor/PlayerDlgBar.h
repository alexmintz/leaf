#pragma once


// PlayerDlgBar dialog
#include "znet/TCBusinessDate.h"
#include "ClrButton.h"
#include "leaf/LFRunSettings.h"


class PlayerDlgBar : public CDialogBar
{
	DECLARE_DYNAMIC(PlayerDlgBar)

public:
	PlayerDlgBar();   // standard constructor
	virtual ~PlayerDlgBar();

	virtual BOOL Create(CWnd* pParent, UINT nIDTemplate, UINT nStyle, UINT nID);
	void OnPlay();
	void OnStop();
	void OnRun();
	void OnModel();
	void set_time_from(const ACE_Time_Value& _tv);
	void set_slider(bool running_);
	bool process_inactive();
	double get_mpos() const { return _mpos;}
// Dialog Data
	enum { IDD = IDD_DIALOGBAR };

	afx_msg void OnSliderSet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSliderDraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeTimepicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeTimepicker2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboBar();
	afx_msg void OnUpdateModel(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CMyDlgBar)
	afx_msg LRESULT OnInitDialog(WPARAM, LPARAM);
	//}}AFX_MSG
	void	Pause();
	void	Play();
	void	get_btr();
	void	set_btr(bool init = false);
	long	get_bar();
	long	convert_bar(const std::string& t_);
	void	gray_out(BOOL enable_, bool date_only_);

	void set_play	(bool _visible);
	void set_stop	(bool _visible);
	void set_pause	(bool _visible);

	CBitmapButton		_play_btn;
	CBitmapButton		_stop_btn;
	CDateTimeCtrl*		_date_picker;
	CDateTimeCtrl*		_time_from;
	CDateTimeCtrl*		_time_to;
	CSliderCtrl	*		_slider;
	CComboBox*			_bar_picker;

	TCBusinessTimeRange	_btr;
//	CTime				_strat_time;

	bool				_start;
	bool				_play;
	double				_pos;
	double				_mpos;
	long				_run;

//	bool				_rerun;

//	bool				_rerun_backtest;
//	bool				_rerun_live;

//	bool				_live_backtest;
//	bool				_live_run;

	std::vector<long>	_bar_time;

	DECLARE_MESSAGE_MAP()

public:
//	afx_msg void OnModelBtn(NMHDR *pNMHDR, LRESULT *pResult);
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
