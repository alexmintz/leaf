// md_monitorView.cpp : implementation of the Cmd_graphView class
//

#include "stdafx.h"
#include "md_graphView.h"
#include "md_monitor.h"

#include "md_monitorDoc.h"
#include "p_format.h"
#include "znet/ZActiveQueue.h"
#include "znet/ZFunctor.h"
#include "znet/TCTAO.h"
//-------------------------------------------------
#include "Pegrpapi.h"

#define LEADER_STR "leader"
#define FOLLOWER_STR "follower"

//======================================================================
// LFGraph_TradeData
//======================================================================
void LFGraph_TradeData::reset()
{
	TC_Graph_Vol::reset();
}
void LFGraph_TradeData::process(LFTradeDataPtr* p_)
{
	auto_ptr<LFTradeDataPtr> p(p_);
	LFTradeData& d = **p;

	display_point(d.get_t(), d._trade._q);

}
//======================================================================
// LFGraph_TradeData_Leader
//======================================================================
void LFGraph_TradeData_Leader::init(TC_Graph* g_, int first_subset_, int axis_)
{
	LFGraph_TradeData::init(g_, first_subset_, axis_);
	PEnset(m_hPE, PEP_nWORKINGAXIS, _axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	DWORD col = ZARGB(Z::get_setting("LFModel:leader_open", "Blue"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, _subset, &col);

	int nSLT = PELT_THINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, _subset, &nSLT);

	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, _subset, CA2T("Leader Volume"));
	_g->map_subset("Volume", _subset);
	_g->add_to_group(LEADER_STR, this);
}
//======================================================================
// LFGraph_TradeData_Follower
//======================================================================
void LFGraph_TradeData_Follower::init(TC_Graph* g_, int first_subset_, int axis_)
{
	LFGraph_TradeData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_BAR);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	DWORD col = ZARGB(Z::get_setting("LFModel:follower_close", "Red"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);

	int nSLT = PELT_THINSOLID;
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);

	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("Follower Volume"));
	_g->map_subset("Volume", su_first);
//	_g->add_to_group(FOLLOWER_STR, this);
}

//======================================================================
// LFGraph_Panel
//======================================================================
int LFGraph_Panel::subsets()
{
	int ret = 0;
	for(size_t i = 0; i < _gd_v.size(); ++i)
		ret += _gd_v[i]->subsets();
	return ret;
}
void LFGraph_Panel::reset()
{
	for(size_t i = 0; i < _gd_v.size(); ++i)
		_gd_v[i]->reset();
}
void LFGraph_Panel::load_graph()
{
	for(size_t i = 0; i < _gd_v.size(); ++i)
		_gd_v[i]->load_graph();
}
void LFGraph_Panel::reset_graph()
{
	for(size_t i = 0; i < _gd_v.size(); ++i)
		_gd_v[i]->reset_graph();
}
void LFGraph_Panel::draw_graph()
{
	for(size_t i = 0; i < _gd_v.size(); ++i)
		_gd_v[i]->draw_graph();
}
//======================================================================
// LFGraph_Panel_HLOC
//======================================================================
LFGraph_Panel_HLOC::LFGraph_Panel_HLOC(const std::string& name_)
	:LFGraph_Panel(name_), _follower_sp(0)
{
	load_resource();
// Leader
	if(TCSettings::get_bool("LFGUI:Panel:HLOC:leader:on"))
	{
		_l_hloc = new TC_Graph_HLOC(LEADER_STR);
		_gd_v.push_back(_l_hloc);		// leader
	}
// Follower
	_follower_sp = LFSecurityMaster::instance()->get_settlement_price(Z::get_setting("LFStrategy:follower"));
	_f_hloc = new TC_Graph_HLOC(FOLLOWER_STR, LFSecurityMaster::instance()->get_fprecision(), _follower_sp);

	_gd_v.push_back(_f_hloc);		// follower


	std::vector<std::string> labels;

	if(TCSettings::get_bool("LFGUI:Panel:HLOC:leader:expected:on"))
	{
		for(size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			std::ostringstream os; os << "LE" << i;
			labels.push_back(os.str() + " HU");
			labels.push_back(os.str() + " SU");
			labels.push_back(os.str() + " HD");
			labels.push_back(os.str() + " SD");
			_l_escopes.push_back(new LFGraph_HLOC_Band(*this, labels, _l_hloc.get()));
			labels.clear();
		}
	}
	if(TCSettings::get_bool("LFGUI:Panel:HLOC:leader:trend:on"))
	{
		for(size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			std::ostringstream os; os <<"LT" << i;
			labels.push_back(os.str() + " HU");
			labels.push_back(os.str() + " SU");
			labels.push_back(os.str() + " HD");
			labels.push_back(os.str() + " SD");
			_l_tscopes.push_back(new LFGraph_HLOC_Band(*this, labels, _l_hloc.get()));
			labels.clear();
		}
	}
	if(TCSettings::get_bool("LFGUI:Panel:HLOC:follower:expected:on"))
	{
		for(size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			std::ostringstream os; os << "E" << i;
			labels.push_back(os.str() + " HU");
			labels.push_back(os.str() + " SU");
			labels.push_back(os.str() + " HD");
			labels.push_back(os.str() + " SD");
			_f_escopes.push_back(new LFGraph_HLOC_Band(*this, labels, _f_hloc.get()));
			labels.clear();
		}
	}
	if(TCSettings::get_bool("LFGUI:Panel:HLOC:follower:trend:on"))
	{
		for(size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			std::ostringstream os; os << "T" << i;
			labels.push_back(os.str() + " HU");
			labels.push_back(os.str() + " SU");
			labels.push_back(os.str() + " HD");
			labels.push_back(os.str() + " SD");
			_f_tscopes.push_back(new LFGraph_HLOC_Band(*this, labels, _f_hloc.get()));
			labels.clear();
		}
	}
}
void LFGraph_Panel_HLOC::load_resource()
{
	_show_charge_on_scope = TCSettings::get_integer("LFModel:LFGUI:show_charge_on_scope", 1001);
}
void LFGraph_Panel_HLOC::reset()
{
	LFGraph_Panel::reset();
	load_resource();
}

void LFGraph_Panel_HLOC::subsets_by_axes(std::vector<int>& v_)
{
	// Leader
	if(_l_hloc.get())
	{
		int l_s = _l_hloc->subsets();

		for(size_t i = 0; i < _l_tscopes.size(); ++i)
			l_s += (int)(_l_tscopes[i]->subsets());
		for(size_t i = 0; i < _l_escopes.size(); ++i)
			l_s += (int)(_l_escopes[i]->subsets());

		v_.push_back(l_s);
	}
	// Follower
	int f_s = _f_hloc->subsets();
	for(size_t i = 0; i < _f_tscopes.size(); ++i)
		f_s += (int)(_f_tscopes[i]->subsets());
	for(size_t i = 0; i < _f_escopes.size(); ++i)
		f_s += (int)(_f_escopes[i]->subsets());

	v_.push_back(f_s);
}
void LFGraph_Panel_HLOC::make_subset(std::vector<HLOC_BandPtr>& scope_,
									 const std::string& actor_,
									 bool as_trend_,
									 int& su_first_)
{
	size_t index = 0;
	for(size_t i = 0; i < scope_.size(); ++i)
	{
		TCResourceMap	color_map;
		std::ostringstream cos;
		cos << "LFGUI:Panel:HLOC:expected_color:" << actor_ << ":" << index << ":*";
		TCSettings::get_resource_match(cos.str(), color_map);

		TCResourceMap	line_map;
		std::ostringstream los;
		los << "LFGUI:Panel:HLOC:expected_line:" << actor_ << ":" << index << ":*";
		TCSettings::get_resource_match(los.str(), line_map);

		if(line_map.empty() || color_map.empty())
		{
			if(index == 0)
				z_throw_abort("UNKNOWN HLOC RESOURCES");
			index = 0;
			--i;
			continue;
		}

		TCResourceMap::iterator li = line_map.begin();
		TCResourceMap::iterator ci = color_map.begin();

		int k = 0;
		size_t s_max = scope_[i]->subsets();

		for(size_t l = 0; l < s_max; ++l )
		{
			DWORD col = ZARGB((*ci).second->get_value());
			int nSLT = (*li).second->get_integer();

			PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS,  su_first_, &col);
			PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES,su_first_, &nSLT);
			PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first_, CA2T(scope_[i]->_v[k++]->name().c_str()));
			if(as_trend_)
				_g->map_subset(ScopeTables::trend_id(actor_, i), su_first_, "HLOC");
			else
				_g->map_subset(ScopeTables::expectation_id(actor_, i), su_first_, "HLOC");
			++su_first_;
			++li;
			++ci;
			if(li == line_map.end()) li = line_map.begin();
			if(ci == color_map.end()) ci = color_map.begin();
		}
		++index;
	}
}


void LFGraph_Panel_HLOC::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;

	DWORD col = TC_RGB::Black;
	int nSLT = PELT_THINSOLID;

	if(_l_hloc.get())
	{
		_l_hloc->init(_g, subset_lines, axis);
		_g->add_to_group(LEADER_STR, _l_hloc.dptr());
		_g->map_axis("HLOC:leader", axis);
		subset_lines += _l_hloc->subsets();

		for(size_t i = 0; i < _l_escopes.size(); ++i)
			_l_escopes[i]->init(_g, subset_lines, axis, LEADER_STR);
		for(size_t i = 0; i < _l_tscopes.size(); ++i)
			_l_tscopes[i]->init(_g, subset_lines, axis, LEADER_STR);

		PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_SPECIFICPLOTMODE);
		PEnset(m_hPE, PEP_nSPECIFICPLOTMODE, PESPM_BOXPLOT);
		PEnset(m_hPE, PEP_nCOMPARISONSUBSETS, subset_lines - 4);
		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

		// High
		col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:leader:h", "Black"));
		nSLT = PELT_THINSOLID;
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("L High"));
		g_->map_subset("HLOC", su_first);

		su_first++;
		//Low
		col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:leader:l", "Black"));
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("L Low"));
		g_->map_subset("HLOC", su_first);
		su_first++;
		//Open
		//col = TC_RGB::Yellow;
		col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:leader:o", "Yellow"));
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("L Open"));
		g_->map_subset("HLOC", su_first);
		su_first++;
		//Close
		//col = TC_RGB::Blue;
		col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:leader:c", "Blue"));
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("L Close"));
		g_->map_subset("HLOC", su_first);
		su_first++;

		make_subset(_l_escopes, LEADER_STR, false, su_first);
		make_subset(_l_tscopes, LEADER_STR, true, su_first);
		axis++;
	}


///////// f_depth
	int s = subset_lines;
	_f_hloc->init(_g, subset_lines, axis);
	_g->add_to_group(FOLLOWER_STR, _f_hloc.dptr());
	_g->map_axis("HLOC:follower", axis);

	subset_lines += _f_hloc->subsets();

	for(size_t i = 0; i < _f_escopes.size(); ++i)
		_f_escopes[i]->init(_g, subset_lines, axis, FOLLOWER_STR);
	for(size_t i = 0; i < _f_tscopes.size(); ++i)
		_f_tscopes[i]->init(_g, subset_lines, axis, FOLLOWER_STR);

//---------------------------------------------------------------------
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_SPECIFICPLOTMODE);
	PEnset(m_hPE, PEP_nSPECIFICPLOTMODE, PESPM_BOXPLOT);
	PEnset(m_hPE, PEP_nCOMPARISONSUBSETS, subset_lines - s - 4);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	// High
	//col = TC_RGB::Black;
	col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:follower:h", "Black"));
	nSLT = PELT_THINSOLID;
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("F High"));
	g_->map_subset("HLOC", su_first);
	su_first++;
	//Low
	col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:follower:l", "Black"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("F Low"));
	g_->map_subset("HLOC", su_first);
	su_first++;
	//Open
	//col = TC_RGB::Red;
	col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:follower:o", "Red"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("F Open"));
	g_->map_subset("HLOC", su_first);
	su_first++;
	//Close
	//col = TC_RGB::Green;
	col = ZARGB(TCSettings::get("LFGUI:Panel:HLOC:color:follower:c", "Green"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("F Close"));
	g_->map_subset("HLOC", su_first);
	su_first++;

	make_subset(_f_escopes, FOLLOWER_STR, false, su_first);
	make_subset(_f_tscopes, FOLLOWER_STR, true, su_first);

	if (_follower_sp != 0)
	{
		TC_AnnotationData pa;
		pa._ann_type = PELT_THINSOLID;
		pa._pa_type = PEP_faHORZLINEANNOTATION;
		pa.set_group("HLOC");

		// Leader - Red Upper/Lower Horizontal Lines
		pa._ann_axis = axis_;
		pa._Y = _follower_sp;
		pa._ann_color = ZARGB(Z::get_setting("LFModel:follower_open", "Blue"));
		_g->add_annotation(pa); // Horizontal Settlement price annotation
	}
}
void LFGraph_Panel_HLOC::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();

	for(size_t i = 0; i < _f_tscopes.size(); ++i)
		_f_tscopes[i]->display_trend(t, d_._f_1._box_trends[i], d_._f_1._p);

	for (size_t i = 0; i < _f_escopes.size(); ++i)
	{
		if (i == _show_charge_on_scope)
			_f_escopes[i]->display_expected(t, d_._f_1._charge_trend, d_._f_1._p);
		else
			_f_escopes[i]->display_expected(t, d_._f_1._box_trends[i], d_._f_1._p);
	}

	for(size_t i = 0; i < _l_tscopes.size(); ++i)
		_l_tscopes[i]->display_trend(t, d_._l_1._box_trends[i], d_._l_1._p);

	for(size_t i = 0; i < _l_escopes.size(); ++i)
		_l_escopes[i]->display_expected(t, d_._l_1._box_trends[i], d_._l_1._p);

	_f_hloc->display_point(t, d_._f_1._p);
	if(_l_hloc.get() != 0)
		_l_hloc->display_point(t, d_._l_1._p);
}
//======================================================================
void LFGraph_HLOC_Last::display_table(int index_)
{
	if(_hloc == 0)
		return;
	char buf[128];
	float hloc_p = 0;
	PEvgetcellEx(m_hPE, PEP_faYDATA, _hloc->subset()+3, index_, &hloc_p);
	float f = 0;
	PEvgetcellEx(m_hPE, PEP_faYDATA, _subset, index_, &f);
	ACE_OS::memset(buf, 0, sizeof(buf));
	if(f == hloc_p)
	{
	
		ACE_OS::sprintf(buf, " %c%c%c:", _name[0], _name[1], _name[2]);
	}
	else
	{
		std::string format = " %c%c%c:" + LFSecurityMaster::instance()->get_fprecision();
		ACE_OS::sprintf(buf, format.c_str(), _name[0], _name[1], _name[2], f);
	}
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, _row, 0, CA2T(buf));
}

//======================================================================
// LFGraph_HLOC_Band
//======================================================================
LFGraph_HLOC_Band::LFGraph_HLOC_Band(LFGraph_Panel& p_, const std::vector<std::string>& labels_, TC_GraphData* hloc_)
{
	if(labels_.size() < subsets())
		Z::throw_error("not enough labels in LFGraph_HLOC_Band");
	for(size_t i = 0; i < subsets(); ++i)
	{
		_v.push_back(new LFGraph_HLOC_Last(labels_[i], hloc_));
		p_.bind(_v[i]);
	}
}

void LFGraph_HLOC_Band::display_trend(const ACE_Time_Value& t_, const LFBoxTrend& trend_, double p_)
{
	LF::RefPair<TC_GraphDataPtr> solid(&_v[1], &_v[3]);
	LF::RefPair<TC_GraphDataPtr> dash(&_v[0], &_v[2]);

	if (trend_[0].valid())
	{
		const LFSide* s = trend_[0]._side;
//		solid[s->value()]->display_point(t_, trend_[0].roll_end_p());
		solid[s->c_value()]->display_point(t_, 0);
	}
	if (trend_[1].valid())
	{
		const LFSide* s = trend_[1]._side;
//		dash[s->value()]->display_point(t_, trend_[1].roll_end_p());
		dash[s->c_value()]->display_point(t_, 0);
	}

}
void LFGraph_HLOC_Band::display_expected(const ACE_Time_Value& t_, const LFBoxTrend& trend_, double p_)
{
	if (trend_[0].valid() && trend_[1].valid())
	{
		if (trend_[0]._side->value() == LF::s_BUY)
		{
			_v[0]->display_point(t_, trend_[1].exp_contra_p());
			_v[1]->display_point(t_, trend_[1].exp_contra_p());
			_v[2]->display_point(t_, trend_[0].exp_contra_p());
			_v[3]->display_point(t_, trend_[0].exp_contra_p());
		}
		else
		{
			_v[0]->display_point(t_, trend_[0].exp_contra_p());
			_v[1]->display_point(t_, trend_[0].exp_contra_p());
			_v[2]->display_point(t_, trend_[1].exp_contra_p());
			_v[3]->display_point(t_, trend_[1].exp_contra_p());
		}

	}
	else
	{
		for (size_t i = 0; i < 4; ++i)
			_v[i]->display_point(t_, 0);
	}

}

void LFGraph_HLOC_Band::init(TC_Graph* g_,
			  int& first_subset_,
			  int axis_,
			  const std::string& group_name_)
{
	for(size_t i = 0; i < _v.size(); ++i)
	{
		_v[i]->init(g_, first_subset_++, axis_);
//		g_->add_to_group(group_name_, _v[i].dptr());
	}
}
//======================================================================
// LFGraph_Panel_Scope
//======================================================================
LFGraph_Panel_Scope::LFGraph_Panel_Scope(const std::string& name_)
:LFGraph_Panel(name_)
{
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[0], up
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[0], down
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[1], up
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[1], down
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[2], up
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // lcy[2], down
	_gd_v.push_back(new TC_Graph_Last(FOLLOWER_STR, ysAuto)); // Mid Price

	load_resource();
}
void LFGraph_Panel_Scope::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(7);
}
void LFGraph_Panel_Scope::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;

	DWORD col = 0;
	int nSLT = 0;

	col = TC_RGB::Green;
	nSLT = PELT_THINSOLID;
	_gd_v[0]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("0up"));
	_g->map_subset("SCOPE", su_first);
	su_first++;
	col = TC_RGB::Red;
	nSLT = PELT_THINSOLID;
	_gd_v[1]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("0down"));
	_g->map_subset("SCOPE", su_first);
	su_first++;
	
	col = TC_RGB::Green;
	nSLT = PELT_THINSOLID;
	_gd_v[2]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("1up"));
	_g->map_subset("SCOPE", su_first);
	su_first++;
	col = TC_RGB::Red;
	nSLT = PELT_THINSOLID;
	_gd_v[3]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("1down"));
	_g->map_subset("SCOPE", su_first);
	su_first++;
	
	col = TC_RGB::Green;
	nSLT = PELT_THINSOLID;
	_gd_v[4]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("2up"));
	_g->map_subset("SCOPE", su_first);
	su_first++;
	col = TC_RGB::Red;
	nSLT = PELT_THINSOLID;
	_gd_v[5]->init(_g, subset_lines++, axis);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("2down"));
	_g->map_subset("SCOPE", su_first);
	su_first++;

	//-------------------------------------------------------------
	// Price
	//-------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:SCOPE:Price_color", "SteelBlue"));
	nSLT = PELT_THINSOLID;

	_gd_v[6]->init(_g, subset_lines++, axis);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Scopes"));

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("Scope|S"));
	_g->map_subset("SCOPE", su_first);
	su_first++;

	_g->map_axis("SCOPE", axis_);

	init_scope_annotations(FOLLOWER_STR);
}
void LFGraph_Panel_Scope::init_scope_annotations(const std::string& actor_)
{
	TC_AnnotationData pa;
	pa._active = true;

	

	for (size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		TCResourceMap	color_map;
		std::ostringstream cos;
		cos << "LFGUI:Panel:HLOC:expected_color:" << actor_ << ":" << i << ":0" ;
		DWORD col = ZARGB(Z::get_setting(cos.str(), "Magenta"));

		pa._ann_text_size = 85;
		std::string scope_id = ScopeTables::volume_g_id(actor_, i);

		pa._pa_group_size = 6;
		pa._pa_group_name = scope_id;
		pa.set_group(scope_id);
		pa._ann_type = (i == 0) ? PEGAT_THINSOLIDLINE : PEGAT_LINECONTINUE;
		pa._ann_axis = _g->get_axis("SCOPE");
		pa._pa_type = PEP_faGRAPHANNOTATIONX;
		pa._ann_color = col;// TC_RGB::DarkMagenta;

		_g->add_annotation(pa); // Graph up/down levels annotations fixed group
		_g->map_annot(pa._pa_group_name, scope_id);

		scope_id = ScopeTables::volume_t_id(actor_, i);
		pa._pa_group_name = scope_id;
		pa.set_group(scope_id);
		pa._ann_type = PEGAT_POINTER;

		_g->add_annotation(pa); // Graph up/down levels annotations fixed group
		_g->map_annot(pa._pa_group_name, scope_id);


	}
}

void LFGraph_Panel_Scope::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();

	if (_show_lcy_trends)
	{
		LFBoxTrends lcy = d_._f_1._lcy_trends;
		std::bitset<6> p_ind(0x3F);
		if (lcy[0][0].valid())
		{
			LFBox* lcy_rl = &lcy[0][0];
			if (lcy[0][1].valid())
				lcy_rl = &lcy[0][1];
			if (lcy_rl->_side->value() == LF::s_BUY)
			{
				_gd_v[0]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(0, false);
			}
			else
			{
				_gd_v[1]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(1, false);
			}
		}
		if (lcy[1][0].valid())
		{
			LFBox* lcy_rl = &lcy[1][0];
			if (lcy[1][1].valid())
				lcy_rl = &lcy[1][1];
			if (lcy_rl->_side->value() == LF::s_BUY)
			{
				_gd_v[2]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(2, false);
			}
			else
			{
				_gd_v[3]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(3, false);
			}
		}
		if (lcy[2][0].valid())
		{
			LFBox* lcy_rl = &lcy[2][0];
			if (lcy[2][1].valid())
				lcy_rl = &lcy[2][1];
			if (lcy_rl->_side->value() == LF::s_BUY)
			{
				_gd_v[4]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(4, false);
			}
			else
			{
				_gd_v[5]->display_point(t, lcy_rl->_w._cluster_dp->_p);
				p_ind.set(5, false);
			}
		}
		for (size_t i = 0; i < 6; ++i)
		{
			if (p_ind.test(i))
				_gd_v[i]->display_point(t, d_._f_1._p);
		}
	}
	_gd_v[6]->display_point(t, d_._f_1._p);
}
void LFGraph_Panel_Scope::reset()
{
	LFGraph_Panel::reset();
	load_resource();
}
void LFGraph_Panel_Scope::load_resource()
{
	_show_lcy_trends = Z::get_setting_bool("LFModel:LFGUI:show_lcy_trends", false);
}
//======================================================================
// LFGraph_Panel_Energy
//======================================================================

void LFGraph_Panel_Energy::reset()
{
	LFGraph_Panel::reset();
	load_resource();
	_box.reset();
}
void LFGraph_Panel_Energy::load_resource()
{
	_activity_level_high = Z::get_setting_double		("LFModel:follower:activity_level_high", 66.);
	_activity_level_low = Z::get_setting_double		("LFModel:follower:activity_level_low", 66.);
}

LFGraph_Panel_Energy::LFGraph_Panel_Energy(const std::string& name_)
:LFGraph_Panel(name_)
{
	_gd_v.push_back(new TC_Graph_Last("V" + name_, TC_GraphData::ysSymmetrical)); // VMA
	_gd_v.push_back(new TC_Graph_Last("O" + name_, TC_GraphData::ysSymmetrical)); // OBV
	_gd_v.push_back(new TC_Graph_Last("A" + name_, TC_GraphData::ysSymmetrical)); // E Activity
	_gd_v.push_back(new TC_Graph_Last("a" + name_, TC_GraphData::ysSymmetrical)); // P Activity
	_gd_v.push_back(new TC_Graph_Last("M" + name_, TC_GraphData::ysSymmetrical)); // MFI
	_gd_v.push_back(new TC_Graph_Last("C" + name_, TC_GraphData::ysSymmetrical)); // Correlation

	load_resource();
}
void LFGraph_Panel_Energy::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(1);
	v_.push_back(1);
	v_.push_back(2);
	v_.push_back(1);
	v_.push_back(1);
}

void LFGraph_Panel_Energy::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;

	//-------------------------------------------------------------
	// VMA
	//-------------------------------------------------------------

	DWORD col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:VMA_color", "Green"));
	int nSLT = PELT_THINSOLID;

	_gd_v[0]->init(_g, subset_lines++, axis);
	_g->add_to_group(FOLLOWER_STR, _gd_v[0].dptr());

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("VMA"));

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("VMA|V"));
	_g->map_subset("VACTIVITY", su_first);
	su_first++;
	axis++;

	//-------------------------------------------------------------
	// OBV
	//-------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:OBV_color", "Blue"));
	nSLT = PELT_THINSOLID;

	_gd_v[1]->init(_g, subset_lines++, axis);
	_g->add_to_group(FOLLOWER_STR, _gd_v[1].dptr());

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("OBV"));

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("OBV|O"));
	_g->map_subset("ENERGY", su_first);
	su_first++;
	axis++;

	//-------------------------------------------------------------
	// Activity
	//-------------------------------------------------------------

	col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:EActivity_color", "ForrestGreen"));
	nSLT = PELT_THINSOLID;
	_gd_v[2]->init(_g, subset_lines++, axis);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[2].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Activity"));


	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("E Activity|A"));
	_g->map_subset("ACTIVITY", su_first);
	su_first++;

	//-------------------------------------------------------------
	// Price Activity
	//-------------------------------------------------------------

	col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:PActivity_color", "Magenta"));
	nSLT = PELT_THINSOLID;
	_gd_v[3]->init(_g, subset_lines++, axis);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[3].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("P Activity|a"));
	_g->map_subset("ACTIVITY", su_first);
	su_first++;

	//-------- Set Horizontal Activity Annotations
	TC_AnnotationData pa;

	pa._ann_type = PELT_THINSOLID;
	pa._pa_type = PEP_faHORZLINEANNOTATION;
	pa._ann_axis = axis;
	pa._Y = _activity_level_high;
	pa._ann_color = ZARGB(Z::get_setting("LFModel:follower:box_activity_h_color", "Red"));
	_g->add_annotation(pa); // Horizontal +Upper Activity annotaion

	pa._pa_group_name.clear();
	pa._Y *= -1.;
	_g->add_annotation(pa); // Horizontal -Upper Activity annotaion
	_g->map_axis("ENERGY:leader", axis);
	axis++;

	//-------------------------------------------------------------
	// MFI
	//-------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:MFI_color", "Red"));
	nSLT = PELT_THINSOLID;

	_gd_v[4]->init(_g, subset_lines++, axis);
	_g->add_to_group(FOLLOWER_STR, _gd_v[4].dptr());

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("MFI"));

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("MFI|M"));
	_g->map_subset("ENERGY", su_first);
	su_first++;
	axis++;


	//-------------------------------------------------------------
	// Correlation
	//-------------------------------------------------------------

	col = ZARGB(Z::get_setting("LFGUI:Panel:ENERGY:Correlation_color", "Black"));
	_gd_v[5]->init(_g, subset_lines++, axis);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[5].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
	PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Correlation"));

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("Correlation|C"));
	_g->map_subset("ENERGY", su_first);
	_g->map_axis("CORRELATION:leader", axis);

	su_first++;
	axis++;

}

void LFGraph_Panel_Energy::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();
	if(d_._exec.valid())
	{
		TC_AnnotationData pa(PEGAT_POINTER, PEP_faGRAPHANNOTATIONX);
		pa._X = TC_Graph::get_vb_time(t);
		pa._Y = d_._exec._p;
		pa._ann_axis = _g->get_axis("HLOC:follower");
		pa._ann_text_size = 115;

		ACE_OS::memset(pa._label, 0, sizeof(pa._label));
		std::string format = "%s %s%.0f@" + LFSecurityMaster::instance()->get_fprecision();
		ACE_OS::sprintf(pa._label, format.c_str(), P_FORMAT::side_p()(d_._exec._side->contra_s()).c_str(), d_._exec._comment.c_str(), d_._exec._q, d_._exec._p);

		pa._ann_color = TC_RGB::Black;
		pa.set_group("ENERGY:2");

		_g->add_annotation(pa); //Graph exec annotation
		_g->map_annot(pa._pa_group_name, "HLOC");
	}

	_gd_v[0]->display_point(t, d_._f_1._vma);
	_gd_v[1]->display_point(t, d_._f_1._obv);
	_gd_v[2]->display_point(t, d_._f_1._v_activity);
	_gd_v[3]->display_point(t, d_._f_1._p_activity);
//	_gd_v[4]->display_point(t, d_._f_1._mfi*d_._f_1._vma / 100.);
	_gd_v[4]->display_point(t, d_._f_1._mfi);
	_gd_v[5]->display_point(t, d_._f_1._corr);

}
//======================================================================
// LFGraph_Panel_EnergyCharge
//======================================================================

void LFGraph_Panel_EnergyCharge::reset()
{
	LFGraph_Panel::reset();
	load_resource();
	_box.reset();
}
void LFGraph_Panel_EnergyCharge::load_resource()
{
}

LFGraph_Panel_EnergyCharge::LFGraph_Panel_EnergyCharge(const std::string& name_)
:LFGraph_Panel(name_)
{
//	_gd_v.push_back(new TC_Graph_Last("G" + name_, TC_GraphData::ysSymmetrical)); // E Charge
	_gd_v.push_back(new TC_Graph_Last("M" + name_, TC_GraphData::ysSymmetrical)); // E MicroBias

	for (size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		std::ostringstream os; os << i;
		_gd_v.push_back(new TC_Graph_Last(os.str() + name_, TC_GraphData::ysSymmetrical)); // P RollBias
//		_gd_v.push_back(new TC_Graph_Last(os.str() + name_, TC_GraphData::ysAuto)); // P RollBias

	}
//	_gd_v.push_back(new TC_Graph_Last("E" + name_)); // E End

	load_resource();
}
void LFGraph_Panel_EnergyCharge::subsets_by_axes(std::vector<int>& v_)
{
//	v_.push_back(1);
//	v_.push_back(1);
	v_.push_back(1 + LFScopeStore::NUMBER_OF_SCOPES);
//	v_.push_back(1);
}

void LFGraph_Panel_EnergyCharge::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;
	size_t index = 0;

/*
	//------------------------------------------------------------------------
	// Energy Charge
	//------------------------------------------------------------------------

	DWORD col = ZARGB(Z::get_setting("LFGUI:Panel:ECHARGE:ECharge_color", "ForestGreen"));
	int nSLT = PELT_EXTRATHINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("E Charge|G"));
	_g->map_subset("ECHARGE", su_first);
	su_first++;
	index++;
	axis++;
*/
	//------------------------------------------------------------------------
	// Energy Micro Bias
	//------------------------------------------------------------------------

	DWORD col = ZARGB(Z::get_setting("LFGUI:Panel:ECHARGE:EMicro_color", "FireBrick"));
	int nSLT = PELT_THINSOLID;
	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
//	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("E Micro|M"));
	_g->map_subset("ECHARGE", su_first);
	su_first++;
	index++;
	//-------- Set Horizontal VIAS levels
	TC_AnnotationData pa;

	pa._ann_type = PELT_THINSOLID;
	pa._pa_type = PEP_faHORZLINEANNOTATION;
	pa._ann_axis = axis;
	pa._Y = 0.5;
	pa._ann_color = TC_RGB::Red;
	_g->add_annotation(pa); // Horizontal +Vias annotaion

	pa._pa_group_name.clear();
	pa._Y *= -1.;
	_g->add_annotation(pa); // Horizontal -Vias annotaion

	init_scope_bias(FOLLOWER_STR, FOLLOWER_STR, index, su_first, subset_lines, axis);
	if (TCSettings::get_resource("LFGUI:Panel:HLOC:leader:expected:on").get_bool())
		init_scope_bias(LEADER_STR, LEADER_STR, index, su_first, subset_lines, axis);
/*
	//------------------------------------------------------------------------
	// Eng End
	//------------------------------------------------------------------------

	col = ZARGB(Z::get_setting("LFGUI:Panel:ECHARGE:EEnd_color", "Gray"));
	nSLT = PELT_THINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("E End"));

	_g->map_subset("ECHARGE", su_first);
*/


	su_first++;
	index++;
	axis++;
}
void LFGraph_Panel_EnergyCharge::init_scope_bias(const std::string& actor_,
	const std::string& group_,
	size_t& index_,
	int& su_first_,
	int& subset_lines_,
	int& axis_)
{
	DWORD col = 0;
	int nSLT = 0;

	TCResourceMap	color_map;
	TCResourceMap	line_map;

	std::ostringstream cos;
	cos << "LFGUI:Panel:ECHARGE:bias_color:" << actor_ << ":*";
	TCSettings::get_resource_match(cos.str(), color_map);

	std::ostringstream los;
	los << "LFGUI:Panel:ECHARGE:bias_line:" << actor_ << ":*";
	TCSettings::get_resource_match(los.str(), line_map);

	if (color_map.empty())
		z_throw("ABSENT ECHARGE COLOR SETTINGS");

	if (line_map.empty())
		z_throw("ABSENT ECHARGE LINE SETTINGS");

	TCResourceMap::iterator li = line_map.begin();
	TCResourceMap::iterator ci = color_map.begin();

	_g->map_axis("ECHARGE", axis_);
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis_);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	//PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	if (Z::get_setting_bool("LFModel:follower:crl_bias_use_density", false))
		PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Density Bias"));
	else
		PEszset(m_hPE, PEP_szYAXISLABEL, CA2T("Volume Bias"));


	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	for (size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		col = ZARGB((*ci).second->get_value());
		int nSLT = (*li).second->get_integer();

		_gd_v[index_]->init(_g, subset_lines_, axis_);
		_gd_v[index_]->set_null_data(0.0001);

//		std::ostringstream os; os << "ECHARGE:" << actor_ << ":" << i;
//		_g->map_axis(os.str(), axis_);
		/*
		PEnset(m_hPE, PEP_nWORKINGAXIS, axis_);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
		PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);
		*/
		std::string vias_id = ScopeTables::vias_id(actor_, i);
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first_, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first_, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first_, CA2T(vias_id.c_str()));

		_g->map_subset(vias_id, su_first_, "ECHARGE");

		++ci;
		if (ci == color_map.end()) ci = color_map.begin();
		++li;
		if (li == line_map.end()) li = line_map.begin();

		++subset_lines_;
		++su_first_;
		++index_;
	}
	axis_++;
}

void LFGraph_Panel_EnergyCharge::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();

	if(d_._f_1._box.valid())
	{
//		_gd_v[0]->display_point(t, d_._f_1._box._charge);
		_gd_v[0]->display_point(t, d_._f_1._box._bias);

		for (size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			if (d_._f_1._box_trends[i][0].valid())
				_gd_v[i+1]->display_point(t, d_._f_1._box_trends[i][0]._bias);
			else
				_gd_v[i+1]->display_point(t, 0);
		}
	}
	else
	{
		for(size_t i = 0; i < _gd_v.size(); ++i)
			_gd_v[i]->display_point(t, 0);
	}

}
//======================================================================
// LFGraph_Panel_PriceCharge
//======================================================================

void LFGraph_Panel_PriceCharge::reset()
{
	LFGraph_Panel::reset();
	load_resource();
	_box.reset();
}
void LFGraph_Panel_PriceCharge::load_resource()
{
	_show_charge_annot = TCSettings::get_bool("LFModel:LFGUI:show_charge_annot", false);

}
LFGraph_Panel_PriceCharge::LFGraph_Panel_PriceCharge(const std::string& name_)
:LFGraph_Panel(name_)
{
	_gd_v.push_back(new TC_Graph_Last("p" + name_, TC_GraphData::ysSymmetrical)); // P Charge
	_gd_v.push_back(new TC_Graph_Last("m" + name_, TC_GraphData::ysSymmetrical)); // P MicroBias

	for(size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		std::ostringstream os; os << i;
		_gd_v.push_back(new TC_Graph_Last(os.str() + name_, TC_GraphData::ysSymmetrical)); // P RollBias
	}
//	_gd_v.push_back(new TC_Graph_Last("e" + name_, TC_GraphData::ysSymmetrical)); // P End
	load_resource();

}
void LFGraph_Panel_PriceCharge::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(1);
	v_.push_back(1);
//	for(size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
// make all trends/expectations on same axis
	v_.push_back(LFScopeStore::NUMBER_OF_SCOPES);
}

void LFGraph_Panel_PriceCharge::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;
	size_t index = 0;
	DWORD col = 0;
	int nSLT = 0;
	//------------------------------------------------------------------------
	// Price Charge
	//------------------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:PCHARGE:PCharge_color", "ForestGreen"));
	nSLT = PELT_EXTRATHINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	//	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("P Charge|p"));
	_g->map_subset("PCHARGE", su_first);
	su_first++;
	index++;
	axis++;

	//------------------------------------------------------------------------
	// Price MicroBias
	//------------------------------------------------------------------------

	col = ZARGB(Z::get_setting("LFGUI:Panel:PCHARGE:PMicro_color", "FireBrick"));
	nSLT = PELT_THINSOLID;
	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
//	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("P Micro|m"));
	_g->map_subset("PCHARGE", su_first);
	su_first++;
	index++;
	axis++;

	init_scope_bias(FOLLOWER_STR, FOLLOWER_STR, index, su_first, subset_lines, axis);
	if(TCSettings::get_resource("LFGUI:Panel:HLOC:leader:expected:on").get_bool())
		init_scope_bias(LEADER_STR, LEADER_STR, index, su_first, subset_lines, axis);

}
void LFGraph_Panel_PriceCharge::init_scope_bias(const std::string& actor_,
												const std::string& group_,
												size_t& index_,
												int& su_first_,
												int& subset_lines_,
												int& axis_)
{
	DWORD col = 0;
	int nSLT = 0;

	TCResourceMap	color_map;
	TCResourceMap	line_map;

	std::ostringstream cos;
	cos << "LFGUI:Panel:PCHARGE:bias_color:" << actor_ << ":*";
	TCSettings::get_resource_match(cos.str(), color_map);

	std::ostringstream los;
	los << "LFGUI:Panel:PCHARGE:bias_line:" << actor_ << ":*";
	TCSettings::get_resource_match(los.str(), line_map);

	if(color_map.empty())
		z_throw("ABSENT PCHARGE COLOR SETTINGS");

	if(line_map.empty())
		z_throw("ABSENT PCHARGE LINE SETTINGS");

	TCResourceMap::iterator li = line_map.begin();
	TCResourceMap::iterator ci = color_map.begin();

	for(size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		col = ZARGB((*ci).second->get_value());
		int nSLT = (*li).second->get_integer();

		_gd_v[index_]->init(_g, subset_lines_, axis_);
		_gd_v[index_]->set_null_data(0.0001);

		std::ostringstream os; os << "PCHARGE:" << actor_ << ":" << i;
	//	_g->add_to_group(group_, _gd_v[index_].dptr());
		_g->map_axis(os.str(), axis_);

		PEnset(m_hPE, PEP_nWORKINGAXIS, axis_);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
//		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
		PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

		std::string pias_id = ScopeTables::pias_id(actor_, i);
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first_, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first_, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first_, CA2T(pias_id.c_str()));
		

		_g->map_subset(pias_id, su_first_, "PCHARGE");
//		_g->map_subset(aos.str(), su_first_, ScopeTables::make_scope_annot_id(actor_, i, "T"));
//		g_->map_subset(ScopeTables::make_scope_annot_id(actor_, i), su_first_);
//		g_->map_subset("PCHARGE", su_first_);

		++ci;
		if(ci == color_map.end()) ci = color_map.begin();
		++li;
		if(li == line_map.end()) li = line_map.begin();

		++subset_lines_;
		++su_first_;
		++index_;
//		++axis_;
	}
	axis_++;

}

void LFGraph_Panel_PriceCharge::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();

	if(d_._f_1._box.valid())
	{
		_gd_v[0]->display_point(t, d_._f_1._box.p_charge());

		_gd_v[1]->display_point(t, d_._f_1._box._pias);
		if (_show_charge_annot && !d_._f_1._box.equal(_box))
		{
			const LFSide* bside = d_._f_1._latter_box._side;
			if (d_._f_1._latter_box.valid() )
			{
				TC_AnnotationData pa;
				pa._X = TC_Graph::get_vb_time(t);
				pa._ann_axis = _g->get_axis("HLOC:follower");
				pa._ann_text_size = 115;
				pa._ann_type = PELT_DASH;
				pa._ann_color = TC_RGB::Green;
				if(bside->sign() > 0)
					pa._ann_color = TC_RGB::Red;

				pa.set_group("ENERGY:3");

				_g->add_annotation(pa);
				_g->map_annot(pa._pa_group_name, "PCHARGE");
			}
		}
		for (size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
		{
			const LFBox* b =  &d_._f_1._box_trends[i][0];
			if(b->valid())
				_gd_v[i+2]->display_point(t, b->_pias);
			else
				_gd_v[i+2]->display_point(t, 0);
		}

		_box = d_._f_1._box;
	}
	else
	{
		for(size_t i = 0; i < _gd_v.size(); ++i)
			_gd_v[i]->display_point(t, 0);
	}

}

//======================================================================
// LFGraph_Panel_Volume
//======================================================================

LFGraph_Panel_Volume::LFGraph_Panel_Volume(const std::string& name_)
	:LFGraph_Panel(name_),
	_scope(0),
	_keep_last(true)
{
	load_resource();

//	_gd_v.push_back(new TC_Graph_Last("V" + name_, TC_GraphData::ysSymmetrical)); // Volume Total
//	_gd_v.push_back(new TC_Graph_Last("v" + name_, TC_GraphData::ysSymmetrical)); // Volume Contra
	
	for (size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		std::ostringstream os; os << i;
		_gd_v.push_back(new TC_Graph_Last("V" + os.str() + name_, TC_GraphData::ysSymmetrical)); // Scope Volume Total
		_gd_v.push_back(new TC_Graph_Last("v" + os.str() + name_, TC_GraphData::ysSymmetrical)); // Scope Volume Contra
	}
	
}
void LFGraph_Panel_Volume::reset()
{
	LFGraph_Panel::reset();
	load_resource();
}

void LFGraph_Panel_Volume::load_resource()
{
	_scope = TCSettings::get_integer("LFModel:LFGUI:VOLUME:scope", 0);
	_keep_last = TCSettings::get_bool("LFModel:LFGUI:VOLUME:keep_last", true);

}

void LFGraph_Panel_Volume::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(/*2 + */LFScopeStore::NUMBER_OF_SCOPES * 2);
}

void LFGraph_Panel_Volume::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;
	size_t index = 0;
	DWORD col = 0;
	int nSLT = 0;
	//------------------------------------------------------------------------
	// Volume Total
	//------------------------------------------------------------------------
	/*
	col = ZARGB(Z::get_setting("LFGUI:Panel:VOLUME:Volume_color", "Blue"));
	nSLT = PELT_EXTRATHINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	//	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("VolumeT"));
	_g->map_subset("VOLUME", su_first);
	su_first++;
	index++;

	//------------------------------------------------------------------------
	// Volume Contra
	//------------------------------------------------------------------------
	col = TC_RGB::Red;// ZARGB(Z::get_setting("LFGUI:Panel:VOLUME:Volume_color", "Blue"));
	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	//	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("VolumeC"));
	_g->map_subset("VOLUME", su_first);

	*/
	col = ZARGB(Z::get_setting("LFGUI:Panel:VOLUME:Volume_color", "Blue"));
	//-------- Set Horizontal VOLUME levels
	TC_AnnotationData pa;
	pa._ann_type = PELT_THINSOLID;
	pa._pa_type = PEP_faHORZLINEANNOTATION;
	pa._ann_axis = axis;
	pa._Y = Z::get_setting_double("LFGUI:Panel:VOLUME:Volume_level", 1000);
	pa._ann_color = col; // TC_RGB::Red;
	_g->add_annotation(pa); // Horizontal +VOLUME annotaion

	pa._pa_group_name.clear();
	pa._Y *= -1.;
	_g->add_annotation(pa); // Horizontal -VOLUME annotaion

//	su_first++;
//	index++;

	
	init_scope_bias(FOLLOWER_STR, FOLLOWER_STR, index, su_first, subset_lines, axis);
	if (TCSettings::get_resource("LFGUI:Panel:HLOC:leader:expected:on").get_bool())
		init_scope_bias(LEADER_STR, LEADER_STR, index, su_first, subset_lines, axis);
}
void LFGraph_Panel_Volume::init_scope_bias(const std::string& actor_,
	const std::string& group_,
	size_t& index_,
	int& su_first_,
	int& subset_lines_,
	int& axis_)
{
	DWORD col = 0;
	int nSLT = 0;

	TCResourceMap	color_map;
	TCResourceMap	line_map;

	std::ostringstream cos;
	cos << "LFGUI:Panel:VOLUME:bias_color:" << actor_ << ":*";
	TCSettings::get_resource_match(cos.str(), color_map);

	std::ostringstream los;
	los << "LFGUI:Panel:VOLUME:bias_line:" << actor_ << ":*";
	TCSettings::get_resource_match(los.str(), line_map);

	if (color_map.empty())
		z_throw("ABSENT VOLUME COLOR SETTINGS");

	if (line_map.empty())
		z_throw("ABSENT VOLUME LINE SETTINGS");

	_g->map_axis("VOLUME", axis_);
	TCResourceMap::iterator li = line_map.begin();
	TCResourceMap::iterator ci = color_map.begin();

	for (size_t i = 0; i <LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		col = ZARGB((*ci).second->get_value());
		nSLT = (*li).second->get_integer();

		_gd_v[index_]->init(_g, subset_lines_, axis_);
		_gd_v[index_]->set_null_data(0.0001);

		//std::ostringstream os; os << "VOLUME:" << actor_ << ":" << i;
		std::string volumet_id = ScopeTables::volumet_id(actor_, i); //Total Volume
		_g->map_axis(volumet_id, axis_);

		PEnset(m_hPE, PEP_nWORKINGAXIS, axis_);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
		PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first_, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first_, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first_, CA2T(volumet_id.c_str()));

		_g->map_subset(volumet_id, su_first_, "VOLUME");

		++ci;
		if (ci == color_map.end()) ci = color_map.begin();
		++li;
		if (li == line_map.end()) li = line_map.begin();

		++subset_lines_;
		++su_first_;
		++index_;

		col = ZARGB((*ci).second->get_value());
		nSLT = (*li).second->get_integer();

		_gd_v[index_]->init(_g, subset_lines_, axis_);
		_gd_v[index_]->set_null_data(0.0001);

		//std::ostringstream os; os << "VOLUME:" << actor_ << ":" << i;
		std::string volumec_id = ScopeTables::volumec_id(actor_, i); //Total Volume
		_g->map_axis(volumec_id, axis_);

		PEnset(m_hPE, PEP_nWORKINGAXIS, axis_);
		PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
		PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);
		PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

		
		PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first_, &col);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first_, &nSLT);
		PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first_, CA2T(volumec_id.c_str()));

		_g->map_subset(volumec_id, su_first_, "VOLUME");

		++ci;
		if (ci == color_map.end()) ci = color_map.begin();
		++li;
		if (li == line_map.end()) li = line_map.begin();

		++subset_lines_;
		++su_first_;
		++index_;
		
	}
	axis_++;

}

void LFGraph_Panel_Volume::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();
	for (size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		size_t index = i * 2;
		LF::RefPair<TC_GraphDataPtr> vol(&_gd_v[index], &_gd_v[index+1]);

		LFBoxTrend& scope = d_._f_1._box_trends[i];
		if (scope[0].is_valid())
		{
			_gd_v[index]->display_point(t, LF::volume_i(scope[0]._side, scope[0]._w._cluster_dp, scope[0]._w._dp)*scope[0]._side->sign());
			_gd_v[index + 1]->display_point(t, LF::volume_i(scope[0]._side->contra_s(), scope[0]._w._cluster_dp, scope[0]._w._dp)*scope[0]._side->contra_s()->sign());
		}
		else
		{
			_gd_v[index]->display_point(t, 0);
			_gd_v[index+1]->display_point(t, 0);
		}
	}
}

//======================================================================
// LFGraph_Panel_Density
//======================================================================

LFGraph_Panel_Density::LFGraph_Panel_Density(const std::string& name_)
	:LFGraph_Panel(name_),
	_scope(0),
	_keep_last(true)
{
	load_resource();
	_gd_v.push_back(new TC_Graph_Last("D" + name_, TC_GraphData::ysSymmetrical)); // DensityUP
	_gd_v.push_back(new TC_Graph_Last("d" + name_, TC_GraphData::ysSymmetrical)); // DensityDOWN
	_gd_v.push_back(new TC_Graph_Last("F" + name_, TC_GraphData::ysSymmetrical)); // FrequencyUP
	_gd_v.push_back(new TC_Graph_Last("f" + name_, TC_GraphData::ysSymmetrical)); // FrequencyDOWN
}
void LFGraph_Panel_Density::reset()
{
	LFGraph_Panel::reset();
	load_resource();
}

void LFGraph_Panel_Density::load_resource()
{
	_scope = TCSettings::get_integer("LFModel:LFGUI:VOLUME:scope", 0);
	_keep_last = TCSettings::get_bool("LFModel:LFGUI:VOLUME:keep_last", true);
}

void LFGraph_Panel_Density::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(2);
	v_.push_back(2);
}

void LFGraph_Panel_Density::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int su_first = first_subset_;
	int subset_lines = first_subset_;
	int axis = axis_;
	size_t index = 0;
	DWORD col = 0;
	int nSLT = 0;
	//------------------------------------------------------------------------
	// DensityUP
	//------------------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:DENSITY:Density_color", "ForrestGreen"));
	nSLT = PELT_EXTRATHINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("DensityUP"));
	_g->map_subset("DENSITY", su_first);
	su_first++;
	index++;

	//------------------------------------------------------------------------
	// DensityDOWN
	//------------------------------------------------------------------------

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("DensityDOWN"));
	_g->map_subset("DENSITY", su_first);
	su_first++;
	index++;

	//-------- Set Horizontal Density levels
	TC_AnnotationData pa;
	pa._ann_type = PELT_THINSOLID;
	pa._pa_type = PEP_faHORZLINEANNOTATION;
	pa._ann_axis = axis;
	pa._Y = Z::get_setting_double("LFGUI:Panel:DENSITY:Density_level", 100);
	pa._ann_color = col; // TC_RGB::Red;
	_g->add_annotation(pa); // Horizontal +Density annotaion

	pa._pa_group_name.clear();
	pa._Y *= -1.;
	_g->add_annotation(pa); // Horizontal -Density annotaion


	axis++;
	//------------------------------------------------------------------------
	// FrequencyUP
	//------------------------------------------------------------------------
	col = ZARGB(Z::get_setting("LFGUI:Panel:DENSITY:Frequency_color", "Magenta"));
	nSLT = PELT_EXTRATHINSOLID;

	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("FrequencyUP"));
	_g->map_subset("DENSITY", su_first);
	su_first++;
	index++;
	//------------------------------------------------------------------------
	// FrequencyDOWN
	//------------------------------------------------------------------------
	_gd_v[index]->init(_g, subset_lines++, axis);
	_gd_v[index]->set_null_data(0.0001);
	_g->add_to_group(FOLLOWER_STR, _gd_v[index].dptr());
	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_EMPTY);
	PEnset(m_hPE, PEP_bAUTOPADBEYONDZEROY, TRUE);

	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, su_first, &col);
	PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, su_first, &nSLT);
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, su_first, CA2T("FrequencyDOWN"));
	_g->map_subset("DENSITY", su_first);
	su_first++;
	index++;

	//-------- Set Horizontal Frequency levels
//	TC_AnnotationData pa;
	pa._ann_type = PELT_THINSOLID;
	pa._pa_type = PEP_faHORZLINEANNOTATION;
	pa._ann_axis = axis;
	pa._Y = Z::get_setting_double("LFGUI:Panel:DENSITY:Frequency_level", 1000);
	pa._ann_color = col; // TC_RGB::Red;
	_g->add_annotation(pa); // Horizontal +Frequency annotaion

	pa._pa_group_name.clear();
	pa._Y *= -1.;
	_g->add_annotation(pa); // Horizontal -Frequency annotaion

	axis++;
}

void LFGraph_Panel_Density::process(LFStrategyData& d_)
{
	ACE_Time_Value t = d_.get_t();
	LF::RefPair<TC_GraphDataPtr> den(&_gd_v[0], &_gd_v[1]);
	LF::RefPair<TC_GraphDataPtr> freq(&_gd_v[2], &_gd_v[3]);

	if (_scope >= 0 && _scope < d_._f_1._box_trends.size())
	{
		LFBoxTrend& scope = d_._f_1._box_trends[_scope];

		if (scope[0].valid())
		{
			//den[scope[0]._side->value()]->display_point(t, std::max(scope[0]._w._cluster_density, scope[2]._w._cluster_density)*scope[0]._side->sign());
			//freq[scope[0]._side->value()]->display_point(t, std::max(scope[0]._w._cluster_frequency, scope[2]._w._cluster_frequency)*scope[0]._side->sign());
			den[scope[0]._side->value()]->display_point(t, scope[0]._w._cluster_density*scope[0]._side->sign());
			freq[scope[0]._side->value()]->display_point(t, scope[0]._w._cluster_frequency*scope[0]._side->sign());
		}
		if (scope[1].valid())
		{
			double dd = _keep_last ? scope[1]._w._cluster_density*scope[1]._side->sign() : 0;
			double ff = _keep_last ? scope[1]._w._cluster_frequency*scope[1]._side->sign() : 0;
			den[scope[1]._side->value()]->display_point(t, dd);
			freq[scope[1]._side->value()]->display_point(t, ff);
		}
	}
	else
	{
		for (size_t i = 0; i < _gd_v.size(); ++i)
			_gd_v[i]->display_point(t, 0);
	}
}
//======================================================================
// LFGraph_StrategyData
//======================================================================
LFGraph_StrategyData::LFGraph_StrategyData(BMStrategy& s_, const std::string& key_, const std::string& input_,
		bool always_inactive_, long hwm_, long lwm_)
	: TC_GraphData("Strategy"),
	_sub(s_, key_, input_, always_inactive_, hwm_, lwm_),
	_sub_pos(s_, LF::make_strategy_key(key_, theApp.follower()), "f_depth", true, hwm_, lwm_)
{
	//-- Prices
	if(TCSettings::get_bool("LFGUI:Panel:HLOC:on"))
		_gdv.push_back(new LFGraph_Panel_HLOC("HLOC"));

	//-- Energy
	if(TCSettings::get_bool("LFGUI:Panel:ENERGY:on"))
		_gdv.push_back(new LFGraph_Panel_Energy("ENERGY"));

	//-- Energy Charges
	if(TCSettings::get_bool("LFGUI:Panel:ECHARGE:on"))
		_gdv.push_back(new LFGraph_Panel_EnergyCharge("ECHARGE"));

	//-- Price Charges
	if (TCSettings::get_bool("LFGUI:Panel:PCHARGE:on"))
		_gdv.push_back(new LFGraph_Panel_PriceCharge("PCHARGE"));

	//-- Volume / Transactions
	if (TCSettings::get_bool("LFGUI:Panel:VOLUME:on"))
		_gdv.push_back(new LFGraph_Panel_Volume("VOLUME"));
	//-- Density / Frequency
	if (TCSettings::get_bool("LFGUI:Panel:DENSITY:on"))
		_gdv.push_back(new LFGraph_Panel_Density("DENSITY"));

	//-- Scale
	if (TCSettings::get_bool("LFGUI:Panel:SCOPE:on"))
		_gdv.push_back(new LFGraph_Panel_Scope("SCOPE"));

	//-- P&L
	if(TCSettings::get_bool("LFGUI:Panel:PNL:on"))
		_gdv.push_back(new LFGraph_Panel_Position("POSITION"));

}
int LFGraph_StrategyData::subsets()
{
	int ret = 0;
	for(size_t i = 0; i < _gdv.size(); ++i)
		ret += _gdv[i]->subsets();
	return ret;
}

void LFGraph_StrategyData::subsets_by_axes(std::vector<int>& v_)
{
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->subsets_by_axes(v_);
}
void LFGraph_StrategyData::overlap_multi_axis(std::vector<int>& v_)
{
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->overlap_multi_axis(v_);
}
void LFGraph_StrategyData::reset()
{
	TC_GraphData::reset();
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->reset();
}
void LFGraph_StrategyData::load_graph()
{
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->load_graph();
}
void LFGraph_StrategyData::reset_graph()
{
	TC_GraphData::reset_graph();
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->reset_graph();
}
void LFGraph_StrategyData::draw_graph()
{
	for(size_t i = 0; i < _gdv.size(); ++i)
		_gdv[i]->draw_graph();
}
void LFGraph_StrategyData::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);
	int s = first_subset_;
	int a = axis_;
	for(size_t i = 0; i < _gdv.size(); ++i)
	{
		for(int k = 0; k < _gdv[i]->axes(); ++k)
			g_->vec_axis(_gdv[i]->name());
		_gdv[i]->init(g_, s, a);
		s += _gdv[i]->subsets();
		a += _gdv[i]->axes();
	}
}

void LFGraph_StrategyData::process(LFStrategyDataPtr* p_)
{
	auto_ptr<LFStrategyDataPtr> p(p_);
	LFStrategyData& d = **p;

	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());

	for(size_t i=0; i < _gdv.size(); ++i)
		_gdv[i]->process(d);
	theApp.mainframe()->get_graph()->engage_main_thread();
}
void LFGraph_StrategyData::process_pos(LFPositionDataPtr* p_)
{
	auto_ptr<LFPositionDataPtr> p(p_);
	LFPositionData& d = **p;

	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());

	for(size_t i=0; i < _gdv.size(); ++i)
		_gdv[i]->process_pos(d);
	theApp.mainframe()->get_graph()->engage_main_thread();
}
void LFGraph_StrategyData::process_fills(LFOrderFillPtr* p_)
{
	auto_ptr<LFOrderFillPtr> p(p_);

	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	LFOrderFill& d = **p;
	for(size_t i=0; i < _gdv.size(); ++i)
		_gdv[i]->process_fills(d);
	theApp.mainframe()->get_graph()->engage_main_thread();
}
ScopeTables::ScopeTables()
	:_scope_tables(Z::get_setting("LFGUI:scope_tables"))
{
	for (size_t i = 0; i < _scope_tables.size(); ++i)
	{
		if (_scope_tables[i] < 0)
			_scope_tables[i] = 0;
		if (_scope_tables[i] > 5)
			_scope_tables[i] = 5; 
	}
	std::sort(_scope_tables.begin(), _scope_tables.end());
	std::vector<long>::iterator it = std::unique(_scope_tables.begin(), _scope_tables.end());
	_scope_tables.resize(std::distance(_scope_tables.begin(), it));
}

//======================================================================
// LFGraph_Panel_Position
//======================================================================
LFGraph_Panel_Position::LFGraph_Panel_Position(const std::string& name_)
: LFGraph_Panel(name_),
_table_rows(0),
_table_cols(0),
_w_id(0), _p_id(0), _s_id(0),
_level_annot(0,0),
_level_g_annot(0,0),
_show_lcy(false),
_hloc_table_inx(-1)
{
	load_resource();
	_gd_v.push_back(new TC_Graph_Last("PnL", TC_GraphData::ysSymmetrical));
	_gd_v.push_back(new TC_Graph_Last("R PnL", TC_GraphData::ysSymmetrical));
}
void LFGraph_Panel_Position::reset()
{
	_p_map.clear();
	_fills.clear();
	_fbin.clear();
	load_resource();
	LFGraph_Panel::reset();
}
void LFGraph_Panel_Position::reset_graph()
{
	_fbin.clear();
	LFGraph_Panel::reset_graph();
}
void LFGraph_Panel_Position::load_resource()
{
	_show_lcy = Z::get_setting_bool("LFModel:LFGUI:show_lcy", false);
}

void LFGraph_Panel_Position::init_scope_annotations(const std::string& actor_)
{
	TC_AnnotationData pa;

	// levels
	//-----------------------------------------------------------------------------
	if (Z::get_setting_bool("LFGUI:Panel:ENERGY:1:ann_group:on", false))
	{
		pa._pa_group_name.clear();
		pa._ann_type = PELT_THINSOLID;
		pa._pa_type = PEP_faHORZLINEANNOTATION;
		pa._ann_color = TC_RGB::Green;
		pa._ann_axis = _g->get_axis("HLOC:follower");
		pa.set_group("ENERGY:1");
		pa._pa_group_size = 8;
		_g->add_annotation(pa); // Horizontal up/down level annotation fixed group
		_level_annot_id = pa._pa_group_name;

		// Graph Annotations for levels
		pa._pa_group_name.clear();
		pa._ann_type = PEGAT_POINTER;
		pa._pa_type = PEP_faGRAPHANNOTATIONX;
		pa._pa_group_size = 8;
		_g->add_annotation(pa); // Graph up/down levels annotations fixed group
		_g->map_annot(pa._pa_group_name, "HLOC");
		_level_g_annot_id = pa._pa_group_name;
	}

	pa._active = true;

	for(size_t i = 0; i < LFScopeStore::NUMBER_OF_SCOPES; ++i)
	{
		pa._ann_text_size = 85;
		std::ostringstream aos; aos << "LFGUI:Panel:HLOC:expected_annot:"
									<< actor_ << ":" << i;
		std::string scope_id = ScopeTables::expectation_id(actor_, i);

		// Vertical annotation
		pa._ann_type = Z::get_setting_integer(aos.str(), 2);
		pa._ann_axis = _g->get_axis("HLOC:"+actor_);
		pa._pa_type = PEP_faVERTLINEANNOTATION;
		pa._pa_group_size = 6;
		pa._pa_group_name = scope_id;
		pa.set_group(scope_id);
		_g->add_annotation(pa); // Vertical Roll annotation fixed group
		_g->map_annot(pa._pa_group_name, scope_id);

		// Graph Annotation
		std::ostringstream os; os << "PCHARGE:" << actor_ << ":" << i;
		pa._ann_type = PEGAT_POINTER;
		pa._ann_axis = _g->get_axis(os.str()); // AMTD
		pa._pa_type = PEP_faGRAPHANNOTATIONX;
		pa._pa_group_size = 6;
		pa._pa_group_name = scope_id;

		_g->add_annotation(pa); // Graph up/down levels annotations fixed group
		_g->map_annot(pa._pa_group_name, scope_id);
	}
}


void LFGraph_Panel_Position::subsets_by_axes(std::vector<int>& v_)
{
	v_.push_back(2);
}
void LFGraph_Panel_Position::init(TC_Graph* g_, int first_subset_, int axis_)
{
	TC_GraphData::init(g_, first_subset_, axis_);

	int subset_lines = first_subset_;
	int axis = axis_;

	int nSLT = PELT_THINSOLID;

	for (size_t i = 0; i < _gd_v.size(); ++i)
	{
		_gd_v[i]->init(_g, subset_lines++, axis);
		PEvsetcell(m_hPE, PEP_naSUBSETLINETYPES, _gd_v[i]->subset(), &nSLT);
	}

	PEnset(m_hPE, PEP_nWORKINGAXIS, axis);
	PEnset(m_hPE, PEP_nPLOTTINGMETHOD, PEGPM_LINE);
	PEnset(m_hPE, PEP_nSHOWYAXIS, PESA_GRIDNUMBERS);

	DWORD col = ZARGB(Z::get_setting("LFModel:leader_open", "Blue"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, _gd_v[0]->subset(), &col);
	col = ZARGB(Z::get_setting("LFModel:follower_close", "Red"));
	PEvsetcell(m_hPE, PEP_dwaSUBSETCOLORS, _gd_v[1]->subset(), &col);

	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, _gd_v[0]->subset(), CA2T("PnL"));
	_g->map_subset("P&L", _gd_v[0]->subset());
	PEvsetcell(m_hPE, PEP_szaSUBSETLABELS, _gd_v[1]->subset(), CA2T("R PnL"));
	_g->map_subset("P&L", _gd_v[1]->subset());
	_g->add_to_group("Trading", this);

	init_tables(FOLLOWER_STR);
	init_scope_annotations(FOLLOWER_STR);
	if (TCSettings::get_resource("LFGUI:Panel:HLOC:leader:expected:on").get_bool())
		init_scope_annotations(LEADER_STR);
}
void LFGraph_Panel_Position::init_tables(const std::string& actor_)
{
	_table_v.push_back(PCol(5, "Id"));
	_table_v.push_back(PCol(3, "V"));
	_table_v.push_back(PCol(3, " "));
	_table_v.push_back(PCol(6, "Vol"));
	_table_v.push_back(PCol(6, "CVol"));
	_table_v.push_back(PCol(5, "Tran"));
	_table_v.push_back(PCol(3, "Den"));
	_table_v.push_back(PCol(4, "Freq"));
	_table_v.push_back(PCol(3, "Slope"));
	_table_v.push_back(PCol(3, " "));
	_table_v.push_back(PCol(3, "L"));
	_table_v.push_back(PCol(3, " "));
	_table_v.push_back(PCol(6, "Pat"));
	//_table_v.push_back(PCol(4, "E_v"));
	//_table_v.push_back(PCol(4, "E_inv"));
	//_table_v.push_back(PCol(4, "E_w"));
	_table_v.push_back(PCol(4, "Pias"));
	_table_v.push_back(PCol(4, "P"));
	_table_v.push_back(PCol(6, "Ident"));


	_table_rows = 6;
	_table_cols = _table_v.size();

	for (int i = 0; i < _st._scope_tables.size(); ++i)
	{
		//int tb_ind = MANAGING_TABLE_INDEX + _st._scope_tables[i] + 1;
		std::ostringstream os; os << "SCOPE_DETAILS_" << i;
		std::string tb_s = os.str();
		
		_scope_table_inx.push_back(_g->make_table(ScopeTables::scope_id(actor_, _st._scope_tables[i]), (int)(_table_rows + 1), (int)_table_cols));
		
		PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, FALSE);
//		PEnset(m_hPE, PEP_dwTABACKCOLOR, 1);
//		PEnset(m_hPE, PEP_nTABORDER, PETAB_DROP_SHADOW);

		// Set Column Widths //

		int nJust = PETAJ_LEFT;

		for (size_t i = 0; i < _table_cols; ++i)
		{
			PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, (int)i, &_table_v[i].first);
			PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, (int)i, &nJust);
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, (int)i, CA2T(_table_v[i].second));
		}
	}
}

void LFGraph_Panel_Position::process(LFStrategyData& d_)
{
	if(_p_map.empty())
		return;
	LFPositionDataPtr& pp = (*_p_map.rbegin()).second;
	pp->mark_to_market(d_._f_1._md);
	pp->strat_to_market(d_._f_1._md);
	process_last_pos(pp);
}
void LFGraph_Panel_Position::process_last_pos(LFPositionDataPtr pp_)
{
	_gd_v[0]->display_point(pp_->get_t(), pp_->_pnl);
	_gd_v[1]->display_point(pp_->get_t(), pp_->_realized_pnl);

}
void LFGraph_Panel_Position::process_pos(LFPositionData& d)
{
	LFPositionDataPtr pp(new LFPositionData(d));
	_p_map[d.get_t()] = pp;

	process_last_pos(pp);
	add_level_annot(*pp);
}
void LFGraph_Panel_Position::process_fills(LFOrderFill& p_)
{
	if(_p_map.empty())
		return;
	double pnl = (*(*_p_map.rbegin()).second)._pnl;
	_fills.push_back(std::pair<LFOrderFill, double>(p_, pnl));
	display_fill_annot(p_, pnl);
}
void LFGraph_Panel_Position::display_fill_annot(LFOrderFill& d, double pnl_)
{
	_fbin[_g->dt().index(_g->first_t(), _g->dt().bar_time(d.get_t()))].push_back(d);

	double bar_t_vb = TC_Graph::get_vb_time(d.get_t());

	if(_p_map.size() == 0)
		return;


	if(TCSettings::get_resource("LFGUI:Panel:PNL:ann_group:on").get_bool())
	{
	// GRAPH P&L annotation
	//-----------------------------------------------------------------------------

		TC_AnnotationData pa(PEGAT_POINTER, PEP_faGRAPHANNOTATIONX);

		pa.set_group("PNL");
		pa._X = bar_t_vb;
		pa._Y = pnl_;
		pa._ann_axis = axis();
		pa._ann_text_size = 85;

		ACE_OS::memset(pa._label, 0, sizeof(pa._label));
		std::string format = " %s %.0f@" + LFSecurityMaster::instance()->get_fprecision();
		ACE_OS::sprintf(pa._label, format.c_str(), (d._side == LF::s_BUY)? "B": "S", d._fill._q, d._fill.p_p());

		if(d._side == LF::s_BUY)
		{
			pa._ann_color = TC_RGB::Green; // s_BUY
			if(d._sub_strategy_id != 2)
				pa._ann_color = TC_RGB::DarkGreen; // s_BUY
		}
		else
		{
			pa._ann_color = TC_RGB::Red; // s_SELL
			if(d._sub_strategy_id != 2)
				pa._ann_color = TC_RGB::DarkRed; // s_SELL
		}

		_g->add_annotation(pa);	// Graph P&L annotation
		_g->map_annot(pa._pa_group_name, "P&L");
		// Vertical P&L annotation
		//-----------------------------------------------------------------------------
		pa._pa_group_name.clear();
		pa._pa_type = PEP_faVERTLINEANNOTATION;
		pa._ann_type = PELT_EXTRAEXTRATHINSOLID;
		if(d._sub_strategy_id < 0)
			pa._ann_type = PELT_DOT;

		ACE_OS::memset(pa._label, 0, sizeof(pa._label));
		_g->add_annotation(pa);	// vertical P&L
		_g->map_annot(pa._pa_group_name, "P&L");
	}
}

void LFGraph_Panel_Position::display_table(int index_)
{
	if (_hloc_table_inx == -1)
		_hloc_table_inx = _g->get_table_info("HLOC")._num;
	display_fill_table(index_);
	display_pos_table(index_);
//	PEdrawtable(m_hPE, _hloc_table_inx, NULL);
}
void LFGraph_Panel_Position::display_fill_table(int index_)
{
	char buf[128];

	FillList* fl = 0;
	if(_fbin.size() > 0)
	{
		FillsByIndex::iterator ii = _fbin.lower_bound(index_+1);
		if(ii == _fbin.end())
			fl = &(*_fbin.rbegin()).second;
		else if(ii != _fbin.begin())
			fl = &(*(--ii)).second;
	}
	int ri = rows() - fill_rows();
	if(fl != 0)
	{
		for(FillList::iterator i = fl->begin(); i != fl->end(); ++i)
		{
			if(ri < _rows)
			{
				LFOrderFill& f = (*i);
				ACE_OS::memset(buf, 0, sizeof(buf));
				std::string format = " %s %.0f@" + LFSecurityMaster::instance()->get_fprecision();
				ACE_OS::sprintf(buf, format.c_str(), (f._side == LF::s_BUY)? "B": "S", f._fill._q, f._fill.p_p());
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, _row + ri, 0, CA2T(buf));
				ri++;
			}
			else
			{
				std::string emp("...");
				PEvsetcellEx(m_hPE, PEP_szaTATEXT, _row + _rows - 1, 0, CA2T(emp.c_str()));
			}
		}
	}

	std::string emp(" ");
	while(ri < _rows)
	{
		PEvsetcellEx(m_hPE, PEP_szaTATEXT, _row + ri, 0, CA2T(emp.c_str()));
		ri++;
	}
}
void LFGraph_Panel_Position::display_pos_table(int index_)
{
	ACE_Time_Value tv_bound = _g->dt().by_index(_g->first_t(), index_ + 1);
	LFPositionDataPtr pp;
	if(_p_map.size() > 0)
	{
		PositionMap::iterator ii = _p_map.lower_bound(tv_bound);
		if(ii == _p_map.end())
			pp = (*_p_map.rbegin()).second;
		else if(ii != _p_map.begin())
			pp = (*(--ii)).second;
		else
			pp = (*ii).second;

		display_position(pp, index_);
	}
	else
	{
		std::string emp = " ";
		for(int i = 1; i < _rows - fill_rows(); ++i)
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, _row + i, 0, CA2T(emp.c_str()));

	}
//	PEdrawtable(m_hPE, _hloc_table_inx, NULL);
}
void LFGraph_Panel_Position::display_click(int index_)
{
	if(_p_map.empty())
		return;

	ACE_Time_Value tv_bound = _g->dt().by_index(_g->first_t(), index_ + 1);
	LFPositionDataPtr pp;

	PositionMap::iterator ii = _p_map.lower_bound(tv_bound);
	if(ii == _p_map.end())
		pp = (*_p_map.rbegin()).second;
	else if(ii != _p_map.begin())
		pp = (*(--ii)).second;
	else
		pp = (*ii).second;

	display_scope_table(*pp);
	process_trend_annot(*pp);
	add_level_annot(*pp);
}
void LFGraph_Panel_Position::process_trend_annot(LFPositionData& d_)
{
	for(size_t k = 0; k < LFScopeStore::NUMBER_OF_SCOPES; ++k)
	{
		const LFBoxTrend& rl = d_._box_trends[k];
		const LFBoxTrend& lcy = d_._lcy_trends[k];
		std::vector<std::string> v_;
		std::vector<std::string> vs_;
		std::vector<std::string> vS_;

		_g->annot_by_group(v_, ScopeTables::expectation_id(FOLLOWER_STR, k));
		_g->annot_by_group(vS_, ScopeTables::volume_g_id(FOLLOWER_STR, k));
		_g->annot_by_group(vs_, ScopeTables::volume_t_id(FOLLOWER_STR, k));

		if(v_.empty())
			continue;
		size_t a_size = _g->fixed_set_size(v_[0]);
		for (size_t i = 0; i < rl.size() && i < a_size; ++i)
		{
			add_scope_annot(rl[i], (int)i, v_);
			if (_show_lcy)
			{
				add_scope_annot_s(lcy[i], (int)i, vs_, vS_);
				if (i > 0 && !lcy[i].valid())
					end_scope_annot_s(lcy[i - 1], (int)i, vs_, vS_);
			}
			else
			{
				add_scope_annot_s(rl[i], (int)i, vs_, vS_);
				if (i > 0 && !rl[i].valid())
					end_scope_annot_s(rl[i - 1], (int)i, vs_, vS_);
			}

		}
	}
}

void LFGraph_Panel_Position::display_scope_table(const LFPositionData& d_)
{
	for (size_t k = 0; k < _scope_table_inx.size(); ++k)
	{
		PEnset(m_hPE, PEP_nWORKINGTABLE, _scope_table_inx[k]);

		const LFBoxTrend& rl = d_._box_trends[_st._scope_tables[k]];
		for (size_t i = 0; i < _table_rows; ++i)
		{
			if (i < rl.size() && rl[i]._w.valid())
			{
				display_scope_item(rl, (int)i);
			}
			else
			{
				for (size_t ii = 0; ii < _table_cols; ++ii)
				{
					char buf[128];
					ACE_OS::memset(buf, 0, sizeof(buf));
					ACE_OS::sprintf(buf, " ");
					PEvsetcellEx(m_hPE, PEP_szaTATEXT, (int)(i + 1), (int)(ii), CA2T(buf));
				}
			}
//			PEdrawtable(m_hPE, _scope_table_inx[k], NULL);
		}
	}
}
void LFGraph_Panel_Position::add_scope_annot(const LFBox& b_, int r,
											 const std::vector<std::string>& v_)
{
	TC_AnnotationData& pa = _g->get_annotation(v_, PEP_faVERTLINEANNOTATION, r);
	TC_AnnotationData& ga = _g->get_annotation(v_, PEP_faGRAPHANNOTATIONY, r);

	pa._active = b_.valid();
	ga._active = pa._active;

	if(b_.valid())
	{
		std::string format = "%d%c%d %.0f(%.0f) @" + LFSecurityMaster::instance()->get_fprecision() +
			" [%.0f,%.0f] %.0f/%.1f(%.0f/%.1f) %s %c [%s]";
			 
		ACE_OS::sprintf(ga._label, format.c_str(),
			b_.id()._scale, b_.ended() ? ' ' : '_',
			r, b_._pias, b_._d_pias, b_._w._dp->_p,
			b_._w._cluster_vol, b_._w._cluster_tran, b_._w._cluster_v, b_._w._cluster_s,
			b_.tip_v(), b_.tip_s(), b_._comment.c_str(), b_._pinned ? 'P' : ' ', b_._w._pattern.to_alfa().c_str());

		if(b_._side->value() == LF::s_SELL)
			pa._ann_color = TC_RGB::Green;
		else
			pa._ann_color = TC_RGB::Red;

		pa._X = TC_Graph::get_vb_time(b_._max_dp->_t);
		pa._Y		= b_._max_dp->_p;
		ga._ann_color = pa._ann_color;
		ga._X = pa._X;
		ga._Y = b_._pias;
	}
}
void LFGraph_Panel_Position::add_scope_annot_s(const LFBox& b_, int r,
	const std::vector<std::string>& vs_, 
	const std::vector<std::string>& vS_)
{
	TC_AnnotationData& gal = _g->get_annotation(vS_, PEP_faGRAPHANNOTATIONY, r);
	TC_AnnotationData& gat = _g->get_annotation(vs_, PEP_faGRAPHANNOTATIONY, r);


	gal._active = b_.valid();
	gat._active = b_.valid();

	if (!b_.valid())
		return;

	ACE_OS::sprintf(gat._label, "%s%d_%d_%s_%.0f_%.0f_%.1f_%.0f", b_._comment.c_str(), b_.id()._scale, b_.id()._roll, b_._w._pattern.to_alfa().c_str(),
		b_._w._cluster_v, b_._w._cluster_vol, b_._w._cluster_s, b_._w._dp->_vma);

	gal._ann_type = (r == 0) ? PEGAT_THINSOLIDLINE : PEGAT_LINECONTINUE;

	if (b_._side->value() == LF::s_SELL)
		gat._ann_color = TC_RGB::Green;
	else
		gat._ann_color = TC_RGB::Red;

	gat._X = TC_Graph::get_vb_time(b_._max_dp->_t);
	gat._Y = b_._max_dp->_p;

	gal._X = gat._X;
	gal._Y = gat._Y;
}

void LFGraph_Panel_Position::end_scope_annot_s(const LFBox& b_, int r,
	const std::vector<std::string>& vs_,
	const std::vector<std::string>& vS_)
{
	TC_AnnotationData& gal = _g->get_annotation(vS_, PEP_faGRAPHANNOTATIONY, r);
	TC_AnnotationData& gat = _g->get_annotation(vs_, PEP_faGRAPHANNOTATIONY, r);


	gal._active = b_.valid();	
	gat._active = false;

	if (!b_.valid() && r > 0)
	{
		TC_AnnotationData& prevl = _g->get_annotation(vS_, PEP_faGRAPHANNOTATIONY, r-1);
		if (prevl._active && prevl._ann_type == PEGAT_LINECONTINUE)
		{
			gal._active = true;
			gal._ann_type = PEGAT_THINSOLIDLINE;

			gal._X = prevl._X;
			gal._Y = prevl._Y;
		}
		return;
	}

//	ACE_OS::sprintf(gat._label, "%d_%d_%s", b_.id()._scale, b_.id()._roll, b_._w._pattern.to_alfa().c_str());

	gal._ann_type = PEGAT_LINECONTINUE;
//	gal._ann_type = PEGAT_THINSOLIDLINE;

	gal._X = TC_Graph::get_vb_time(b_._w._cluster_dp->_t);
	gal._Y = b_._w._cluster_dp->_p;

}

void LFGraph_Panel_Position::add_level_annot(LFPositionData& d_)
{
	add_level_annot_box(d_._bias_bc, 2, "bc", true);
	add_level_annot_box(d_._bias_bx, 6, "bx", false);
}

void LFGraph_Panel_Position::add_level_annot_box(const LFBox& b_,
												 size_t level_,
												 const std::string& comment_, 
												 bool show_end_)
{
//	_level_annot = LF::RefPair<TC_AnnotationData>(&_g->get_annotation(_level_annot_id, level_), &_g->get_annotation(_level_annot_id, level_+1));
	_level_g_annot = LF::RefPair<TC_AnnotationData>(&_g->get_annotation(_level_g_annot_id, level_), &_g->get_annotation(_level_g_annot_id, level_+1));

	_level_g_annot[LF::s_SELL]._active = false;
	_level_g_annot[LF::s_BUY]._active = false;

	const LFCluster& sl = b_._w;

	if (sl.cluster_valid())
	{
		TC_AnnotationData& ga = _level_g_annot[sl._side->value()];
		ga._active = true;
		ga._ann_color = TC_RGB::Green;
		if (sl._side->value() == LF::s_BUY)
			ga._ann_color = TC_RGB::Red;
		ga._X = TC_Graph::get_vb_time(sl._dp->_t);
		ga._Y = sl._dp->_p;
		std::string format = "%d_%d %s%s @" + LFSecurityMaster::instance()->get_fprecision() + " %.0f/%.1f";
		ACE_OS::sprintf(ga._label, format.c_str(),
			b_._w._id._scale, b_._w._id._roll, comment_.c_str(), b_._comment.c_str(), ga._Y, 
			sl._cluster_v, sl._cluster_s);

		if (show_end_)
		{
			TC_AnnotationData& gac = _level_g_annot[sl._side->c_value()];
			gac._active = true;
			gac._ann_color = ga._ann_color;
			gac._X = TC_Graph::get_vb_time(sl._cluster_dp->_t);
			gac._Y = sl._cluster_dp->_p;
			std::string format ="%s @" + LFSecurityMaster::instance()->get_fprecision();
			ACE_OS::sprintf(gac._label, format.c_str(), comment_.c_str(), gac._Y);
		}
		
	}
}
void LFGraph_Panel_Position::display_position(const LFPositionDataPtr& pp, int index_)
{
	char buf[128];
	int row = _row;

	float pnl = 0;
	float rpnl = 0;
	PEvgetcellEx(m_hPE, PEP_faYDATA, _subset, index_, &pnl); //pnl
	PEvgetcellEx(m_hPE, PEP_faYDATA, _subset+1, index_, &rpnl); //rpnl

	//----------------------------------
	PEnset(m_hPE, PEP_nWORKINGTABLE, _hloc_table_inx);
	//----------------------------------
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "   %s", P_FORMAT::position_status()(pp->_status).c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "End: %s", pp->_signal.c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "Act: %s", P_FORMAT::position_action()(pp->_action).c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	/*
	ACE_OS::memset(buf, 0, sizeof(buf));
//	ACE_OS::sprintf(buf, "Average: %.0f", pp->_q_A);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
//	ACE_OS::sprintf(buf, "Profit:  %.0f", pp->_q_P);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
//	ACE_OS::sprintf(buf, "Scratch: %.0f", pp->_q_S);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	*/
	ACE_OS::sprintf(buf, " PnL : %.0f", pnl);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::sprintf(buf, " RPnL: %.0f", rpnl);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, " Pos : %s %.0f", pp->_side==LF::s_BUY?"L ":(pp->_side==LF::s_SELL?"S ":"  "), pp->_open._q);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;
	ACE_OS::memset(buf, 0, sizeof(buf));
	std::string format = " AvgP: " + LFSecurityMaster::instance()->get_fprecision();
	ACE_OS::sprintf(buf, format.c_str(), pp->_open.p_p());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, row, 0, CA2T(buf));
	row++;

}

void LFGraph_Panel_Position::display_scope_item(const LFBoxTrend& td_, int i_)
{
/*
_table_v.push_back(PCol(5, "Id"));
_table_v.push_back(PCol(3, "V"));
_table_v.push_back(PCol(3, " "));
_table_v.push_back(PCol(6, "Vol"));
_table_v.push_back(PCol(6, "CVol"));
_table_v.push_back(PCol(5, "Tran"));
_table_v.push_back(PCol(3, "Den"));
_table_v.push_back(PCol(4, "Freq"));
_table_v.push_back(PCol(3, "Slope"));
_table_v.push_back(PCol(3, " "));
_table_v.push_back(PCol(3, "L"));
_table_v.push_back(PCol(3, " "));
_table_v.push_back(PCol(6, "Pat"));
_table_v.push_back(PCol(4, "Pias"));
_table_v.push_back(PCol(4, "P"));
_table_v.push_back(PCol(6, "Ident"));
*/
	char buf[128];
	int c = 0;
	int r = i_ + 1;
	const LFBox& b = td_[i_];

	//-----------------------------------------------------
	//	 Id
	//-----------------------------------------------------

	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%d %s", i_, P_FORMAT::slope_side()(b._side->c_value()).c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// V
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_v);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// 
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", !b._tip.valid() ? 0 : b._tip._cluster_v);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Vol
	ACE_OS::memset(buf, 0, sizeof(buf));
	//	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_vol);
	ACE_OS::sprintf(buf, "%.0f", LF::volume(b._w._side, b._w._cluster_dp, b._w._dp));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// CVol
	ACE_OS::memset(buf, 0, sizeof(buf));
	//	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_vol);
	ACE_OS::sprintf(buf, "%.0f", LF::volume(b._w._side->contra_s(), b._w._cluster_dp, b._w._dp));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Tran
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_tran);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Den
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_density);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Freq
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", b._w._cluster_frequency);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// Slope
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.1f", b._w._cluster_s);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// 
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.1f", !b._tip.valid() ? 0 : b._tip._cluster_s);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// L
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.1f", b._w._cluster_l);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// 
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.1f", !b._tip.valid() ? 0 : b._tip._cluster_l);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Pat
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%s", b._w._pattern.to_alfa().c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	//// E_v
	//ACE_OS::memset(buf, 0, sizeof(buf));
	//ACE_OS::sprintf(buf, "%.0f", b._e_v);
	//PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	//// E_inv
	//ACE_OS::memset(buf, 0, sizeof(buf));
	//ACE_OS::sprintf(buf, "%.0f", b._e_in_v);
	//PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	//// E_w
	//ACE_OS::memset(buf, 0, sizeof(buf));
	//ACE_OS::sprintf(buf, "%.0f", b._e_w);
	//PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

	// Pias
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%.0f", b._pias);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// P
	ACE_OS::memset(buf, 0, sizeof(buf));
	
	ACE_OS::sprintf(buf, LFSecurityMaster::instance()->get_fprecision().c_str(), b._w._dp->_p);
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));
	// Ident
	ACE_OS::memset(buf, 0, sizeof(buf));
	ACE_OS::sprintf(buf, "%s", b.id().to_alfa().c_str());
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, r, c++, CA2T(buf));

}


//======================================================================
// Cmd_graphView
//======================================================================
//-------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(Cmd_graphView, ETSLayoutFormView)

BEGIN_MESSAGE_MAP(Cmd_graphView, ETSLayoutFormView)
	//{{AFX_MSG_MAP(ETSLayoutFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

Cmd_graphView::Cmd_graphView(UINT nID)
	: ETSLayoutFormView(nID),
	m_hPE(0),
	_g(new TC_Graph),
	_visible_config(0),
	_visible(0),
	_subsets(0),
	_exclude_group(Z::get_setting("LFGUI:2:EXCLUDE_GROUP")),
	_exclude_subset(Z::get_setting("LFGUI:2:EXCLUDE_SUBSET")),
	_panel_table_index(-1),
	_scope_table_index(-1),
	_slope_points_vec(3),
	_slope_points_ind(0),
	_inputs_inactive(0)
{
	std::sort(_exclude_group.begin(), _exclude_group.end());
	std::sort(_exclude_subset.begin(), _exclude_subset.end());

	set_default_graph_resources();

	theApp.mainframe()->set_graph(this);
	_s.reset(new BMStrategy(theApp.star(), 0));

	if(!Z::get_setting_bool("LFGUI:thread", true))
		_s_data.reset(new BMStrategy(theApp.star(), Z::make_callback(this, &Cmd_graphView::engage_main_thread)));
	else
		_s_data.reset(new BMStrategy(0));//Z::make_callback(this, &Cmd_graphView::engage_main_thread)));

	_s->set_name("Graph_Timer");
	_s_data->set_name("Graph_Data");
	_s_callback.reset(new Z::ActiveCallback(Z::make_callback(this, &Cmd_graphView::process_run), theApp.star()));

	long hwm = Z::get_setting_integer("LFGUI:hwm", 1000);
	long lwm = Z::get_setting_integer("LFGUI:lwm", 999);

	std::string strategy_id = Z::get_setting("LFStrategy:strategy_id");
	std::string strategy_key_l = LF::make_strategy_key(strategy_id, theApp.leader());
	std::string strategy_key_f = LF::make_strategy_key(strategy_id, theApp.follower());

	_g->add_source(new LFGraph_StrategyData(*_s_data, strategy_id, "strat", false, hwm, lwm));
	if(Z::get_setting_bool("LFGUI:Panel:LVOLUME:on", false))
		_g->add_source(new LFGraph_TradeData_Leader(*_s_data, theApp.leader(), "l_trade", true, hwm, lwm));
	if(Z::get_setting_bool("LFGUI:Panel:FVOLUME:on", false))
		_g->add_source(new LFGraph_TradeData_Follower(*_s_data, theApp.follower(), "f_trade", true, hwm, lwm));

	//_s->subscribe_change_run(Z::make_callback(this, &Cmd_graphView::change_run));
	_s_data->subscribe_change_run(Z::make_callback(this, &Cmd_graphView::change_run));
	_s_data->subscribe_inputs_inactive(Z::make_callback(this, &Cmd_graphView::process_inputs_inactive));

	_min_zoom = Z::get_setting_double("LFGUI:Panel:HLOC:min_zoom", .40);
	_pct_zoom = Z::get_setting_double("LFGUI:Panel:HLOC:pct_zoom", 0);
	_cursor_y.first = -1;
	_cursor_y.second = 0;
}

Cmd_graphView::~Cmd_graphView()
{
	Z::report_debug("---exiting Cmd_graphView");
	_g->fini();
	tc_tao->tc_exit();
	if(_s_callback.get() != 0)
		_s_callback->close(true);
	_s->close();
	_s->unsubscribe_change_run();
	_s_data->close();
//	_s_data->unsubscribe_change_run();
//	_g->fini();
	_g.reset();

	if( m_hPE )
		PEdestroy(m_hPE);
	Z::report_debug("---exited Cmd_graphView");
	tc_tao->tc_exit();
}
bool Cmd_graphView::init_visible_group(const std::string& gid_)
{
	SubsetVec& v = _g->get_group_subsets(gid_);
	bool inactive_group = true;
	
	for(size_t k = 0; k < v.size(); ++k)
	{
		if(v[k]._visible)
		{
			inactive_group = false;
			break;
		}
	}
	int axis = _g->get_axis(gid_);
	if(std::binary_search(_exclude_group.begin(), _exclude_group.end(), gid_))
	{
		for (size_t i = 0; i < v.size(); ++i)
		{
			v[i]._visible = false;
			_visible[v[i]._num] = 0;
			_g->set_visible_axis(axis, v[i]._num, false);
		}
		inactive_group = true;
	}
	_g->group_map()[gid_]._visible = !inactive_group;
	_g->manage_annot_menu(gid_, !inactive_group);
	

	return inactive_group;
}

void Cmd_graphView::set_visual_subsets(const std::string& gid_, int show_, bool sub_group_)
{// change visibility of the group
	SubsetVec& gv = _g->get_group_subsets(gid_, sub_group_);

	for(size_t i = 0; i < gv.size(); ++i)
	{
		if(show_ == 0)
			gv[i]._visible = false;
		else
			gv[i]._visible = _visible_config[gv[i]._num];

		int subset = gv[i]._num;
		_visible[subset] = (int)gv[i]._visible;
		
		int axis = _g->get_axis(gid_);
		_g->set_visible_axis(axis, subset, gv[i]._visible);
	}
	_g->manage_annot_menu(gid_, show_);
	PEvset(m_hPE, PEP_naSUBSETSTOSHOW, _visible.get(), (int)_subsets);
}
void Cmd_graphView::init_visible_subsets()
{
	TCHAR szTmp[512];
//-----------------------------------------
// construct visibility array for subsets

	_visible_config.reset(new int[_subsets]);
	_visible.reset(new int[_subsets]);

	for(size_t l = 0; l < _subsets; l++)
	{
		PEvgetcell(m_hPE, PEP_szaSUBSETLABELS, (int)l, szTmp);
		bool vis = !(std::binary_search(_exclude_subset.begin(), _exclude_subset.end(), std::string(CT2A(szTmp))));
		_visible_config[(int)l] = vis;
		_visible[(int)l] = vis;
	}
}
void Cmd_graphView::set_visible_table()
{
	if(_visible.get() != 0)
		return;

	init_visible_subsets();

//-----------------------------------------
// Construct floating visibility table

	make_panel_table();
	make_scope_table();
	
	PEvset(m_hPE, PEP_naSUBSETSTOSHOW, _visible.get(), (int)_subsets);

	PEnset(m_hPE, PEP_nTAX, 50 ); // initial setting, we set again further down in code.
	PEnset(m_hPE, PEP_nTAY, 50 );

}
void Cmd_graphView::make_panel_table()
{
	
	const GraphSubsetGroupsMap& gmap = _g->group_map();
	const std::vector<std::string>& gvec = _g->group_vec();

	int rows = 0;
	for (GraphSubsetGroupsMap::const_iterator it = gmap.begin(); it != gmap.end(); ++it)
	{
		if (!it->second._super_group.empty())
			continue;
		rows++;
	}
	_panel_table_index = _g->make_table("PANELS", rows + 1, 2); // extra header row

	DWORD dwCellColor = PERGB(255, 255, 0, 0);

	int nTypeT = PEGAT_LARGESQUARESOLID;
	int nTypeF = PEGAT_LARGESQUARE;
	int nHotSpot = 1;

	// Pass the table text //
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, CA2T("Panel"));
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 1, CA2T("Visible"));

	int l = 0;

	for (std::vector<std::string>::const_iterator it = gvec.begin(); it != gvec.end(); ++it)
	{
		const std::string& group = *it;
		std::string subgrp;
		bool is_subgroup = _g->get_subgroup(group, subgrp);
		bool super_visible = init_visible_group(group);
		//_g->set_visible_axis(axis, l, vis);
		if (!is_subgroup)
		{
			PEvsetcellEx(m_hPE, PEP_szaTATEXT, l + 1, 0, CA2T(group.c_str()));
			PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, l + 1, 1, &dwCellColor);
			PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, l + 1, 1, &nHotSpot);
			PEvsetcellEx(m_hPE, PEP_naTATYPE, l + 1, 1, super_visible ? &nTypeF : &nTypeT);
			l++;
		}
	}
}
void Cmd_graphView::make_scope_table()
{
	_scope_tabe_rows.push_back("Expectation");
	_scope_tabe_rows.push_back("Trend");
	_scope_tabe_rows.push_back("Pias");
	_scope_tabe_rows.push_back("Vias");
	_scope_tabe_rows.push_back("Volume");
	_scope_tabe_rows.push_back("Details");
	_scope_tabe_rows.push_back("Vert Lines");


	const GraphSubsetGroupsMap& gmap = _g->group_map();
	const std::vector<std::string>& gvec = _g->group_vec();

	DWORD dwCellColor = PERGB(255, 255, 0, 0);
	int nTACW = 3;
	int nTypeT = PEGAT_LARGESQUARESOLID;
	int nTypeF = PEGAT_LARGESQUARE; 
	int nJust = PETAJ_CENTER;
	int nHotSpot = 1;
	int rows = (int)_scope_tabe_rows.size() + 1;

	_scope_table_index = _g->make_table("SCOPES", rows, (int)_st._scope_tables.size() + 1); // extra header row
	
	// Set the column text //
	PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, 0, CA2T(""));
	
	// Set the row text //
	for (size_t i = 0; i < _scope_tabe_rows.size(); ++i)
		PEvsetcellEx(m_hPE, PEP_szaTATEXT, int(i+1), 0, CA2T(_scope_tabe_rows[i].c_str()));
	
	for (int i = 0; i < _st._scope_tables.size(); ++i)
	{
		int scope = _st._scope_tables[i];
		std::ostringstream os;
		os << scope;
		PEvsetcellEx(m_hPE, PEP_szaTATEXT, 0, i + 1, CA2T(os.str().c_str()));
		PEvsetcell(m_hPE, PEP_naTACOLUMNWIDTH, i, &nTACW);
		PEvsetcell(m_hPE, PEP_naTAJUSTIFICATION, i, &nJust);

		std::vector<std::string> os_vec;

		os_vec.push_back(ScopeTables::expectation_id(FOLLOWER_STR, scope));
		os_vec.push_back(ScopeTables::trend_id(FOLLOWER_STR, scope));
		os_vec.push_back(ScopeTables::pias_id(FOLLOWER_STR, scope));
		os_vec.push_back(ScopeTables::vias_id(FOLLOWER_STR, scope));
		os_vec.push_back(ScopeTables::volumet_id(FOLLOWER_STR, scope));
		int k = 0;
		for (; k < os_vec.size(); ++k)
		{
			GraphSubsetGroupsMap::const_iterator it = _g->group_map().find(os_vec[k]);
			if (it != _g->group_map().end())
			{
				PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, k + 1, i + 1, &dwCellColor);
				PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, k + 1, i + 1, &nHotSpot);
				PEvsetcellEx(m_hPE, PEP_naTATYPE, k + 1, i + 1, init_visible_group(os_vec[k]) ? &nTypeF : &nTypeT);
			}
		}
		k++;
		PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, k, i + 1, &dwCellColor);
		PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, k, i + 1, &nHotSpot);
		PEvsetcellEx(m_hPE, PEP_naTATYPE, k, i + 1, &nTypeF);

		std::vector<std::string> vs;
		_g->annot_by_group(vs, ScopeTables::expectation_id(FOLLOWER_STR, scope));
		TC_AnnotationData& pa = _g->get_annotation(vs, PEP_faVERTLINEANNOTATION, 0);
		
		k++;
		PEvsetcellEx(m_hPE, PEP_dwaTACOLOR, k, i + 1, &dwCellColor);
		PEvsetcellEx(m_hPE, PEP_naTAHOTSPOT, k, i + 1, &nHotSpot);
		PEvsetcellEx(m_hPE, PEP_naTATYPE, k, i + 1, pa._pa_group_active ? &nTypeT : &nTypeF);
	}
}

//-----------------------------------------------------------
int Cmd_graphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());

    if (ETSLayoutFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	

    RECT rect;
    GetClientRect( &rect );
    // Construct PE Graph Object //
	m_hPE = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);

	_g->add_table_group(LEADER_STR, ZARGB(Z::get_setting("LFGUI:0:Leader:FONTCOLOR", "Blue")));
	_g->add_table_group(FOLLOWER_STR, ZARGB(Z::get_setting("LFGUI:0:Follower:FONTCOLOR", "Black")));
	_g->add_table_group("Trading", ZARGB(Z::get_setting("LFGUI:0:Trading:FONTCOLOR", "Black")));

	_g->init(m_hPE);

	if(Z::get_setting_bool("LFGUI:HORZANDVERTZOOM"))
		PEnset(m_hPE, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);

	_subsets = PEnget(m_hPE, PEP_nSUBSETS);
	set_visible_table();
	_g->init_tables();

	_s->open();
	_s_data->open();
	_s_callback->open();
	_s_callback->notify();
	return 0;
}
void Cmd_graphView::OnSize(UINT nType, int cx, int cy)
{
	ETSLayoutFormView::OnSize(nType, cx, cy);
	if( m_hPE )
	{
		::MoveWindow( m_hPE, 0, 0, cx, cy, FALSE);
	}
}

BOOL Cmd_graphView::OnEraseBkgnd(CDC* pDC)
{
	if(	::IsWindow(m_hPE))			  // Don't do anything, prevents the control from flashing.
		return TRUE;

	return ETSLayoutFormView::OnEraseBkgnd(pDC);
}
BOOL Cmd_graphView::MouseMove()
{
	_g->draw_graph();
	_g->mouse_move();
	return TRUE;
}
BOOL Cmd_graphView::KeyDown()
{
	_g->key_down();
	return TRUE;
}

BOOL Cmd_graphView::CursorMove()
{
	int nSubset = PEnget(m_hPE, PEP_nCURSORSUBSET);
	int nPoint = PEnget(m_hPE, PEP_nCURSORPOINT);

	// find out which axis this subset belongs

	_cursor_y.first = _g->get_subset_axis(nSubset);
	PEvgetcellEx(m_hPE, PEP_faYDATA, nSubset, nPoint, &_cursor_y.second);

	return TRUE;
}
// NOT INVOKED
/*
BOOL Cmd_graphView::ClickedEmpty()
{
	process_run_i();
	return TRUE;
}
*/
BOOL Cmd_graphView::ClickedTable(const HOTSPOTDATA& hsd, int table_)
{
	PEnset(m_hPE, PEP_nWORKINGTABLE, table_);
	int nType = 0;
	
	int row = hsd.w1;
	int col = hsd.w2;

	int nShow = 1;
	PEvgetcellEx(m_hPE, PEP_naTATYPE, row, col, &nType);

	if (nType == PEGAT_LARGESQUARE)
	{
		nType = PEGAT_LARGESQUARESOLID;
		nShow = 1;
	}
	else
	{
		nType = PEGAT_LARGESQUARE;
		nShow = 0;
	}
	PEvsetcellEx(m_hPE, PEP_naTATYPE, row, col, &nType);
	const TableInfo& ti = _g->get_table_info(table_);


	if (hsd.w1 == 0) // on/off table
	{
		if (nShow == false)
			PEnset(m_hPE, PEP_nTAROWS, 1);
		else
			PEnset(m_hPE, PEP_nTAROWS, ti._rows);
	}
	
	else if (table_ == _panel_table_index)
	{
		TCHAR szTmp[512];
		PEvgetcellEx(m_hPE, PEP_szaTATEXT, row, 0, &szTmp);
		std::string row_header = std::string(CT2A(szTmp));

		switch (hsd.w2)
		{

		case 1: // group visibility
			set_visual_subsets(row_header, nShow);
			break;
		case 2: // sub group visibility
			set_visual_subsets(row_header, nShow, true);
			break;
		case 3: // vol table visibility
			PEnset(m_hPE, PEP_nWORKINGTABLE, _g->get_table_info(row_header)._num);
			PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, nShow);
			break;
		}
	}
	else if (table_ == _scope_table_index)
	{
		std::string id;

		switch (hsd.w1)
		{
		case 1:
			id = ScopeTables::expectation_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);
			break;
		case 2:
			id = ScopeTables::trend_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);
			break;
		case 3:
			id = ScopeTables::pias_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);
			break;
		case 4:
			id = ScopeTables::vias_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);
			break;
		case 5:
			id = ScopeTables::volumet_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);
			id = ScopeTables::volumec_id(FOLLOWER_STR, col - 1);
			set_visual_subsets(id, nShow);

			break;
		case 6:
		{
			id = ScopeTables::scope_id(FOLLOWER_STR, col - 1);
			int num = _g->get_table_info(id)._num;
			PEnset(m_hPE, PEP_nWORKINGTABLE, num);
			PEnset(m_hPE, PEP_bSHOWTABLEANNOTATION, nShow);

			_g->manage_annot_menu(ScopeTables::volume_g_id(FOLLOWER_STR, col - 1), nShow, false);
			_g->manage_annot_menu(ScopeTables::volume_t_id(FOLLOWER_STR, col - 1), nShow, false);

			PEdrawtable(m_hPE, num, NULL);
			break;
		}
		case 7:
		{
			id = ScopeTables::expectation_id(FOLLOWER_STR, col - 1);
			std::vector<std::string> vs;

			_g->annot_by_group(vs, id);
			TC_AnnotationData& pa = _g->get_annotation(vs, PEP_faVERTLINEANNOTATION, 0);
			_g->manage_annot_menu(pa, nShow);
			break;
		}
		}
	}
	PEdrawtable(m_hPE, table_, NULL);
	return TRUE;
}
BOOL Cmd_graphView::ClickedAxis(const HOTSPOTDATA& hsd)
{
	//-------------- MIN & MAX on Axis ------------------	
	// mid - middle between min/max
	// reset - size of reset area around mid

	double	fmin	= 0;
	double	fmax	= 0;
	int		nA		= hsd.w1;

	PEnset(m_hPE, PEP_nWORKINGAXIS, nA);
	PEvget(m_hPE, PEP_fMANUALMAXY, &fmax);
	PEvget(m_hPE, PEP_fMANUALMINY, &fmin);

	double scale = (fmax - fmin) / 2;
	if (TCCompareQty().diff(scale, 0) <= 0)
		return TRUE; // not initialized

	double mid = fmin + scale;
	double reset = (scale * 2) / 5;

	//-------------- Mouse position ------------------

	POINT pt;
	PEvget(m_hPE, PEP_ptLASTMOUSEMOVE, &pt);
	int nX = pt.x;
	int nY = pt.y;

	double fX		= 0;
	double fmouse	= 0;

	PEconvpixeltograph(m_hPE, &nA, &nX, &nY, &fX, &fmouse, 0, 0, 0);

	//----------------------------------------------------

	//		TCHAR  buffer[48];

	//------ RESET ---------------------------
	if (fmouse > mid - reset && fmouse < mid + reset)
	{
		//			_stprintf(buffer, TEXT("mouse %.2f mid %.2f reset %.2f"), fmouse, mid, reset);
		//			MessageBox(buffer, L"RESET", MB_OK | MB_ICONINFORMATION);

		_g->set_axis_lock(nA, false);

//		PE_reinit_i();
		return TRUE;
	}
	//-----------------------------------------------------

	double cursor_y = (nA == _cursor_y.first) ? _cursor_y.second : 0; // _cursor_y;
	bool cursor_set = cursor_y != 0 && TCComparePrice::diff(cursor_y, fmin) > 0 && TCComparePrice::diff(fmax, cursor_y) > 0;
	if (!cursor_set)
		cursor_y = mid;

	double step = _pct_zoom == 0 ? _min_zoom : scale*_pct_zoom / 100;
	fmax = cursor_y + scale;
	fmin = cursor_y - scale;
	
	if (fmouse >= mid + reset)	//------ ADD ---------------------------
	{
		fmax += step;
		fmin -= step;
	}
	else						//------ SUBTRACT ---------------------------
	{
		fmax -= step;
		fmin += step;
	}
		//			_stprintf(buffer, TEXT("cur %.2f mid %.2f min %.2f max %.2f "), cursor, mid, fmin, fmax);
		//			MessageBox(buffer, L"SUBTRACT", MB_OK | MB_ICONINFORMATION);
	if (fmax > fmin)
	{
		_g->set_axis_lock(nA, true);
		PEvset(m_hPE, PEP_fMANUALMAXY, &fmax, 1);
		PEvset(m_hPE, PEP_fMANUALMINY, &fmin, 1);
//		PE_reinit_i();
	}

	return TRUE;
}
BOOL Cmd_graphView::Clicked()
{
	HOTSPOTDATA hsd;
	PEvget(m_hPE, PEP_structHOTSPOTDATA, &hsd);

//	if (hsd.nHotSpotType == PEHS_NONE) // Clicked somewhere
//		return ClickedEmpty();

	if (hsd.nHotSpotType >= PEHS_TABLEANNOTATION) // zero represents first table annotation
		return ClickedTable(hsd, hsd.nHotSpotType - PEHS_TABLEANNOTATION);

	if (hsd.nHotSpotType == PEHS_YAXIS)
	{
		std::string& axis_name = _g->get_axis(hsd.w1);
		if (axis_name == "HLOC" || 
			axis_name == "SCOPE" || 
			axis_name == "ECHARGE" || 
			axis_name == "PCHARGE" ||
			axis_name == "VOLUME")
			return ClickedAxis(hsd);
	}
	return TRUE;
}
BOOL Cmd_graphView::CustomMenu()
{
	// Custom Menu was clicked //

	int nM = PEnget(m_hPE, PEP_nLASTMENUINDEX);
	int nSM = PEnget(m_hPE, PEP_nLASTSUBMENUINDEX);
	int n;
	PEvgetcellEx(m_hPE, PEP_naCUSTOMMENUSTATE, nM, nSM, &n);
	if (n == PECMS_UNCHECKED)
		n = PECMS_CHECKED;
	else
		n = PECMS_UNCHECKED;
	PEvsetcellEx(m_hPE, PEP_naCUSTOMMENUSTATE, nM, nSM, &n);
	TCHAR	menu[128];
	ACE_OS::memset(menu, 0, sizeof(menu));
	PEvgetcell(m_hPE, PEP_szaCUSTOMMENUTEXT, nM, menu);

	_g->set_annot_menu(nM, nSM, n);

	return TRUE;
}
void Cmd_graphView::SetSlope(const POINT& pt_)
{
	_slope_points_vec[_slope_points_ind++] = pt_;
	if (_slope_points_ind == 3)
	{
		_slope_points_ind = 0;
		//draw slope wave

		int	nA = 0;
		double fX = 0;
		double fY = 0;
		
		int symbol = PEGAT_THINSOLIDLINE;
		int noBold = 0;
		DWORD col = PERGB(255, 0, 198, 198);

		for (size_t i = 0; i < 3; ++i)
		{
			int nX = _slope_points_vec[i].x;
			int nY = _slope_points_vec[i].y;
			PEconvpixeltograph(m_hPE, &nA, &nX, &nY, &fX, &fY, 0, 0, 0);

			// Draw some lines //
			
			PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONX, 1, &nX);
			PEvsetcell(m_hPE, PEP_faGRAPHANNOTATIONY, 1, &nY);
			if (i > 0)
				symbol = PEGAT_LINECONTINUE;
			PEvsetcell(m_hPE, PEP_naGRAPHANNOTATIONTYPE, 1, &symbol);
			PEvsetcell(m_hPE, PEP_dwaGRAPHANNOTATIONCOLOR, 1, &col);
			PEvsetcell(m_hPE, PEP_szaGRAPHANNOTATIONTEXT, 1, TEXT(""));
			PEvsetcell(m_hPE, PEP_naGRAPHANNOTATIONBOLD, 1, &noBold);
		}
		_g->draw_first_t();
		_g->draw_graph();
		PE_reinit_i();
		_slope_points_ind = 0;
	}
}
BOOL Cmd_graphView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONUP && pMsg->wParam & MK_SHIFT)
	{
		SetSlope(pMsg->pt);
		return false;
	}
	return ETSLayoutFormView::PreTranslateMessage(pMsg);
}

BOOL Cmd_graphView::OnCommand(WPARAM wp, LPARAM lp)
{
	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	bool not_started = (_g.get() == 0 || _g->first_t() == ACE_Time_Value::zero);
	if (not_started)
		return CView::OnCommand(wp, lp);

	bool ret = FALSE;

	switch HIWORD(wp)
	{
	case PEWN_MOUSEMOVE:
		return MouseMove();
		
	case PEWN_CLICKED:
		ret = Clicked();
		_g->draw_first_t();
		_g->draw_graph();
		PE_reinit_i();
		return ret;
		
	case PEWN_CUSTOMMENU:
		ret = CustomMenu();
		break;

	case PEWN_CURSORMOVE:
		ret = CursorMove();
		break;

	default:
		break;
	}
	if (!ret)
		return FALSE;// CView::OnCommand(wp, lp);
	process_run_i();
	return TRUE;
}

bool Cmd_graphView::change_run()
{
	Z::report_debug("--- Cmd_graphView::change_run --- graph reset");

	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	_g->reset();
	_inputs_inactive = 0;
	_s_callback->notify();
	return true;
}
bool Cmd_graphView::process_inputs_inactive()
{
	Z::report_debug("--- Cmd_graphView::process_inputs_inactive");
	_inputs_inactive++;
	_s_callback->notify();
	return true;
}
bool Cmd_graphView::engage_main_thread()
{
	// deposit empty method onto main thread theApp.star() queue
	_s_callback->notify();
	return true;
}
bool Cmd_graphView::process_run()
{
	bool ret = false;
	ACE_Time_Value tv;
	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	ret = process_run_i();
	if (!BMModel::instance()->is_live())
	{
		tv = _s_data->get_input_time();
	}
	if (tv != ACE_Time_Value::zero)
			theApp.mainframe()->get_player().set_time_from(tv);

	if (_inputs_inactive > 0)
	{
		Z::report_debug("--- Cmd_graphView::process_run -- process_inactive");
		_inputs_inactive = 0;
		theApp.mainframe()->get_player().process_inactive();
		theApp.mainframe()->get_player().set_time_from(tv);
	}
	return ret;


}
bool Cmd_graphView::process_run_i()
{
	_g->draw_first_t();
	_g->draw_graph();
	_g->draw_shift();

	_g->mouse_move();
	_g->clicked();
	_g->display_annotations();
	PE_reinit_i();
	return true;
}
void Cmd_graphView::PE_reinit_sub_i()
{
	PEresetimage(m_hPE, 0, 0);
	::InvalidateRect(m_hPE, NULL, FALSE);
}
void Cmd_graphView::PE_reinit_i()
{
	PEreinitialize( m_hPE );
	PE_reinit_sub_i();
}
void Cmd_graphView::set_play(const std::vector<long>& bar_time_, int indx_, const TCBusinessTimeRange& br_)
{
	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	_g->set_play(bar_time_, indx_, br_);
}

void Cmd_graphView::set_dt(const std::vector<long>& bar_time_, int indx_)
{
	ACE_Guard<ACE_Thread_Mutex> guard(_g->lock());
	_g->set_dt(bar_time_, indx_);
	_s_callback->notify();
}
void Cmd_graphView::set_default_graph_resources()
{

	// Set HLOC Line Colors

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:0:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:0:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:0:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:0:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:0:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:0:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:0:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:0:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:1:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:1:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:1:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:1:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:1:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:1:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:1:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:1:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:2:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:2:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:2:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:2:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:2:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:2:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:2:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:2:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:3:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:3:0", "MindightBlue"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:3:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:3:1", "RoyalBlue"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:3:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:3:2", "DarkMagenta"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:3:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:3:3", "Magenta"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:4:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:4:0", "Black"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:4:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:4:1", "DimGray"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:4:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:4:2", "OrangeRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:follower:4:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:follower:4:3", "Orange"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:0:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:0:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:0:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:0:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:0:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:0:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:0:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:0:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:1:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:1:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:1:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:1:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:1:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:1:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:1:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:1:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:2:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:2:0", "DarkGreen"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:2:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:2:1", "Green"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:2:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:2:2", "DarkRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:2:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:2:3", "Red"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:3:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:3:0", "MindightBlue"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:3:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:3:1", "RoyalBlue"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:3:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:3:2", "DarkMagenta"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:3:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:3:3", "Magenta"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:4:0",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:4:0", "Black"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:4:1",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:4:1", "DimGray"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:4:2",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:4:2", "OrangeRed"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_color:leader:4:3",	TCSettings::get("LFGUI:Panel:HLOC:trend_color:leader:4:3", "Orange"));


// Set HLOC Line Types
	// leader
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:0:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:0:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:0:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:0:1", "0"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:0:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:0:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:0:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:0:3", "0"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:1:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:1:0", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:1:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:1:1", "9"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:1:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:1:2", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:1:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:1:3", "9"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:2:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:2:0", "21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:2:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:2:1", "5"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:2:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:2:2", "21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:2:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:2:3", "5"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:3:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:3:0", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:3:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:3:1", "9"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:3:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:3:2", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:3:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:3:3", "9"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:4:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:4:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:4:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:4:1", "0"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:4:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:4:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:leader:4:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:leader:4:3", "0"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:leader:0", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:0","2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:leader:1", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:1","17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:leader:2", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:2","21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:leader:3", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:3","29"));

	// follower
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:0:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:0:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:0:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:0:1", "0"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:0:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:0:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:0:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:0:3", "0"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:1:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:1:0", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:1:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:1:1", "9"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:1:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:1:2", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:1:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:1:3", "9"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:2:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:2:0", "21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:2:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:2:1", "5"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:2:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:2:2", "21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:2:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:2:3", "5"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:3:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:3:0", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:3:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:3:1", "9"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:3:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:3:2", "17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:3:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:3:3", "9"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:4:0", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:4:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:4:1", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:4:1", "0"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:4:2", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:4:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:trend_line:follower:4:3", TCSettings::get("LFGUI:Panel:HLOC:trend_line:follower:4:3", "0"));

	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:follower:0", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:0","2"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:follower:1", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:1","17"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:follower:2", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:2","21"));
	TCSettings::set_resource("LFGUI:Panel:HLOC:expected_annot:follower:3", Z::get_setting("LFGUI:Panel:HLOC:expected_annot:follower:3","29"));

// PCCharge Panel Settings

	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_color:follower:0", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:0", "Blue"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_color:follower:1", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:1", "Red"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_color:follower:2", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:2", "Brown"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_color:follower:3", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:3", "Black"));

	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_line:follower:0", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_line:follower:1", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:1", "2"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_line:follower:2", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:PCHARGE:bias_line:follower:3", Z::get_setting("LFGUI:Panel:PCHARGE:bias_color:follower:3", "2"));

// Volume Panel Settings

	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_color:follower:0", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:0", "Blue"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_color:follower:1", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:1", "Red"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_color:follower:2", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:2", "Brown"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_color:follower:3", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:3", "Black"));

	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_line:follower:0", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:0", "2"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_line:follower:1", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:1", "2"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_line:follower:2", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:2", "2"));
	TCSettings::set_resource("LFGUI:Panel:VOLUME:bias_line:follower:3", Z::get_setting("LFGUI:Panel:VOLUME:bias_color:follower:3", "2"));
}
//=============================================================================================================//
#ifdef _DEBUG
void Cmd_graphView::AssertValid() const
{
	CView::AssertValid();
}

void Cmd_graphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cmd_monitorDoc* Cmd_graphView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cmd_monitorDoc)));
	return (Cmd_monitorDoc*)m_pDocument;
}
#endif //_DEBUG


