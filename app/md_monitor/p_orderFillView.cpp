#include "stdafx.h"
#include "p_format.h"
#include "p_orderFillView.h"
#include "md_monitor.h"

//======================================================================
Cp_OrderFillView::Ctrl::Ctrl()
{
//	add_column("Strategy",		f_loc(&Record::_strategy_key));
	add_column("Auto",			f_loc(&Record::_sub_strategy_id), P_FORMAT::sub_strategy_id());

	add_column("LEAF Id",	f_loc(&Record::_order_id));
	add_column("CQG Id",	f_loc(&Record::_cqg_OriginalOrderId));
	//add_column("GWY_OrderId",	f_loc(&Record::_cqg_GWOrderID));

	//add_column("Instrument",	f_loc(&Record::_instr));
	//add_column("AccountId",		f_loc(&Record::_cqg_AccountID));
	add_column("Time",			f_loc(&Record::_t), P_FORMAT::time());

	add_column("Qty",			f_loc(&Record::_fill, &TC_pqp::_q));
	add_column("Price",			f_loc(&Record::_fill, &TC_pqp::_p), P_FORMAT::price());
	add_column("Fill Id",		f_loc(&Record::_fill_id));

	add_column("Side",			f_loc(&Record::_side), P_FORMAT::side());
	add_column("Status",		f_loc(&Record::_status), P_FORMAT::fill_status());
}
//======================================================================
bool Cp_OrderFillView::Ctrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
{
	clrText_ = 0xFF000000;
	clrTextBk_ = 0xFF000000;
	switch(r_->_status){
		case LF::fsNormal:
			clrTextBk_ = TC_COL::LightGreen;
			break;	
		case LF::fsCanceled:
			clrTextBk_ = TC_COL::Red;
			break;
		case LF::fsModified:			
			clrTextBk_ = TC_COL::LightBlue;
			break;	
		case LF::fsBusted:	
			clrTextBk_ = TC_COL::Yellow;
			break;
	};

	return true;
}


IMPLEMENT_DYNCREATE(Cp_OrderFillView, ETSLayoutFormView)

BEGIN_MESSAGE_MAP(Cp_OrderFillView, ETSLayoutFormView)
END_MESSAGE_MAP()


//======================================================================
Cp_OrderFillView::Cp_OrderFillView(UINT nID)
: ETSLayoutFormView(nID), _ctrl(new Ctrl)
{
	_s.reset(new BMStrategy(theApp.star(), Z::make_callback(ctrl(), &Cp_OrderFillView::Ctrl::process_run)));
	_s->set_name("Order_FillView");

	_s->subscribe_input(ctrl(), 
						&Cp_OrderFillView::Ctrl::process_order, 
						LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.leader()), "l_fillorder");
	_s->subscribe_input(ctrl(), 
						&Cp_OrderFillView::Ctrl::process_order, 
						LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.follower()), "f_fillorder");
	_s->open();

	theApp.mainframe()->set_fill(this);
}

//======================================================================
Cp_OrderFillView::~Cp_OrderFillView()
{
	Z::report_debug("---exiting Cp_OrderFillView");

	_s->close();
	_s->unsubscribe_input<LFOrderFill>("f_fillorder");
	_s->unsubscribe_input<LFOrderFill>("l_fillorder");
	Z::report_debug("---exited Cp_OrderFillView");
}
//======================================================================
void Cp_OrderFillView::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REC_LIST, *_ctrl);
}
//======================================================================
void Cp_OrderFillView::OnInitialUpdate()
{
	ETSLayoutFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);

	update_layout();
}
//======================================================================
void Cp_OrderFillView::update_layout()
{
	UpdateLayout(pane(VERTICAL)	<< item(IDC_REC_LIST));
}
//======================================================================
void Cp_OrderFillView::Ctrl::process_order(LFOrderFillPtr* p_)
{
	auto_ptr<LFOrderFillPtr> p(p_);
	r_vec.push_back(BASE::R_Ptr(new LFOrderFill(*(*p_).dptr())));
}
//======================================================================
bool Cp_OrderFillView::Ctrl::process_run()
{
	SetColumnWidth((int)(_col_vec.size()-1), LVSCW_AUTOSIZE_USEHEADER);

	update_data(r_vec);
	r_vec.clear();
	return true;
}
//======================================================================
bool Cp_OrderFillView::Ctrl::change_run()
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
