#include "stdafx.h"
#include "p_format.h"
#include "p_orderView.h"
#include "md_monitor.h"


//======================================================================
Cp_OrderView::Ctrl::Ctrl()
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

//	add_column("Strategy",		f_loc(&Record::_strategy_key));
//	add_column("Instrument",	f_loc(&Record::_instr));
//	add_column("GWY_OrderId",	f_loc(&Record::_cqg_GWOrderID));
//	add_column("AccountId",		f_loc(&Record::_cqg_AccountID));
//	add_column("StopPrice",		f_loc(&Record::_stop_p));
//	add_column("Trader",		f_loc(&Record::_trader_id));
}
//======================================================================
bool Cp_OrderView::Ctrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
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

	/*
	if(r_.i_state.i_traded_pct > 0)
		clrText_ = TC_COL::Blue;
*/
	return true;
}


IMPLEMENT_DYNCREATE(Cp_OrderView, ETSLayoutFormView)

BEGIN_MESSAGE_MAP(Cp_OrderView, ETSLayoutFormView)
END_MESSAGE_MAP()


//======================================================================
Cp_OrderView::Cp_OrderView(UINT nID)
: ETSLayoutFormView(nID), _ctrl(new Ctrl)
{
	_s.reset(new BMStrategy(theApp.star(), Z::make_callback(ctrl(), &Cp_OrderView::Ctrl::process_run)));
	_s->set_name("Order_View");
	_s->subscribe_input(ctrl(), 
						&Cp_OrderView::Ctrl::process_order, 
						LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"),theApp.leader()), "l_order");
	_s->subscribe_input(ctrl(), 
						&Cp_OrderView::Ctrl::process_order, 
						LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.follower()), "f_order");

	_s->open();

	theApp.mainframe()->set_order(this);
}

//======================================================================
Cp_OrderView::~Cp_OrderView()
{
	_s->unsubscribe_input<LFOrderUpdate>("f_order");
	_s->unsubscribe_input<LFOrderUpdate>("l_order");
}
//======================================================================
void Cp_OrderView::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REC_LIST, *_ctrl);
}
//======================================================================
void Cp_OrderView::OnInitialUpdate()
{
	ETSLayoutFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);

	update_layout();
}
//======================================================================
void Cp_OrderView::update_layout()
{
	UpdateLayout(pane(VERTICAL)	<< item(IDC_REC_LIST));
}
//======================================================================
void Cp_OrderView::Ctrl::process_order(LFOrderUpdatePtr* p_)
{
	auto_ptr<LFOrderUpdatePtr> p(p_);
	r_vec.push_back(BASE::R_Ptr(new LFOrderUpdate(*(*p_).dptr())));
}
//======================================================================
bool Cp_OrderView::Ctrl::process_run()
{
	SetColumnWidth((int)(_col_vec.size()-1), LVSCW_AUTOSIZE_USEHEADER);

	update_data(r_vec);
	r_vec.clear();
	return true;
}
//======================================================================
bool Cp_OrderView::Ctrl::change_run()
{

	r_vec.clear();
	mark_for_delete();
	do_delete();

	//update_data(r_vec, 2, TC_ListCtrlBase::SORT_DSC);	
	//DeleteAllItems();
	//UpdateData(FALSE);

	return true;
}
//======================================================================
