// md_monitorView.cpp : implementation of the Cmd_monitorView class
//

#include "stdafx.h"
#include "md_monitor.h"

#include "md_monitorDoc.h"
#include "md_monitorView.h"
#include "znet/ZActiveQueue.h"
#include "znet/ZFunctor.h"
#include "znet/ZNet.h"
#include "znet/TCTAO.h"
#include "leaf/LFSide.h"
#include "p_format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(Cmd_monitorView, Cp_monView)

BEGIN_MESSAGE_MAP(Cp_monView, ETSLayoutFormView)
	//{{AFX_MSG_MAP(ETSLayoutFormView)
	ON_WM_GETMINMAXINFO()
	//ON_WM_ERASEBKGND()
	//ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(Cmd_monitorView, Cp_monView)
	ON_BN_CLICKED(IDC_BUY_BUTTON1, OnBuyLmtAsk)
	ON_BN_CLICKED(IDC_BUY_BUTTON2, OnBuyLmtBid)
	ON_BN_CLICKED(IDC_SELL_LMT_BUTTON, OnSellLmt)
	ON_BN_CLICKED(IDC_BUY_LMT_BUTTON, OnBuyLmt)
	ON_BN_CLICKED(IDC_BUY_BUTTON3, OnBuyMkt)
	ON_BN_CLICKED(IDC_SELL_BUTTON1, OnSellLmtAsk)
	ON_BN_CLICKED(IDC_SELL_BUTTON2, OnSellLmtBid)
	ON_BN_CLICKED(IDC_SELL_BUTTON3, OnSellMkt)
	ON_BN_CLICKED(IDC_QTY_1, OnQty1)
	ON_BN_CLICKED(IDC_QTY_2, OnQty2)
	ON_BN_CLICKED(IDC_QTY_3, OnQty3)
	ON_BN_CLICKED(IDC_QTY_4, OnQty4)
	ON_BN_CLICKED(IDC_AQ_BUTTON, OnAverage)
	ON_BN_CLICKED(IDC_AQ_RESET, OnAverageReset)
	ON_BN_CLICKED(IDC_PQ_BUTTON, OnProfit)
	ON_BN_CLICKED(IDC_PQ_RESET, OnProfitReset)
	ON_BN_CLICKED(IDC_LIQUIDATE_BUTTON, OnLiquidate)
	ON_BN_CLICKED(IDC_LIQUIDATEALL_BUTTON, OnLiquidateAll)
	ON_BN_CLICKED(IDC_SUSPEND_BUTTON, OnSuspend)
	ON_BN_CLICKED(IDC_RESUME_BUTTON, OnResume)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancel)
	ON_BN_CLICKED(IDC_GO_BUTTON, OnGo)
//	ON_BN_CLICKED(IDC_GO_RESET, OnGoReset)
	ON_BN_CLICKED(IDC_CANCEL_ALL_BUTTON, OnCancelAll)
	ON_BN_CLICKED(IDC_BOOK_BUTTON, OnBooking)
	ON_EN_CHANGE(IDC_QTY_EDIT, OnQtyChanged)
//	ON_EN_UPDATE(IDC_QTY_EDIT, OnQtyChanged)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_REC_LIST2, OnClickFollower) 
	ON_NOTIFY(NM_CLICK, IDC_REC_LIST4, OnClickOrder) 
END_MESSAGE_MAP()

struct t_p_col{ 
	void operator()(const double& v_, COLORREF& clrText_, COLORREF& clrTextBk_) const 
	{
		clrText_ = 0xFF000000;
		clrTextBk_ = 0xFF000000;
	}  
};
struct compute_qty { 
	std::string operator()(const std::pair<double, TickType>& pqty) const {return vc_to_string(pqty.first); } 
};
struct b_a_col{ 
	void operator()(const std::pair<double, TickType>& v_, COLORREF& clrText_, COLORREF& clrTextBk_) const 
	{
		switch(v_.second)
		{
		case ASK:
			clrText_ = TC_COL::Red;
			break;
		case BID:
			clrText_ = TC_COL::Green;
			break;
		case BEST_ASK:
			clrTextBk_ = TC_COL::Red;
			clrText_ = TC_COL::White;
			break;
		case BEST_BID:
			clrTextBk_ = TC_COL::Green;
			clrText_ = TC_COL::White;
			break;
		}	
	} 
};

void Cp_monView::OnInitialUpdate()
{
	update_layout();
	ETSLayoutFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);
//	update_layout();
}
void Cp_monView::update_layout()
{
	UpdateLayout(
		pane(VERTICAL)
			<<(pane(HORIZONTAL, GREEDY|ABSOLUTE_VERT) << item(IDC_REC_LIST3)) // top list for Strategy
			
			<<(pane(HORIZONTAL)
				<< (pane(VERTICAL, GREEDY|ABSOLUTE_HORZ) 
					<< item(IDC_BUY_BUTTON1, NORESIZE | ALIGN_VCENTER) 
					<< item(IDC_BUY_BUTTON2, NORESIZE | ALIGN_VCENTER) 
					<< item(IDC_BUY_BUTTON3, NORESIZE | ALIGN_VCENTER)
					<< item(paneNull, GREEDY)
					<< item(IDC_BUY_LMT_BUTTON, NORESIZE | ALIGN_VCENTER)
					<< item(paneNull, GREEDY)
					<< item(IDC_LIQUIDATE_BUTTON, NORESIZE | ALIGN_VCENTER)
					<< item(IDC_LIQUIDATEALL_BUTTON, NORESIZE | ALIGN_VCENTER)
					)
					
				<< (pane(VERTICAL)
					<< (pane(HORIZONTAL) 
						<< (pane(VERTICAL) << item(IDC_REC_LIST1, GREEDY, 0,0, 50, 200))
						<< (pane(VERTICAL) << item(IDC_REC_LIST2, GREEDY, 0,0, 50, 200))
						)
					)
				<< (pane(VERTICAL, GREEDY|ABSOLUTE_HORZ) 
					<< item(IDC_SELL_BUTTON1, NORESIZE | ALIGN_VCENTER) 
					<< item(IDC_SELL_BUTTON2, NORESIZE | ALIGN_VCENTER) 
					<< item(IDC_SELL_BUTTON3, NORESIZE | ALIGN_VCENTER)
					<< item(paneNull, GREEDY)
					<< item(IDC_SELL_LMT_BUTTON, NORESIZE | ALIGN_VCENTER)
					<< item(paneNull, GREEDY)
					<< item(IDC_SUSPEND_BUTTON, NORESIZE | ALIGN_VCENTER)
					<< item(IDC_RESUME_BUTTON, NORESIZE | ALIGN_VCENTER)
					)
				)
				
			<< (pane(HORIZONTAL, GREEDY|ABSOLUTE_VERT) 
				<< item( paneNull, GREEDY)
				<< (pane(HORIZONTAL, GREEDY, 0)
					<< item(IDC_AQ_RESET, GREEDY |ABSOLUTE_HORZ | ALIGN_VCENTER)
					<< item(IDC_AQ_BUTTON, NORESIZE | ALIGN_VCENTER)
					)
		
				<< item(IDC_QTY_EDIT, NORESIZE | ALIGN_VCENTER)
				<< item(IDC_QTY_1, NORESIZE | ALIGN_CENTER)
				<< item(IDC_QTY_2, NORESIZE | ALIGN_CENTER)
				<< item(IDC_QTY_3, NORESIZE | ALIGN_CENTER)
				<< item(IDC_QTY_4, NORESIZE | ALIGN_CENTER)
		
				<< (pane(HORIZONTAL, GREEDY, 0)
					<< item(IDC_PQ_BUTTON, NORESIZE | ALIGN_CENTER)
					<< item(IDC_PQ_RESET, GREEDY |ABSOLUTE_HORZ | ALIGN_CENTER)
					)
				<< item(paneNull, GREEDY)
				)
				<< (pane(HORIZONTAL, GREEDY|ABSOLUTE_VERT)
					<< (pane(VERTICAL, GREEDY|ABSOLUTE_HORZ, 0)
						<< item(IDC_GO_BUTTON, NORESIZE | ALIGN_VCENTER)
						//<< item(IDC_GO_RESET, GREEDY |ABSOLUTE_VERT | ALIGN_VCENTER)
						<< item(paneNull, GREEDY)
						)
					<< (pane(VERTICAL, GREEDY|ABSOLUTE_VERT) 
						<< item(IDC_REC_LIST4)
						)
					<< (pane(VERTICAL, GREEDY|ABSOLUTE_HORZ) 
						<< item(IDC_CANCEL_ALL_BUTTON, NORESIZE | ALIGN_VCENTER)
						<< item(IDC_CANCEL_BUTTON, NORESIZE | ALIGN_VCENTER)
						<< item(paneNull, GREEDY)
						<< item(IDC_BOOK_BUTTON, NORESIZE | ALIGN_VCENTER)
						)
					)
	);
}
void Cmd_monitorView::OnClickFollower(NMHDR* pNMHDR, LRESULT* pResult)
{
	int row = ctrl2()->GetNextItem(-1, LVNI_SELECTED);
	if(row == -1)
		return;
	limit_rec = *(ctrl2()->get_record(row));
	set_limit_buttons();
}
void Cmd_monitorView::OnClickOrder(NMHDR* pNMHDR, LRESULT* pResult)
{
	int row = ctrl4()->GetNextItem(-1, LVNI_SELECTED);
	if(row == -1)
		return;
	LFOrderUpdate* order = ctrl4()->get_record(row);
	LF::OrderStatus s = order->_order_status;
	if(	s == LF::osInOrderBook	||
		s == LF::osInTransit	||
		s == LF::osInOrderBook	||
		s == LF::osInModify		||
		s == LF::osParked		||
		s == LF::osContingent	||
		s == LF::osActiveAt)
		cancel_b.EnableWindow(TRUE);
	else
		ctrl4()->SetItemState(row, 0, LVIS_SELECTED | LVIS_FOCUSED);

}
void Cmd_monitorView::OnCancel()
{
	POSITION pos = ctrl4()->GetFirstSelectedItemPosition();
	if(pos == 0)
		return;
		
	while(pos)
	{
		int row = ctrl4()->GetNextSelectedItem(pos);
		LFOrderUpdate* order = ctrl4()->get_record(row);
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_cancel_id = order->_order_id;
		BMModel::instance()->send(r.release());
	}
	cancel_b.EnableWindow(FALSE);
}
void Cmd_monitorView::OnGo()
{
	if(_last_pos.get() == 0 || _last_pos->_strategy_id == 0)
		return;

	auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
	r->_t = _s->get_input_time();
	r->_order_side = _sl._side->value();
	r->_st_action = LF::StrategyActionType(_sl._strength);
	r->_q = qty_edit;

	go_b.SetColour(TC_COL::Black, TC_COL::LightGreen, TC_COL::LightGrey);
	r->_st_active = true;
	BMModel::instance()->send(r.release());
}

void Cmd_monitorView::OnCancelAll()
{
	auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
	r->_t = _s->get_input_time();
	r->_cancel_id = 0;
	BMModel::instance()->send(r.release());

	cancel_all_b.EnableWindow(FALSE);
}
void Cmd_monitorView::OnBooking()
{
	if(_last_pos.get() == 0)
		return;
	_booking = !_booking;
	set_action_buttons_color();
}
void Cmd_monitorView::OnBuyLmt()
{
	if(limit_rec._price == 0)
		return;
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_BUY;
		r->_limit_p = limit_rec._price/limit_rec._factor;
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_BUY, qty_edit, LF::otLimit, 0, limit_rec._price/limit_rec._factor);
	}
	buy_lmt.EnableWindow(false);
	sell_lmt.EnableWindow(false);
}
void Cmd_monitorView::OnSellLmt()
{
	if(limit_rec._price == 0)
		return;
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_SELL;
		r->_limit_p = limit_rec._price/limit_rec._factor;
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_SELL, qty_edit, LF::otLimit, 0, limit_rec._price/limit_rec._factor);
	}
	buy_lmt.EnableWindow(false);
	sell_lmt.EnableWindow(false);
}
void Cmd_monitorView::set_limit_buttons()
{
	std::ostringstream osq;
	std::string label;
	std::string prefix;

	osq << qty_edit;

	if(limit_rec._price == 0)
	{

		prefix = osq.str() + "\n--" + "\nLMT DAY";

		label = "\nBuy " + prefix;
		buy_lmt.SetWindowText(CA2T(label.c_str()));

		label = "\nSell " + prefix;
		sell_lmt.SetWindowText(CA2T(label.c_str()));

		buy_lmt.EnableWindow(false);
		sell_lmt.EnableWindow(false);
	}
	else
	{
		std::ostringstream osa;

		osa << limit_rec._price;
		prefix = osq.str() + "\n@" + osa.str() + "\nLMT DAY";

		label = "\nBuy " + prefix;
		buy_lmt.SetWindowText(CA2T(label.c_str()));

		label = "\nSell " + prefix;
		sell_lmt.SetWindowText(CA2T(label.c_str()));

		buy_lmt.EnableWindow(true);
		sell_lmt.EnableWindow(true);
	}
}
void Cmd_monitorView::OnSize(UINT nType, int cx, int cy)
{
	ETSLayoutFormView::OnSize(nType, cx, cy);
	if (::IsWindow(_ctrl1->GetSafeHwnd()) && ::IsWindow(_ctrl2->GetSafeHwnd()))
	{
		ctrl1()->center();
		ctrl1()->display_data();
		
		ctrl2()->center();
		ctrl2()->display_data();
	}
}
void Cmd_monitorView::OnInitialUpdate()
{
	Cp_monView::OnInitialUpdate();

	buy_lmt_ask.SubclassDlgItem(IDC_BUY_BUTTON1, this);
	buy_lmt_bid.SubclassDlgItem(IDC_BUY_BUTTON2, this);

	buy_mkt.SubclassDlgItem(IDC_BUY_BUTTON3, this);
	buy_lmt.SubclassDlgItem(IDC_BUY_LMT_BUTTON, this);

	sell_lmt_ask.SubclassDlgItem(IDC_SELL_BUTTON1, this);
	sell_lmt_bid.SubclassDlgItem(IDC_SELL_BUTTON2, this);
	sell_mkt.SubclassDlgItem(IDC_SELL_BUTTON3, this);
	sell_lmt.SubclassDlgItem(IDC_SELL_LMT_BUTTON, this);

	average_b.SubclassDlgItem(IDC_AQ_BUTTON, this);
	average_br.SubclassDlgItem(IDC_AQ_RESET, this);

	profit_b.SubclassDlgItem(IDC_PQ_BUTTON, this);
	profit_br.SubclassDlgItem(IDC_PQ_RESET, this);

	liquidate_b.SubclassDlgItem(IDC_LIQUIDATE_BUTTON, this);
	liquidate_ball.SubclassDlgItem(IDC_LIQUIDATEALL_BUTTON, this);

	suspend_b.SubclassDlgItem(IDC_SUSPEND_BUTTON, this);
	resume_b.SubclassDlgItem(IDC_RESUME_BUTTON, this);

	cancel_all_b.SubclassDlgItem(IDC_CANCEL_ALL_BUTTON, this);
	cancel_b.SubclassDlgItem(IDC_CANCEL_BUTTON, this);
	go_b.SubclassDlgItem(IDC_GO_BUTTON, this);
//	go_br.SubclassDlgItem(IDC_GO_RESET, this);

	book_b.SubclassDlgItem(IDC_BOOK_BUTTON, this);

	set_buttons_price();
	set_buttons_qty();
	set_limit_buttons();


	
	average_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
	average_br.SetColour(TC_COL::White, TC_COL::Red, TC_COL::LightGrey);

	profit_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
	profit_br.SetColour(TC_COL::White, TC_COL::Red, TC_COL::LightGrey);

	suspend_b.SetColour(TC_COL::Black, TC_COL::Orange, TC_COL::LightGrey);
	resume_b.SetColour(TC_COL::Black, TC_COL::LightGreen, TC_COL::LightGrey);

	go_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);

	std::ostringstream os;

	qty_1.SetWindowText(CA2T(TCSettings::get("LFStrategy:qty_1", "10").c_str()));
	qty_2.SetWindowText(CA2T(TCSettings::get("LFStrategy:qty_2", "20").c_str()));
	qty_3.SetWindowText(CA2T(TCSettings::get("LFStrategy:qty_3", "30").c_str()));
	qty_4.SetWindowText(CA2T(TCSettings::get("LFStrategy:qty_4", "50").c_str()));

	_booking = false;
	_suspended = true;

	init_buttons();
	set_action_buttons_color();
}
void Cmd_monitorView::set_action_buttons_color()
{
	if(!_booking)
	{
		liquidate_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
		liquidate_ball.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);

		cancel_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
		cancel_all_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
		book_b.SetWindowText(_T("\nBOOKING OFF\n"));
		book_b.SetColour(ZRGB(TCSettings::get("Button:booking_off:text", "Black")), 
						ZRGB(TCSettings::get("Button:booking_off:bkgrnd", "LightBlue")), 
						ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Limit on ASK
		buy_lmt_ask.SetColour(ZRGB(TCSettings::get("Button:buy_ask:text", "DarkGreen")), 
							  ZRGB(TCSettings::get("Button:buy_ask:bkgrnd", "PaleTurquoise")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Buy Limit on BID
		buy_lmt_bid.SetColour(ZRGB(TCSettings::get("Button:buy_bid:text", "DarkGreen")), 
							  ZRGB(TCSettings::get("Button:buy_bid:bkgrnd", "PaleTurquoise")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Limit
		buy_lmt.SetColour(ZRGB(TCSettings::get("Button:buy_lmt:text", "DarkGreen")), 
						  ZRGB(TCSettings::get("Button:buy_lmt:bkgrnd", "PaleTurquoise")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Market
		buy_mkt.SetColour(ZRGB(TCSettings::get("Button:buy_mkt:text", "DarkGreen")), 
						  ZRGB(TCSettings::get("Button:buy_mkt:bkgrnd", "LightGreen")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Sell Limit on ASK
		sell_lmt_ask.SetColour(ZRGB(TCSettings::get("Button:sell_ask:text", "DarkRed")), 
							  ZRGB(TCSettings::get("Button:sell_ask:bkgrnd", "PaleTurquoise")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Sell Limit on BID
		sell_lmt_bid.SetColour(ZRGB(TCSettings::get("Button:sell_bid:text", "DarkRed")), 
							  ZRGB(TCSettings::get("Button:sell_bid:bkgrnd", "PaleTurquoise")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Sell Limit
		sell_lmt.SetColour(ZRGB(TCSettings::get("Button:sell_lmt:text", "DarkRed")), 
						  ZRGB(TCSettings::get("Button:sell_lmt:bkgrnd", "PaleTurquoise")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Sell Market
		sell_mkt.SetColour(ZRGB(TCSettings::get("Button:sell_mkt:text", "DarkRed")), 
						  ZRGB(TCSettings::get("Button:sell_mkt:bkgrnd", "LightGreen")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
	}
	else // ========== Booking ============
	{
		liquidate_b.SetColour(TC_COL::Black, TC_COL::LightCoral, TC_COL::LightGrey);
		liquidate_ball.SetColour(TC_COL::Black, TC_COL::LightCoral, TC_COL::LightGrey);

		cancel_b.SetColour(TC_COL::Black, TC_COL::LightCoral, TC_COL::LightGrey);
		cancel_all_b.SetColour(TC_COL::Black, TC_COL::LightCoral, TC_COL::LightGrey);

		book_b.SetWindowText(_T("\nBOOKING ON\n"));
		book_b.SetColour(ZRGB(TCSettings::get("Button:booking_off:text", "Black")), 
						ZRGB(TCSettings::get("Button:booking_off:bkgrnd", "Red")), 
						ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Limit on ASK
		buy_lmt_ask.SetColour(ZRGB(TCSettings::get("Button:buy_ask:text", "DarkGreen")), 
							  ZRGB(TCSettings::get("Button:buy_ask:bkgrnd", "LightSalmon")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Buy Limit on BID
		buy_lmt_bid.SetColour(ZRGB(TCSettings::get("Button:buy_bid:text", "DarkGreen")), 
							  ZRGB(TCSettings::get("Button:buy_bid:bkgrnd", "LightSalmon")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Limit
		buy_lmt.SetColour(ZRGB(TCSettings::get("Button:buy_lmt:text", "DarkGreen")), 
						  ZRGB(TCSettings::get("Button:buy_lmt:bkgrnd", "LightSalmon")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Buy Market
		buy_mkt.SetColour(ZRGB(TCSettings::get("Button:buy_mkt:text", "DarkGreen")), 
						  ZRGB(TCSettings::get("Button:buy_mkt:bkgrnd", "LightCoral")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Sell Limit on ASK
		sell_lmt_ask.SetColour(ZRGB(TCSettings::get("Button:sell_ask:text", "DarkRed")), 
							  ZRGB(TCSettings::get("Button:sell_ask:bkgrnd", "LightSalmon")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Sell Limit on BID
		sell_lmt_bid.SetColour(ZRGB(TCSettings::get("Button:sell_bid:text", "DarkRed")), 
							  ZRGB(TCSettings::get("Button:sell_bid:bkgrnd", "LightSalmon")), 
							  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));

		//--- Sell Limit
		sell_lmt.SetColour(ZRGB(TCSettings::get("Button:sell_lmt:text", "DarkRed")), 
						  ZRGB(TCSettings::get("Button:sell_lmt:bkgrnd", "LightSalmon")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
		
		//--- Sell Market
		sell_mkt.SetColour(ZRGB(TCSettings::get("Button:sell_mkt:text", "DarkRed")), 
						  ZRGB(TCSettings::get("Button:sell_mkt:bkgrnd", "LightCoral")), 
						  ZRGB(TCSettings::get("Button:disabled", "LightGrey")));
	}
}
		
void Cmd_monitorView::Ctrl::recalc_size()
{
	if (::IsWindow(GetSafeHwnd()))
	{
		EnableScrollBar(SB_BOTH, FALSE);
		ShowScrollBar(SB_BOTH, FALSE);

 		SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER); //40
		SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER); //40
		SetColumnWidth(2, LVSCW_AUTOSIZE);
		SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);

		_rows = GetCountPerPage();
	}
}
void Cmd_monitorView::process_data(LFDepthDataPtr* p_)
{
	auto_ptr<LFDepthDataPtr> p(p_);
	if (TCComparePrice::diff((*p)->_bid[0]._p, (*p)->_ask[0]._p) > 0)
	{
		Z::report_error("bid [%f] > ask [%f]", (*p)->_bid[0]._p, (*p)->_ask[0]._p);
		return;
	}

	if (_last_pos.get())
	{
		_last_pos->mark_to_market(**p_);
		_last_pos->strat_to_market(**p_);
	}
	ctrl2()->process_data(p.release());
}
void Cmd_monitorView::process_order(LFOrderUpdatePtr* p_)
{
	auto_ptr<LFOrderUpdatePtr> p(p_);
	cancel_all_b.EnableWindow(ctrl4()->process_order(p_));
}
Cmd_monitorView::Ctrl::Ctrl()
:_high_ask(0), _low_bid(0), _tick_size(0), _pf(0), 
_rows(0), _lowt(0), _hight(0), _center(true), _tvec_offset(500)
{
	add_column("TVol",		f_loc(&Record::_trades));	
	add_column("LVol",		f_loc(&Record::_l_trade));
	add_column("Price",		f_loc(&Record::_price));	
	add_column_c("Qty",		f_loc(&Record::_qty_type), compute_qty(), b_a_col());
}
void Cmd_monitorView::Ctrl::process_data(LFDepthDataPtr* p_)
{
	auto_ptr<LFDepthDataPtr> p(p_);
	_last_depth = *p_;
	if(_pos.get())
		_pos->set_market_data(**p_);
	if(_pf != 0) return;
	_tick_size = _last_depth->_tick_size;
	for(_pf = 1; TCCompare_10000::z((_tick_size * _pf) - (int)(_tick_size * _pf)) != 0; _pf *= 10);
}
void Cmd_monitorView::Ctrl::process_trade(LFTradeDataPtr* p_)
{
	auto_ptr<LFTradeDataPtr> p(p_);

	if (_low_bid == 0 || _high_ask == 0)
		return;
	
	_last_trade = (*p_)->_trade;
	double ticks;
	LFSide::mod_tick(_last_trade._p, _tick_size, &ticks);	

	if(_tvec.size() == 0) //allocate
	{
		_lowt = _low_bid - _tvec_offset;//	if(_lowt < 0) _lowt = 0;
		_hight = _high_ask + _tvec_offset; // + 500

		_tvec.reserve(size_t(_hight - _lowt));
		for(size_t i = 0; i < _hight - _lowt; ++i)
			_tvec.push_back(0);
	}
	_tvec[size_t(ticks - _lowt)] = (*p_)->_p_vol;
}


Cmd_monitorView::Cmd_monitorView()
: Cp_monView(new Ctrl, new Ctrl, new PosCtrl, new OrderCtrl, IDD_MD_REPORT)
{
	open_strategy();
	theApp.mainframe()->set_monitor(this);

	qty1 = TCSettings::get_integer("LFStrategy:qty_1", 10); 
	qty2 = TCSettings::get_integer("LFStrategy:qty_2", 20); 
	qty3 = TCSettings::get_integer("LFStrategy:qty_3", 30); 
	qty4 = TCSettings::get_integer("LFStrategy:qty_4", 50); 
	qty_edit = qty1;
}

void Cmd_monitorView::DoDataExchange(CDataExchange* pDX)
{
	unsigned int oldq = qty_edit;
	CString value; 

	Cp_monView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QTY_EDIT, qty_e);
	DDX_Text(pDX, IDC_QTY_EDIT, qty_edit);
	qty_e.GetWindowText(value);     
	if(qty_edit < 1 || qty_edit > 1000 || value[0] == '0')
	{
		qty_edit = oldq;
		set_qty_text();
	}
	DDX_Control(pDX, IDC_QTY_1, qty_1);
	DDX_Control(pDX, IDC_QTY_2, qty_2);
	DDX_Control(pDX, IDC_QTY_3, qty_3);
	DDX_Control(pDX, IDC_QTY_4, qty_4);
}
//======================================================================
void Cmd_monitorView::process_rtpos(LFPositionDataPtr* p_)
{
	auto_ptr<LFPositionDataPtr> p(p_);
	_last_pos = *p;

	// enable buttons..
	enable_position_buttons(_last_pos->_p_remain_q > 0);
	
	if(_last_pos->_status != LF::psActive)
	{
		suspend_b.EnableWindow(FALSE);
		resume_b.EnableWindow(TRUE);
	}
	else
	{
		suspend_b.EnableWindow(TRUE);
		resume_b.EnableWindow(FALSE);
	}
	LeafSound::instance()->play(_last_pos->_action);
	set_buttons_qty();
	set_go_button();
}
void Cmd_monitorView::open_strategy()
{
	long hwm = 0;
	long lwm = 0;
		
	if(!BMModel::instance()->is_live())
	{
		hwm = Z::get_setting_integer("LFGUI:hwm", 1000);
		lwm = Z::get_setting_integer("LFGUI:lwm", 999);
		_s.reset(new BMStrategy(theApp.star(), Z::make_callback(this, &Cmd_monitorView::process_bt_run)));
	}
	else
	{
		hwm = Z::get_setting_integer("LFGUI:hwm", 1000);
		lwm = Z::get_setting_integer("LFGUI:lwm", 999);
		_s.reset(new BMStrategy(theApp.star(), Z::make_callback(this, &Cmd_monitorView::process_rt_run)));
	}

	_s->set_name("DOM_BMStrategy");

	if(Z::get_setting_bool("LFGUI:Panel:DOM:on", true))
	{
		_s->subscribe_input(ctrl1(), &Cmd_monitorView::Ctrl::process_data, theApp.leader(), "l_depth", false, hwm, lwm);
		_s->subscribe_input(this,    &Cmd_monitorView::process_data, theApp.follower(), "f_depth", false, hwm, lwm);
		_s->subscribe_input(ctrl1(), &Cmd_monitorView::Ctrl::process_trade, theApp.leader(), "l_trade", false, hwm, lwm);
		_s->subscribe_input(ctrl2(), &Cmd_monitorView::Ctrl::process_trade, theApp.follower(), "f_trade", false, hwm, lwm);
	}	
	_s->subscribe_input(this, &Cmd_monitorView::process_order, LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.follower()), "strat", true);
	_s->subscribe_input(this, &Cmd_monitorView::process_rtpos, LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.follower()), "strat", true);
	//	_s->subscribe_inputs_inactive(Z::make_callback(&(theApp.mainframe()->get_player()), &PlayerDlgBar::process_inactive));
	_s->open();
}

Cmd_monitorView::~Cmd_monitorView()
{
	Z::report_debug("---exiting Cmd_monitorView");
	close_strategy();
	Z::report_debug("---exited Cmd_monitorView");
}
void Cmd_monitorView::close_strategy()
{
	_s->close();
	_s->unsubscribe_input<LFOrderUpdate>("strat");
	_s->unsubscribe_input<LFPositionData>("strat");
	if (Z::get_setting_bool("LFGUI:Panel:DOM:on"))
	{
		_s->unsubscribe_input<LFDepthData>("l_depth");
		_s->unsubscribe_input<LFDepthData>("f_depth");
		_s->unsubscribe_input<LFTradeData>("l_trade");
		_s->unsubscribe_input<LFTradeData>("f_trade");
	}
//	_s->unsubscribe_inputs_inactive();

}
//======================================================================

bool Cmd_monitorView::Ctrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{
	if(r_ == 0)
		return false;

	switch(r_->_qty_type.second)
	{
	case BEST_ASK:
	case ASK:
		clrTextBk_ = TC_COL::LightGrey;
		break;
	case BEST_BID:
	case BID:
		clrTextBk_ = TC_COL::LightBlue;
		break;
	case SPREAD:
		clrTextBk_ = 0xFF000000;
		break;
	}

	return true;
}

//======================================================================
void Cmd_monitorView::Ctrl::display_data()
{
	BASE::R_Vec r_vec;
	SetColumnWidth(2, LVSCW_AUTOSIZE);
	set_data(r_vec);
	set_volume(r_vec);
	mark_for_delete();
	update_data(r_vec, (INT_PTR)2, TC_ListCtrlBase::SORT_DSC);
}
void Cmd_monitorView::Ctrl::set_data(BASE::R_Vec& r_vec)
{
	if(_last_depth.get() == 0)
		return;

	LFSideBuy	lf_bid;
	LFSideSell	lf_ask;

	lf_bid.fill_same(_pqpv_bid, *(_last_depth).get());
	lf_ask.fill_same(_pqpv_ask, *(_last_depth).get());
	
	size_t ask_steps = _pqpv_ask.size();
	size_t bid_steps = _pqpv_bid.size();

	if(ask_steps == 0 || bid_steps == 0)
		return;
	
	double best_bid		= 0;
	double best_ask		= 0;
	double worst_bid	= 0;
	double worst_ask	= 0;

	LFSide::mod_tick(_pqpv_bid[0]._p, _tick_size, &best_bid);
	LFSide::mod_tick(_pqpv_ask[0]._p, _tick_size, &best_ask);
	LFSide::mod_tick(_pqpv_bid[bid_steps-1]._p, _tick_size, &worst_bid);
	LFSide::mod_tick(_pqpv_ask[ask_steps-1]._p, _tick_size, &worst_ask);
	
	size_t spread		= (size_t)(best_ask==best_bid?0:best_ask - best_bid -1);
	size_t scale		= (size_t)(ask_steps + bid_steps + spread);

	if(_center)
	{
		recalc_size();
		if(_rows == 0)
			return;
		_center = false;

		if(_rows < scale)
		{	
			if(_rows < spread)
			{
				_high_ask = best_ask;
				bid_steps = 0;
				ask_steps = 1;
				spread = _rows - 1;
			}
			else
			{
				ask_steps = size_t(_rows - spread)/2;
				bid_steps = size_t(_rows - spread - ask_steps);
				_high_ask = best_ask + ask_steps;
			}
		}
		else
			_high_ask = best_ask + (_rows - spread)/2;
	}
	else // not centered, but the size also hasn't changed, however, spread might have
	{
		// do we have to move the scale?
		if(best_ask > _high_ask)
			_high_ask = best_ask;
		else if(best_bid < _low_bid && _rows > (spread+2))
			_high_ask = best_bid + _rows;
	}

	_low_bid = _high_ask - _rows;
	r_vec.reserve(_rows);
	//--------------------------------------------------------------------
	// Expand low
	//--------------------------------------------------------------------

	for(size_t i = 0; i < worst_bid - _low_bid; i++)
		r_vec.push_back(BASE::R_Ptr(new DOM_Record((_low_bid+i)*_tick_size, BID, _pf)));	
	//--------------------------------------------------------------------
	// Fill Bids
	//--------------------------------------------------------------------
	for(size_t i = (bid_steps==0?0:bid_steps-1); i > 0; i--)
		r_vec.push_back(new DOM_Record(_pqpv_bid[i], BID, _pf));
	r_vec.push_back(BASE::R_Ptr(new DOM_Record(_pqpv_bid[0], BEST_BID, _pf)));
	//--------------------------------------------------------------------
	// Fill Space
	//--------------------------------------------------------------------
	for(size_t i = 0; i < spread; i++)
		r_vec.push_back(BASE::R_Ptr(new DOM_Record(_pqpv_bid[0]._p + (i+1)*_tick_size, SPREAD, _pf)));	
	//--------------------------------------------------------------------
	// Fill Asks
	//--------------------------------------------------------------------
	r_vec.push_back(BASE::R_Ptr(new DOM_Record(_pqpv_ask[0], BEST_ASK, _pf)));
	for(size_t i = 1; i < ask_steps; i++)
		r_vec.push_back(BASE::R_Ptr(new DOM_Record(_pqpv_ask[i], ASK, _pf)));
	//--------------------------------------------------------------------	
	// Expand high
	//--------------------------------------------------------------------
	for (size_t i = 0; i < _high_ask - worst_ask; i++)
		r_vec.push_back(BASE::R_Ptr(new DOM_Record((worst_ask+i+1)*_tick_size, ASK, _pf)));
}
//======================================================================

void Cmd_monitorView::Ctrl::set_volume(BASE::R_Vec& r_vec_)
{
	if(!_last_trade.good())
		return;
	size_t indx = size_t(_low_bid - _lowt);
	if(indx > _tvec.size()-1)
	{
		//Z::report_error("Out of bounds in set_volume: _low_bid(%f), _lowt(%f), _tvec_size(%d)", 
		//	_low_bid, _lowt, _tvec.size());
		return; // should not happen, check & print
	}
	for(size_t i = 0; i < r_vec_.size() && (i + indx) < _tvec.size(); ++i)
		r_vec_[i]->_trades = _tvec[indx+i];
	
	double ticks;
	LFSide::mod_tick(_last_trade._p, _tick_size, &ticks);
	size_t i = size_t(ticks - _low_bid);
	if(ticks >= _low_bid && ticks <= _high_ask && i < r_vec_.size())
		r_vec_[i]->_l_trade = _last_trade._q; 
	//else
	//	Z::report_error("Out of bounds in set_volume: ticks(%f), _high_ask(%f), _low_bid(%f), indx(%d), i(%d)", 
	//		ticks, _high_ask, _low_bid, indx, i);
}
double Cmd_monitorView::Ctrl::best_bid() const
{
	if(_pqpv_bid.size() == 0)
		return TCCompare::NaN;
	return _pqpv_bid[0]._p;
}
double Cmd_monitorView::Ctrl::best_ask() const
{
	if(_pqpv_ask.size() == 0)
		return TCCompare::NaN;
	return _pqpv_ask[0]._p;
}
bool Cmd_monitorView::Ctrl::edit(CPoint)
{
	center();
	display_data();
	return true;
}
//=====================================================================================================
bool Cmd_monitorView::process_rt_run()
{
	ctrl1()->display_data();
	ctrl2()->display_data();
	ctrl3()->process_run(_last_pos);
	set_buttons_price();

	return true;
}
bool Cmd_monitorView::process_bt_run()
{
	//ACE_Time_Value tv = _s->get_input_time();
	//
	//if(tv != ACE_Time_Value::zero)
	//	theApp.mainframe()->get_player().set_time_from(tv);
	return 	process_rt_run();
}
void Cmd_monitorView::init_buttons()
{
	average_b.EnableWindow(FALSE);	
	average_br.EnableWindow(FALSE);

	profit_b.EnableWindow(FALSE);	
	profit_br.EnableWindow(FALSE);
	
	liquidate_b.EnableWindow(FALSE);
	liquidate_ball.EnableWindow(FALSE);

	suspend_b.EnableWindow(FALSE);	
	resume_b.EnableWindow(FALSE);	

	cancel_b.EnableWindow(FALSE);
	cancel_all_b.EnableWindow(FALSE);

	book_b.EnableWindow(TRUE);
	go_b.EnableWindow(FALSE);
//	go_br.EnableWindow(FALSE);

	cancel_b.SetWindowText(_T("\nCANCEL\n"));
	cancel_all_b.SetWindowText(_T("\nCANCEL ALL\n"));
	book_b.SetWindowText(_T("\nBOOKING OFF\n"));

	go_b.SetWindowText(_T("\nSTRATEGY\n"));
//	go_br.SetWindowText(_T("\nX"));

	enable_buy_sell_buttons(!RunSettings::instance()->get_rerun_backtest());
}
void Cmd_monitorView::enable_buy_sell_buttons(bool enable_)
{
	buy_mkt.EnableWindow(enable_);
	sell_mkt.EnableWindow(enable_);	

	if (!BMModel::instance()->is_live() || RunSettings::instance()->get_live_backtest())
		enable_ = false;
	buy_lmt_ask.EnableWindow(enable_);
	buy_lmt_bid.EnableWindow(enable_);
	sell_lmt_ask.EnableWindow(enable_);
	sell_lmt_bid.EnableWindow(enable_);

}
void Cmd_monitorView::enable_position_buttons(bool enable_)
{
	liquidate_ball.EnableWindow(enable_);
	liquidate_b.EnableWindow(enable_);
}
void Cmd_monitorView::set_buttons_price()
{	
	std::ostringstream osq;
	std::string label;
	std::string prefix;
	if(!TCCompare::is_NaN(ctrl2()->best_ask()) && !TCCompare::is_NaN(ctrl2()->best_bid()))
	{
		std::ostringstream osa;
		std::ostringstream osb;

		osa << ctrl2()->best_ask();
		osb << ctrl2()->best_bid();
		osq << qty_edit;
			
		prefix = "\nBuy " + osq.str() + "\n";

		label = prefix + osa.str() + "\nLMT DAY";
		buy_lmt_ask.SetWindowText(CA2T(label.c_str()));
		
		label = prefix + osb.str() + "\nLMT DAY";
		buy_lmt_bid.SetWindowText(CA2T(label.c_str()));

		label = prefix + "@MKT";
		buy_mkt.SetWindowText(CA2T(label.c_str()));

		prefix = "\nSell " + osq.str() + "\n";
		label = prefix + osa.str() + "\nLMT DAY";
		sell_lmt_ask.SetWindowText(CA2T(label.c_str()));

		label = prefix + osb.str() + "\nLMT DAY";
		sell_lmt_bid.SetWindowText(CA2T(label.c_str()));

		label = prefix + "@MKT";
		sell_mkt.SetWindowText(CA2T(label.c_str()));
	}
	else
	{
		osq << qty_edit;
		qty_e.SetWindowText(CA2T(osq.str().c_str()));

		prefix = "\nBuy " + osq.str() + "\n";

		label = prefix + "LMT DAY";
		buy_lmt_ask.SetWindowText(CA2T(label.c_str()));
		buy_lmt_bid.SetWindowText(CA2T(label.c_str()));
		label = prefix + "@MKT";
		buy_mkt.SetWindowText(CA2T(label.c_str()));

		prefix = "\nSell " + osq.str() + "\n";
		label = prefix + "LMT DAY";
		sell_lmt_ask.SetWindowText(CA2T(label.c_str()));
		sell_lmt_bid.SetWindowText(CA2T(label.c_str()));
		label = prefix + "@MKT";
		sell_mkt.SetWindowText(CA2T(label.c_str()));
	}
}
void Cmd_monitorView::set_go_button()
{
	_sl.reset();
	const LFSide* act_side = 0;
	// Got the length and sharp, looking for bounce to enter 
	if(_last_pos->_bias_bc._w.cluster_valid() && 
		!_last_pos->_bias_bc.ended())
	{
		act_side = _last_pos->_bias_bc._side;
	}

	if(_last_pos->_bias_bx._w.cluster_valid()
		&& _last_pos->_bias_bx._w._strength > 0
		&& _last_pos->_bias_bx._side != act_side)
	{
		_sl = _last_pos->_bias_bx._w;
		if(LF::StrategyActionType(_sl._strength) == LF::satSHARP)
			LeafSound::instance()->play(LF::paSharp);
	}
	
	bool enable = _sl.valid();
	if (enable) // potential entry
	{
		bool contra_action = LF::StrategyActionType(_sl._strength) == LF::satSHARP;
		LF::Side green_b = contra_action ? LF::s_SELL : LF::s_BUY;
		if (_sl._side->value() == green_b)
			go_b.SetColour(TC_COL::White, TC_COL::Green, TC_COL::LightGrey);
		else
			go_b.SetColour(TC_COL::White, TC_COL::Red, TC_COL::LightGrey);

		go_b.SetWindowText(CA2T(_sl._comment.c_str()));
	}
	else
	{
		go_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
		go_b.SetWindowText(_T("\nSTRATEGY\n"));
	}
	go_b.EnableWindow(enable);
}

void Cmd_monitorView::set_buttons_qty()
{	
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		std::ostringstream aq;
		if(average_br.IsWindowEnabled())
		{
			if(_last_pos->_q_A_i == 0)
			{
				OnAverageResetGUI();		
				aq << "\nIN\n" << _last_pos->_q_A_s << " (" << qty_edit << ")\n";
			}
			else
				aq << "\nIN\n" << _last_pos->_q_A_i << " (" << _last_pos->_q_A_s << ")\n";
		}
		else
			aq << "\nIN\n" << _last_pos->_q_A_s << " (" << qty_edit << ")\n";
		average_b.SetWindowText(CA2T(aq.str().c_str()));
		
		std::ostringstream pq;
		if(profit_br.IsWindowEnabled())
		{
			if(_last_pos->_q_P_i == 0)
			{
				OnProfitResetGUI();
				pq << "\nOUT\n" << _last_pos->_q_P << " (" << qty_edit  << ")\n";
			}
			pq << "\nOUT\n" << _last_pos->_q_P_i << " (" << _last_pos->_p_open._q  << ")\n";
		}
		else
			pq << "\nOUT\n" << _last_pos->_q_P << " (" << qty_edit  << ")\n";
		profit_b.SetWindowText(CA2T(pq.str().c_str()));

		std::ostringstream lq;
		lq << "\nLIQUIDATE\n" << qty_edit;
		liquidate_b.SetWindowText(CA2T(lq.str().c_str()));

		std::ostringstream laq;
		laq << "\n LIQUIDATE ALL\n" << _last_pos->_p_open._q << "\n";
		liquidate_ball.SetWindowText(CA2T(laq.str().c_str()));
	}
	else
	{
		average_b.SetWindowText(CA2T("\nIN\n0 (0)\n"));
		average_br.SetWindowText(CA2T("\n x"));

		profit_b.SetWindowText(CA2T("\nOUT\n0 (0)\n"));
		profit_br.SetWindowText(CA2T("\n x"));

		liquidate_b.SetWindowText(CA2T("\nLIQUIDATE\n0\n"));
		liquidate_ball.SetWindowText(CA2T("\nLIQUIDATE ALL\n 0\n"));

		suspend_b.SetWindowText(CA2T("\nSuspend\n"));
		resume_b.SetWindowText(CA2T("\nResume\n"));
		return;
	}
}
void Cmd_monitorView::OnBuyLmtAsk()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_BUY;
		r->_limit_p = ctrl2()->best_ask();
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_BUY, qty_edit, LF::otLimit, 0, ctrl2()->best_ask());
	}
}
void Cmd_monitorView::OnBuyLmtBid()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_BUY;
		r->_limit_p = ctrl2()->best_bid();
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_BUY, qty_edit, LF::otLimit, 0, ctrl2()->best_bid());
	}
}
void Cmd_monitorView::OnBuyMkt()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otMarket;
		r->_order_side = LF::s_BUY;
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_BUY, qty_edit, LF::otMarket);
	}
}
void Cmd_monitorView::OnSellLmtAsk()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_SELL;
		r->_limit_p = ctrl2()->best_ask();
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_SELL, qty_edit, LF::otLimit, 0, ctrl1()->best_ask());
	}
}
void Cmd_monitorView::OnSellLmtBid()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otLimit;
		r->_order_side = LF::s_SELL;
		r->_limit_p = ctrl2()->best_bid();
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_SELL, qty_edit, LF::otLimit, 0, ctrl1()->best_bid());
	}
}
void Cmd_monitorView::OnSellMkt()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_order_type = LF::otMarket;
		r->_order_side = LF::s_SELL;
		r->_q = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
	else
	{
		LFPosition* pos = ctrl2()->get_pos();
		if(pos != 0)
			pos->new_order(LF::s_SELL, qty_edit, LF::otMarket);
	}
}
void Cmd_monitorView::OnQtyChanged()
{
	CString value;     
	qty_e.GetWindowText(value);     
	if( value.IsEmpty())
		return;

	UpdateData(TRUE);  
	set_buttons_qty();
	set_limit_buttons();
}
void Cmd_monitorView::OnQty1()
{
	UpdateData();
	qty_edit = qty1;
	set_qty_text();
}
void Cmd_monitorView::OnQty2()
{
	UpdateData();
	qty_edit = qty2;
	set_qty_text();
}
void Cmd_monitorView::OnQty3()
{
	UpdateData();
	qty_edit = qty3;
	set_qty_text();
}
void Cmd_monitorView::OnQty4()
{
	UpdateData();
	qty_edit = qty4;
	set_qty_text();
}
void Cmd_monitorView::OnAverage()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_q_A_i = qty_edit;
		if(qty_edit > 0)
		{
			average_b.SetColour(TC_COL::Black, TC_COL::LightGreen, TC_COL::LightGrey);
			average_br.EnableWindow(TRUE);
		}
		BMModel::instance()->send(r.release());
	}
}
void Cmd_monitorView::OnProfit()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		auto_ptr<LFStrategyRequest> s(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		s->_t = r->_t;
		r->_q_P_i = qty_edit;
		s->_q_S_i = qty_edit;
		if(qty_edit > 0)
		{
			profit_b.SetColour(TC_COL::Black, TC_COL::LightGreen, TC_COL::LightGrey);
			profit_br.EnableWindow(TRUE);
		}
		BMModel::instance()->send(r.release());
		BMModel::instance()->send(s.release());
	}
}

void Cmd_monitorView::OnLiquidate()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_q_L_i = qty_edit;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
}
void Cmd_monitorView::OnLiquidateAll()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_q_L_i = r->_q;
		r->_booking = _booking;

		BMModel::instance()->send(r.release());
	}
}
///-----------
void Cmd_monitorView::OnAverageResetGUI()
{
	average_br.EnableWindow(FALSE);
	average_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
}
void Cmd_monitorView::OnAverageReset()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_q_A_i = 0;
		OnAverageResetGUI();
		BMModel::instance()->send(r.release());
	}
}
void Cmd_monitorView::OnProfitResetGUI()
{
	profit_br.EnableWindow(FALSE);
	profit_b.SetColour(TC_COL::Black, TC_COL::LightBlue, TC_COL::LightGrey);
}
void Cmd_monitorView::OnProfitReset()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		auto_ptr<LFStrategyRequest> s(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		s->_t = r->_t;

		r->_q_P_i = 0;
		s->_q_S_i = 0;
		
		OnProfitResetGUI();
		
		BMModel::instance()->send(r.release());
		BMModel::instance()->send(s.release());
	}
}

//----------------



void Cmd_monitorView::OnSuspend()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_position_status = LF::psSuspended;

		BMModel::instance()->send(r.release());
	}
}
void Cmd_monitorView::OnResume()
{
	if(_last_pos.get() != 0 && _last_pos->_strategy_id != 0)
	{
		auto_ptr<LFStrategyRequest> r(new LFStrategyRequest(_last_pos->_strategy_id));
		r->_t = _s->get_input_time();
		r->_position_status = LF::psActive;

		BMModel::instance()->send(r.release());
	}
}
void Cmd_monitorView::set_qty_text()
{
	std::ostringstream os;
	os << qty_edit;
	qty_e.SetWindowText(CA2T(os.str().c_str()));
}
bool Cmd_monitorView::change_run()
{
	ctrl1()->reset();
	ctrl2()->reset();
	ctrl1()->set_pos(new LFPosition(theApp.leader(), 0));
	ctrl2()->set_pos(new LFPosition(theApp.follower(), 0));
	ctrl3()->change_run();
	ctrl4()->change_run();
	return true;
}
void Cmd_monitorView::Ctrl::reset()
{
	_last_depth.reset();
	_last_trade.reset();
	_pqpv_bid.clear();
	_pqpv_ask.clear();
	_high_ask = 0;
	_low_bid = 0;
	_tick_size = 0;
	_pf = 0;
	_rows = 0;
	_tvec.clear();
	_lowt = 0;
	_hight = 0;
	_center = true;

	mark_for_delete();
	do_delete();

	UpdateData(FALSE);
}
//------------- PosCtrl --------------------------------------

Cmd_monitorView::PosCtrl::PosCtrl()
{
	add_column("Status",		f_loc(&Record::_status), P_FORMAT::position_status());
	add_column("Trend End",	f_loc(&Record::_signal));
	add_column_c("Strat Act",	f_loc(&Record::_action), P_FORMAT::position_action(), P_COLOR::position_action());
	add_column_c("To Trend", f_loc(&Record::_to_trend), P_COLOR::min_red());
	
	add_column_c("UnR Ticks",f_loc(&Record::_unrealized_ticks), P_COLOR::neg_red());

	add_column("Side",			f_loc(&Record::_side), P_FORMAT::side());
	add_column("Open Qt@Price",	f_loc(&Record::_open), P_FORMAT::pqp());
	add_column_c("Rlz PnL",		f_loc(&Record::_realized_pnl), P_COLOR::neg_red());
	add_column_c("UnR PnL",		f_loc(&Record::_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Tot PnL",		f_loc(&Record::_pnl), P_COLOR::neg_red());
}


//======================================================================
bool Cmd_monitorView::PosCtrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{

	clrText_ = 0xFF000000;
	clrTextBk_ = 0xFF000000;

	switch(r_->_status){
		case LF::psActive: 
			clrTextBk_ = TC_COL::LightGreen;
			break;
		case LF::psSuspended: 
			clrTextBk_ = TC_COL::Yellow;
			break;
		case LF::psUndefined: 
			clrTextBk_ = TC_COL::LightGrey;
			break;
		case LF::psCutLoss: 
			clrTextBk_ = TC_COL::Yellow;
			clrText_ = TC_COL::Red;
			break;
		default:
			break;
	};
	return true;
}

//======================================================================
bool Cmd_monitorView::PosCtrl::process_run(LFPositionDataPtr p_)
{
	if(p_.get() != 0)
	{
		EnableScrollBar(SB_BOTH, FALSE);
		ShowScrollBar(SB_BOTH, FALSE);
		SetColumnWidth((int)(_col_vec.size() - 1), LVSCW_AUTOSIZE_USEHEADER);

		BASE::R_Vec r_vec;
		r_vec.push_back(p_);
		update_data(r_vec);
	}
	return true;
}
//======================================================================
bool Cmd_monitorView::PosCtrl::change_run()
{
	mark_for_delete();
	do_delete();
	return true;
}

//======================================================================
// OrderCtrl
//

Cmd_monitorView::OrderCtrl::OrderCtrl()
{
	add_column("Auto",			f_loc(&Record::_sub_strategy_id), P_FORMAT::sub_strategy_id());
	add_column("Type",			f_loc(&Record::_order_type), P_FORMAT::order_type());
	add_column("Side",			f_loc(&Record::_order_side), P_FORMAT::side());
	add_column("Qty",			f_loc(&Record::_q));
	add_column("LimitPrice",	f_loc(&Record::_limit_p));
	add_column("FillQty",		f_loc(&Record::_filled_q));	
	add_column("AvgFillPrice",	f_loc(&Record::_avg_fill_p));
	add_column("RemainQty",		f_loc(&Record::_remain_q));
	add_column("Status",		f_loc(&Record::_order_status), P_FORMAT::order_status());
	add_column("LEAF Id",		f_loc(&Record::_order_id));
	add_column("CQG Id",		f_loc(&Record::_cqg_OriginalOrderId));
	add_column("Time",			f_loc(&Record::_t), P_FORMAT::time());
	add_column("User",			f_loc(&Record::_user_name));
	add_column("ErrorId",		f_loc(&Record::_err_code));
	add_column("ErrorDesc",		f_loc(&Record::_err_desc));
}
//======================================================================
bool Cmd_monitorView::OrderCtrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{
	clrText_ = 0xFF000000;
	clrTextBk_ = 0xFF000000;
	switch(r_->_order_status){
		case LF::osNotSent:
			break; //default color

		case LF::osInClient:
		case LF::osInModify:
		case LF::osInCancel:
		case LF::osInTransit:
		case LF::osInOrderBook:
		case LF::osActiveAt:
			clrTextBk_ = TC_COL::LightGreen;
			break;	

		case LF::osRejectGW:
		case LF::osInTransitTimeout:
		case LF::osDisconnected:
		case LF::osExpired:		
		case LF::osRejectFCM:
			clrTextBk_ = TC_COL::LightGrey;
			break;
		case LF::osCanceled:	
		case LF::osFilled:		
			clrTextBk_ = TC_COL::LightBlue;
			break;
		case LF::osParked:		
		case LF::osContingent:	
			clrTextBk_ = TC_COL::Yellow;
			break;
		default:
			break;
	};

	return true;
}

//======================================================================
bool Cmd_monitorView::OrderCtrl::process_order(LFOrderUpdatePtr* p_)
{
	r_vec.push_back(BASE::R_Ptr(new LFOrderUpdate(*(*p_).dptr())));
	SetColumnWidth((int)(_col_vec.size() - 1), LVSCW_AUTOSIZE_USEHEADER);

	update_data(r_vec);
	r_vec.clear();
	return true;
}

//======================================================================
bool Cmd_monitorView::OrderCtrl::change_run()
{
	r_vec.clear();
	mark_for_delete();
	do_delete();
	return true;
}
//======================================================================
