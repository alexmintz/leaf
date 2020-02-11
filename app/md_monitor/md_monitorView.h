// md_monitorView.h : interface of the Cmd_monitorView class
//

#ifndef MD_MONITOR_VIEW_H
#define MD_MONITOR_VIEW_H

#include "znet/ZNet.h"
#include "znet/TCPQP.h"
#include "znet/BMStrategy.h"
#include "leaf/LFData.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "ClrButton.h"
#include "md_monitorDoc.h"
#include "Resource.h"

//======================================================================
struct compare_double: public std::binary_function<double, double, bool>
{
	bool operator()(double p1_, double p2_) const
	{ return TCComparePrice::diff(p1_, p2_) > 0;}
};

class Cp_monView : public ETSLayoutFormView
{
protected:
	Cp_monView(	TC_ListCtrlBase* ctrl1_,
				TC_ListCtrlBase* ctrl2_,
				TC_ListCtrlBase* ctrl3_,
				TC_ListCtrlBase* ctrl4_,
				UINT nID = IDD_MD_REPORT)
		: ETSLayoutFormView(nID),
		_ctrl1(ctrl1_), _ctrl2(ctrl2_), _ctrl3(ctrl3_), _ctrl4(ctrl4_), _rows(0),
		_center_ctrl1(true), _center_ctrl2(true){}

public:
	virtual ~Cp_monView(){}
	virtual void OnInitialUpdate();

protected:
	void recalc_size();
	virtual void DoDataExchange(CDataExchange* pDX)    // DDX/DDV support
	{
		ETSLayoutFormView::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_REC_LIST1, *_ctrl1);
		DDX_Control(pDX, IDC_REC_LIST2, *_ctrl2);
		DDX_Control(pDX, IDC_REC_LIST3, *_ctrl3);
		DDX_Control(pDX, IDC_REC_LIST4, *_ctrl4);
	}
	virtual void update_layout();

	DECLARE_MESSAGE_MAP()

	auto_ptr<TC_ListCtrlBase>		_ctrl1;
	auto_ptr<TC_ListCtrlBase>		_ctrl2;
	auto_ptr<TC_ListCtrlBase>		_ctrl3;
	auto_ptr<TC_ListCtrlBase>		_ctrl4;
	size_t							_rows;
	bool							_center_ctrl1;
	bool							_center_ctrl2;
};
//============================================================================
enum TickType { BID, BEST_BID, ASK, BEST_ASK, SPREAD, SHORT_SPREAD };
struct DOM_Record
{
	DOM_Record():_price(0), _trades(0), _l_trade(0), _factor(1){}
	DOM_Record(const TC_pqp& pqp_, TickType tick_type_, int factor_ = 1)
		:_price(TCCompare::NaN), _trades(0), _l_trade(0), _factor(factor_)
	{
		_price = pqp_._p*factor_;
		_qty_type.first = pqp_._q;
		_qty_type.second = tick_type_;
	}
	DOM_Record(double price_, TickType tick_type_, int factor_ = 1)
		:_price(TCCompare::NaN), _trades(0), _l_trade(0), _factor(factor_)
	{
		_price = price_*factor_;
		_qty_type.first = 0;
		_qty_type.second = tick_type_;
	}
	double						_price;
	std::pair<double, TickType>	_qty_type;
	double						_trades;
	double						_l_trade;
	int							_factor;
};
//============================================================================
class Cmd_monitorView : public Cp_monView
{
	typedef Z::smart_ptr<LFDepthData>		LFD_Ptr;

	class Ctrl : public TC_DataListCtrl<DOM_Record, double>
	{
		typedef std::vector<double> TradeVec;
		typedef std::vector<TC_pqp> DepthVec;
		typedef TC_DataListCtrl<DOM_Record, double>	BASE;
		typedef BASE::Record						Record;
	public:
		Ctrl();

		void process_data(LFDepthDataPtr*);
		void process_trade(LFTradeDataPtr*);
		void display_data();
		void set_volume(BASE::R_Vec& r_vec_);
		void set_data(BASE::R_Vec& r_vec);
		void center() {_center = true;}
		void recalc_size();
		double best_bid() const;
		double best_ask() const;
		void set_pos(LFPosition* pos_) {_pos.reset(pos_);}
		LFPosition* get_pos() {return _pos.get();}
		void reset();
		virtual bool edit(CPoint point);

	private:
		virtual bool set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const;
		virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._price;  return true; }
		int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_REPORT | LVS_EX_GRIDLINES;}

		auto_ptr<LFPosition>	_pos;
		LFDepthDataPtr	_last_depth;
		TC_pqp			_last_trade;
		DepthVec		_pqpv_bid;
		DepthVec		_pqpv_ask;
		double			_high_ask;
		double			_low_bid;
		double			_tick_size;
		int				_pf;
		size_t			_rows;
		TradeVec		_tvec;
		double			_lowt;
		double			_hight;
		bool			_center;
		size_t			_tvec_offset;
	};


	class PosCtrl : public TC_DataListCtrl<LFPositionData, std::string>
	{
		typedef TC_DataListCtrl<LFPositionData, std::string>		BASE;
		typedef BASE::Record Record;
	public:
		PosCtrl();
		bool process_run(LFPositionDataPtr);
		bool change_run();

		virtual std::string filter(std::string& rec_) const { return rec_; }

	private:
		virtual bool fill_id(const R& r_, V_ID& id_) {id_ = r_._strategy_key+r_._instr; return true; }

		int exstyle() const { return LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
		virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;

		//auto_ptr<LFStrategyRequest>	_req;
	};

	class OrderCtrl : public TC_DataListCtrl<LFOrderUpdate, std::string>
	{
		public:
			OrderCtrl();
			bool process_order(LFOrderUpdatePtr*);
			bool process_run();
			bool change_run();
			virtual std::string filter(std::string& rec_) const {return rec_;}
			afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	private:
			typedef TC_DataListCtrl<LFOrderUpdate, long>		BASE;
			typedef BASE::Record Record;

			virtual bool fill_id(const LFOrderUpdate& r_, V_ID& id_)
			{
				if (r_._order_id <= 0)
				{
					id_ = r_._cqg_OriginalOrderId;
				}
				else
				{
					std::string& id = _id_map[r_._order_id];
					if (id.empty())
					{
						std::ostringstream os;
						os << r_._order_id << "_LF";
						id = os.str();
					}
					id_ = id;
				}
				return true;
			}

			int exstyle() const { return LVS_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP; }
			virtual bool set_color(Record*, INT_PTR, COLORREF &, COLORREF &) const;

			BASE::R_Vec r_vec;
			typedef std::map<long, std::string> OrderIdMap;
			OrderIdMap _id_map;
	};

public:

	Cmd_monitorDoc* GetDocument() const;
	virtual void OnInitialUpdate();

	bool process_bt_run();
	bool process_rt_run();

	bool change_run();
	void init_buttons();
	void enable_buy_sell_buttons(bool enable_);
	void enable_position_buttons(bool enable_);
	void set_buttons_price();
	void set_limit_buttons();
	void set_buttons_qty();
	void set_go_button();

	void process_rtpos(LFPositionDataPtr*);
	void process_data(LFDepthDataPtr* p_);
	void process_order(LFOrderUpdatePtr*);

	afx_msg void OnClickFollower(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickOrder(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBuyLmtAsk();
	afx_msg void OnBuyLmtBid();
	afx_msg void OnBuyMkt();
	afx_msg void OnSellLmtAsk();
	afx_msg void OnSellLmtBid();
	afx_msg void OnSellMkt();
	afx_msg void OnQtyChanged();

	afx_msg void OnQty1();
	afx_msg void OnQty2();
	afx_msg void OnQty3();
	afx_msg void OnQty4();

	afx_msg void OnAverage();
	afx_msg void OnProfit();
//	afx_msg void OnScratch();
	afx_msg void OnLiquidate();
	afx_msg void OnLiquidateAll();

	afx_msg void OnAverageReset();
	afx_msg void OnProfitReset();

	afx_msg void OnBuyLmt();
	afx_msg void OnSellLmt();

	afx_msg void OnCancel();
	afx_msg void OnCancelAll();

	afx_msg void OnBooking();

	afx_msg void OnGo();
	//afx_msg void OnGoReset();

	void OnAverageResetGUI();
	void OnProfitResetGUI();

	afx_msg void OnSuspend();
	afx_msg void OnResume();

protected: // create from serialization only
	Cmd_monitorView();
	virtual ~Cmd_monitorView();

   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_DYNCREATE(Cmd_monitorView)

	void open_strategy();
	void close_strategy();
	void set_qty_text();

	void set_action_buttons_color();

	Ctrl* ctrl1() {return (Ctrl*)_ctrl1.get(); }
	Ctrl* ctrl2() {return (Ctrl*)_ctrl2.get(); }
	PosCtrl* ctrl3() {return (PosCtrl*)_ctrl3.get(); }
	OrderCtrl* ctrl4() {return (OrderCtrl*)_ctrl4.get(); }

	DECLARE_MESSAGE_MAP()

private:
	typedef std::vector<double> TradeVec;

	auto_ptr<BMStrategy>	_s;

//	afx_msg void OnSysColorChange();
	CClrButton	buy_lmt_ask;
	CClrButton	buy_lmt_bid;
	CClrButton	buy_mkt;
	CClrButton	buy_lmt;

	CClrButton	sell_lmt_ask;
	CClrButton	sell_lmt_bid;
	CClrButton	sell_mkt;
	CClrButton	sell_lmt;

	CClrButton	cancel_b;
	CClrButton	cancel_all_b;
	CClrButton	book_b;
	CClrButton	go_b;
//	CClrButton	go_br;

	unsigned int	qty_edit, qty1, qty2, qty3, qty4;
	DOM_Record		limit_rec;
	CEdit			qty_e;
	CButton			qty_1;
	CButton			qty_2;
	CButton			qty_3;
	CButton			qty_4;

	CClrButton	average_b;
	CClrButton	average_br;

	CClrButton	profit_b;
	CClrButton	profit_br;

	CClrButton	liquidate_b;
	CClrButton	liquidate_ball;

	CClrButton	suspend_b;
	CClrButton	resume_b;

	LFPositionDataPtr	_last_pos;
	bool				_booking;
	bool				_suspended;
	LFCluster			_sl;
};

#endif //#ifndef MD_MONITOR_VIEW_H