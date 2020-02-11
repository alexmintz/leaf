#include "stdafx.h"

#include "md_monitor.h"
#include "p_RTPosView.h"
#include "p_format.h"

//======================================================================
Cp_RTPosView::Ctrl::Ctrl()
{
	add_column("Strategy",		f_loc(&Record::_strategy_key));
//	add_column("SubStrategy",	f_loc(&Record::_sub_strategy_id));
//	add_column("Instrument",	f_loc(&Record::_instr));

	add_column("Status",		f_loc(&Record::_status), P_FORMAT::position_status());
	add_column("Strategy Signal",		f_loc(&Record::_signal));
	add_column_c("Strategy Action",		f_loc(&Record::_action), P_FORMAT::position_action(), P_COLOR::position_action());
	add_column("ToTrend",		f_loc(&Record::_to_trend));

	add_column("Average",		f_loc(&Record::_q_A));
	add_column("Average_i",		f_loc(&Record::_q_A_i));
	add_column("Average_s",		f_loc(&Record::_q_A_s));
	add_column("Profit",		f_loc(&Record::_q_P));
	add_column("Profit_i",		f_loc(&Record::_q_P_i));
	add_column("Scratch",		f_loc(&Record::_q_S));
	add_column("Scratch_i",		f_loc(&Record::_q_S_i));

	add_column_c("UnRealizedTicks",f_loc(&Record::_unrealized_ticks), P_COLOR::neg_red());

	add_column("Open Side",			f_loc(&Record::_side), P_FORMAT::side());
	add_column("Open Qty",		f_loc(&Record::_open, &TC_pqp::_q));
	add_column("Open Price",	f_loc(&Record::_open, &TC_pqp::_p), P_FORMAT::price());
	add_column_c("RealizedPnL",	f_loc(&Record::_realized_pnl), P_COLOR::neg_red());
	add_column_c("UnRealizedPnL",f_loc(&Record::_unrealized_pnl), P_COLOR::neg_red());
	add_column_c("Total PnL",			f_loc(&Record::_pnl), P_COLOR::neg_red());

	//add_column("Last Id",		f_loc(&Record::_last_fill_id));
	//add_column("Last Side",		f_loc(&Record::_last_fill_side), P_FORMAT::side());
	//add_column("Last Qty",		f_loc(&Record::_last_fill, &TC_pqp::_q));
	//add_column("Last Price",	f_loc(&Record::_last_fill, &TC_pqp::_p), P_FORMAT::price());

	_req.reset(new LFStrategyRequest);
	_req->_strategy_id = Z::get_setting_integer("LFStrategy:strategy_id");
	_req->_strategy_key = Z::get_setting("LFStrategy:strategy_id");
}

//======================================================================
bool Cp_RTPosView::Ctrl::set_color(Record* r_, INT_PTR row_, COLORREF& clrText_, COLORREF& clrTextBk_) const
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

IMPLEMENT_DYNCREATE(Cp_RTPosView, ETSLayoutFormView)

BEGIN_MESSAGE_MAP(Cp_RTPosView, ETSLayoutFormView)
END_MESSAGE_MAP()


//======================================================================
Cp_RTPosView::Cp_RTPosView()
: ETSLayoutFormView(IDD_RTPOS_REPORT), _ctrl(new Ctrl)
{
	_s.reset(new BMStrategy(theApp.star(), Z::make_callback(ctrl(), &Cp_RTPosView::Ctrl::process_run)));
	_s->set_name("RTPOS_Strategy");
	_s->subscribe_input(ctrl(), 
						&Cp_RTPosView::Ctrl::process_rtpos, 
						LF::make_strategy_key(Z::get_setting("LFStrategy:strategy_id"), theApp.follower()), "f_depth_1");
	_s->subscribe_input(ctrl(), &Cp_RTPosView::Ctrl::process_data, theApp.follower(), "f_depth_2");
	_s->open();
	theApp.mainframe()->set_pos(this);
}
//======================================================================
Cp_RTPosView::~Cp_RTPosView()
{
	_s->unsubscribe_input<LFPositionData>("f_depth_1");
	_s->unsubscribe_input<LFDepthData>("f_depth_2");
}
//======================================================================
void Cp_RTPosView::OnInitialUpdate()
{
	ETSLayoutFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	ResizeParentToFit(TRUE);

	update_layout();
}
//======================================================================
void Cp_RTPosView::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REC_LIST, *_ctrl);
}
//======================================================================

void Cp_RTPosView::update_layout()
{
	UpdateLayout(
		pane(VERTICAL)
		<< (pane(VERTICAL) << item(IDC_REC_LIST))
		);
}
//======================================================================
void Cp_RTPosView::Ctrl::process_rtpos(LFPositionDataPtr* p_)
{
	auto_ptr<LFPositionDataPtr> p(p_);

	LFPositionDataPtr pd(new LFPositionData(**p));
	_p_map[pd->_instr][pd->_strategy_key] = pd;
}
//======================================================================
void Cp_RTPosView::Ctrl::process_data(LFDepthDataPtr* p_)
{
	auto_ptr<LFDepthDataPtr> p(p_);
	std::string instr = (*p)->_instr_key;
	_d_map[instr] = *p;
}
//======================================================================
bool Cp_RTPosView::Ctrl::process_run()
{
	SetColumnWidth(11, LVSCW_AUTOSIZE_USEHEADER);

	BASE::R_Vec r_vec;
	for(ByInstrMap::iterator bi = _p_map.begin(); bi != _p_map.end(); ++bi)
	{
		ByStrategyMap& smap = (*bi).second;
		for(ByStrategyMap::iterator i = smap.begin(); i != smap.end(); ++i)
		{
			LFPositionDataPtr& pd = (*i).second;
			std::string instr = pd->_instr;
			DepthByInstrMap::iterator di = _d_map.find(instr);
			if(di != _d_map.end())
				pd->mark_to_market(*(*di).second);
			r_vec.push_back(pd);
		}
	}
	update_data(r_vec);
	return true;
}
//======================================================================
bool Cp_RTPosView::Ctrl::change_run()
{

//	r_vec.clear();
	_p_map.clear();
	_d_map.clear();
	mark_for_delete();
	do_delete();

	return true;
}

//======================================================================
