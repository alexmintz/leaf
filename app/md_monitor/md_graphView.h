// md_graphView.h : interface of the Cmd_monitorView class
//

#pragma once
#include "znet/ZNet.h"
#include "znet/TCPQP.h"
#include "znet/BMStrategy.h"
#include "znet/TCTradingTime.h"
#include "leaf/LFData.h"
#include "vc/ETSLayout.h"
#include "vc/TC_ListCtrl.h"
#include "Resource.h"

#include "vc/TC_Graph.h"

#include "md_monitorDoc.h"

struct ScopeTables
{
	ScopeTables();
	tc_vector<long> _scope_tables;

	static std::string expectation_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "E" << scope_;
		return osi.str();
	}
	static std::string trend_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "T" << scope_;
		return osi.str();
	}
	static std::string pias_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "P" << scope_;
		return osi.str();
	}
	static std::string vias_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "VolDen" << scope_;
		return osi.str();
	}
	static std::string scope_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "S" << scope_;
		return osi.str();
	}
	static std::string volume_g_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "V" << scope_;
		return osi.str();
	}
	static std::string volume_t_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "v" << scope_;
		return osi.str();
	}
	static std::string volumet_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "VolT" << scope_;
		return osi.str();
	}
	static std::string volumec_id(const std::string& actor_, size_t scope_)
	{
		std::ostringstream osi; osi << actor_[0] << "VolC" << scope_;
		return osi.str();
	}
};
//======================================================================
class LFGraph_TradeData : public TC_Graph_Vol
{
public:
	LFGraph_TradeData(BMStrategy& s_, const std::string& key_, const std::string& input_,
		bool always_inactive_ = false, long hwm_= 0, long lwm_ = 0)
		: TC_Graph_Vol("Volume"), _sub(s_, key_, input_, always_inactive_, hwm_, lwm_)
	{}
	virtual ~LFGraph_TradeData() {}

	// resets all the data objects
	virtual void reset();

	virtual void init_data()
	{
		_sub._s.subscribe_input(this, &LFGraph_TradeData::process, _sub._key, _sub._input, _sub._always_inactive, _sub._hwm, _sub._lwm);
	}
	virtual void fini_data()
	{
		_sub._s.unsubscribe_input<LFTradeData>(_sub._input);
	}

	virtual void process(LFTradeDataPtr*);

protected:
	BMStrategySubscriber _sub;

};
//======================================================================
class LFGraph_TradeData_Leader : public LFGraph_TradeData
{
public:
	LFGraph_TradeData_Leader(BMStrategy& s_, const std::string& key_, const std::string& input_,
		bool always_inactive_ = false, long hwm_= 0, long lwm_ = 0)
		: LFGraph_TradeData(s_, key_, input_, always_inactive_, hwm_, lwm_)
	{}
	virtual ~LFGraph_TradeData_Leader() {}
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);
};
//======================================================================
class LFGraph_TradeData_Follower : public LFGraph_TradeData
{
public:
	LFGraph_TradeData_Follower(BMStrategy& s_, const std::string& key_, const std::string& input_,
		bool always_inactive_ = false, long hwm_= 0, long lwm_ = 0)
		: LFGraph_TradeData(s_, key_, input_, always_inactive_, hwm_, lwm_)
	{}
	virtual ~LFGraph_TradeData_Follower() {}
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);
};
//======================================================================
class LFGraph_Panel: public TC_GraphData
{
public:
	LFGraph_Panel(const std::string& name_=""):TC_GraphData(name_), _roll_scale_index(0)
	{
		_roll_scale_index = Z::get_setting_integer("LFGUI:Panel:roll_scale_index", 0);
	}
	virtual ~LFGraph_Panel(){}


	virtual int subsets();
	virtual void reset();
	virtual void load_graph();
	virtual void reset_graph();
	virtual void draw_graph();

	virtual void process(LFStrategyData&){}
	virtual void process_pos(LFPositionData&){}
	virtual void process_fills(LFOrderFill&){}
	virtual const ACE_Time_Value& get_last_graph_tv() const { return _gd_v[0]->get_last_graph_tv();}

	// implement in inheritance
	virtual void subsets_by_axes(std::vector<int>& v_) = 0;
	virtual void init(TC_Graph* g_, int first_subset_, int axis_) = 0;

	void bind(TC_GraphDataPtr p_) { _gd_v.push_back(p_); }
protected:
//	std::string make_scope_annot_id(const std::string& actor_, size_t scope_, const std::string& type_="E");

	TC_GraphDataVec _gd_v;
	long	_roll_scale_index;
};
//======================================================================
typedef Z::smart_ptr<LFGraph_Panel>		LFGraph_PanelPtr;
typedef std::vector<LFGraph_PanelPtr>	LFGraph_PanelVec;
//======================================================================

//--------------------------------------------------------------------
class LFGraph_HLOC_Last: public TC_Graph_Last
{
public:
	LFGraph_HLOC_Last(const std::string& name_, TC_GraphData* hloc_ = 0) : TC_Graph_Last(name_), _hloc(hloc_) { set_yscale(ysAuto); }
	virtual ~LFGraph_HLOC_Last() {}
	void display_table(int index_);

protected:
	TC_GraphData*	_hloc;
};
class LFGraph_HLOC_Band
{
public:
	LFGraph_HLOC_Band(LFGraph_Panel& p_, const std::vector<std::string>& labels_, TC_GraphData* hloc_);
	~LFGraph_HLOC_Band(){}

	void display_trend(const ACE_Time_Value& t_, const LFBoxTrend& trend_, double p_);
	void display_expected(const ACE_Time_Value& t_, const LFBoxTrend& trend_, double p_);
	void init(TC_Graph* g_,
			  int& first_subset_,
			  int axis_,
			  const std::string& group_name_);
	size_t subsets(){ return 4;}
	virtual const ACE_Time_Value& get_last_graph_tv() const { return _v[0]->get_last_graph_tv();}

//protected:
	TC_GraphDataVec _v;

};

typedef Z::smart_ptr<LFGraph_HLOC_Band> HLOC_BandPtr;
//--------------------------------------------------------------------
//--------------------------------------------------------------------


class LFGraph_Panel_HLOC: public LFGraph_Panel
{
public:
	LFGraph_Panel_HLOC(const std::string& name_);
	virtual ~LFGraph_Panel_HLOC(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);
	virtual const ACE_Time_Value& get_last_graph_tv() const { return _l_hloc->get_last_graph_tv();}
	virtual void process(LFStrategyData&);
	virtual void reset();
	void load_resource();

protected:
	void make_subset(std::vector<HLOC_BandPtr>& scope_,
					 const std::string& actor_,
					 bool as_trend_,
					 int& su_first_);

	TC_GraphDataPtr			_l_hloc;
	TC_GraphDataPtr			_f_hloc;

	std::vector<HLOC_BandPtr>	_l_tscopes;
	std::vector<HLOC_BandPtr>	_f_tscopes;

	std::vector<HLOC_BandPtr>	_l_escopes;
	std::vector<HLOC_BandPtr>	_f_escopes;

	size_t					_show_charge_on_scope;
	double					_follower_sp;

};
//--------------------------------------------------------------------
class LFGraph_Panel_Scope : public LFGraph_Panel
{
public:
	LFGraph_Panel_Scope(const std::string& name_);
	virtual ~LFGraph_Panel_Scope(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);

	virtual void process(LFStrategyData&);
	virtual void reset();
	void load_resource();

private:
	void init_scope_annotations(const std::string& actor_);

	bool _show_lcy_trends;
};
//----------------------------------------------------------------------
class LFGraph_Panel_Energy: public LFGraph_Panel
{
public:
	LFGraph_Panel_Energy(const std::string& name_);
	virtual ~LFGraph_Panel_Energy(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);

	virtual void process(LFStrategyData&);
	virtual void reset();

	void load_resource();
private:
	double	_activity_level_high;
	double	_activity_level_low;
	LFBox	_box;
};
//-----------------------------------------------------------------------
class LFGraph_Panel_EnergyCharge: public LFGraph_Panel
{
public:
	LFGraph_Panel_EnergyCharge(const std::string& name_);
	virtual ~LFGraph_Panel_EnergyCharge(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);

	virtual void process(LFStrategyData&);
	virtual void reset();

	void load_resource();
private:
	virtual void init_scope_bias(const std::string& actor_,
		const std::string& group_,
		size_t& index_,
		int& su_first_,
		int& subset_lines_,
		int& axis_);
	LFBox	_box;
};
//-----------------------------------------------------------------------
class LFGraph_Panel_PriceCharge: public LFGraph_Panel
{
public:
	LFGraph_Panel_PriceCharge(const std::string& name_);
	virtual ~LFGraph_Panel_PriceCharge(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	void init(TC_Graph* g_, int first_subset_, int axis_);
	virtual void process(LFStrategyData&);
	virtual void reset();

	void load_resource();
private:
	virtual void init_scope_bias(const std::string& actor_,
								const std::string& group_,
								size_t& index_,
								int& su_first_,
								int& subset_lines_,
								int& axis_);

	bool	_show_charge_annot;
	LFBox	_box;
};

//-----------------------------------------------------------------------
class LFGraph_Panel_Volume : public LFGraph_Panel
{
public:
	LFGraph_Panel_Volume(const std::string& name_);
	virtual ~LFGraph_Panel_Volume(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	void init(TC_Graph* g_, int first_subset_, int axis_);
	virtual void process(LFStrategyData&);

	virtual void reset();
	void load_resource();
private:
	virtual void init_scope_bias(const std::string& actor_,
		const std::string& group_,
		size_t& index_,
		int& su_first_,
		int& subset_lines_,
		int& axis_);
	long	_scope; // -1 - show micro level scope
	bool	_keep_last;
};

//-----------------------------------------------------------------------
class LFGraph_Panel_Density : public LFGraph_Panel
{
public:
	LFGraph_Panel_Density(const std::string& name_);
	virtual ~LFGraph_Panel_Density(){}

	virtual void subsets_by_axes(std::vector<int>& v_);
	void init(TC_Graph* g_, int first_subset_, int axis_);
	virtual void process(LFStrategyData&);

	virtual void reset();
	void load_resource();
private:
	long	_scope; // -1 - show micro level scope
	bool	_keep_last;

};

//======================================================================

class LFGraph_Panel_Position : public LFGraph_Panel
{
public:
	LFGraph_Panel_Position(const std::string& name_);
	virtual ~LFGraph_Panel_Position() {}

	virtual int subsets() { return 2; }
	virtual void subsets_by_axes(std::vector<int>& v_);

	virtual void reset();
	virtual void reset_graph();
	virtual void init(TC_Graph* g_, int first_subset_, int axis_);

	virtual void process(LFStrategyData&);
	virtual void process_pos(LFPositionData&);
	virtual void process_fills(LFOrderFill&);

	int fill_rows() { return 6;}
	virtual int rows() { return 14; } //16
	virtual void display_table(int index_);
	virtual void display_click(int index_);

protected:
	void load_resource();
	void init_scope_annotations(const std::string& actor_);
	void init_tables(const std::string& actor_);
	void display_pos_table(int index_);
	void display_scope_table(const LFPositionData& d_);
	void display_scope_item(const LFBoxTrend& td_, int i_);
	void display_position(const LFPositionDataPtr&, int index_);

	void display_fill_table(int index_);
	void display_fill_annot(LFOrderFill& d_, double pnl_);

	void process_last_pos(LFPositionDataPtr pp_);
	void process_trend_annot(LFPositionData& d_);
	void add_scope_annot(const LFBox& b_, int r, const std::vector<std::string>& v_);
	void add_scope_annot_s(const LFBox& b_, int r,
		const std::vector<std::string>& vs_,
		const std::vector<std::string>& vS_);
	void end_scope_annot_s(const LFBox& b_, int r,
		const std::vector<std::string>& vs_,
		const std::vector<std::string>& vS_);

	void add_level_annot(LFPositionData& d_);

	void add_level_annot_box(const LFBox& sl_,
							size_t level_,
							const std::string& comment_, 
							bool show_end_);

	typedef std::map<ACE_Time_Value, LFPositionDataPtr> PositionMap;
	PositionMap _p_map;

	size_t				 _table_rows;
	size_t				 _table_cols;

	typedef std::pair<int, const char*> PCol;
	std::vector< PCol > _table_v;
	ScopeTables			_st;

	typedef std::vector< std::pair<LFOrderFill, double> > Fills;
	Fills _fills;

	typedef std::list<LFOrderFill> FillList;
	typedef std::map<long, FillList> FillsByIndex;
	FillsByIndex _fbin;

	LF::RefPair<TC_AnnotationData> _level_annot;
	LF::RefPair<TC_AnnotationData> _level_g_annot;

	bool							_show_lcy;

	std::string _enrg_annot_id;
	std::string _level_annot_id;
	std::string _level_g_annot_id;

	long _w_id;
	long _p_id;
	long _s_id;

	int					_hloc_table_inx;
	std::vector<int>	_scope_table_inx;
};


//-----------------------------------------------------------------------

class LFGraph_StrategyData : public TC_GraphData
{
public:
	LFGraph_StrategyData(BMStrategy& s_, const std::string& key_, const std::string& input_,
		bool always_inactive_ = false, long hwm_= 0, long lwm_ = 0);

	virtual ~LFGraph_StrategyData() {}

	// number of subsets
	virtual int subsets();
	virtual void subsets_by_axes(std::vector<int>& v_);
	virtual void overlap_multi_axis(std::vector<int>& v_);

	// read config and set annotations
	// resets all the data objects
	virtual void reset();
	// loads stored data to the graph
	virtual void load_graph();
	virtual void reset_graph();
	virtual void draw_graph();

	virtual void init(TC_Graph* g_, int first_subset_, int axis_);

	virtual void init_data()
	{
		_sub._s.subscribe_input(this, &LFGraph_StrategyData::process, _sub._key, _sub._input, _sub._always_inactive, _sub._hwm, _sub._lwm);
		_sub_pos._s.subscribe_input(this, &LFGraph_StrategyData::process_pos,	_sub_pos._key, _sub_pos._input, _sub_pos._always_inactive, _sub_pos._hwm, _sub_pos._lwm);
		_sub_pos._s.subscribe_input(this, &LFGraph_StrategyData::process_fills, _sub_pos._key, _sub_pos._input, _sub_pos._always_inactive, _sub_pos._hwm, _sub_pos._lwm);
	}
	virtual void fini_data()
	{
		_sub._s.unsubscribe_input<LFStrategyData>		(_sub._input);
		_sub_pos._s.unsubscribe_input<LFPositionData>	(_sub_pos._input);
		_sub_pos._s.unsubscribe_input<LFOrderFill>		(_sub_pos._input);
	}

	virtual void process(LFStrategyDataPtr*);
	virtual void process_pos(LFPositionDataPtr*);
	virtual void process_fills(LFOrderFillPtr*);

	virtual const ACE_Time_Value& get_last_graph_tv() const { return _gdv[0]->get_last_graph_tv(); }

protected:
	BMStrategySubscriber _sub;
	BMStrategySubscriber _sub_pos;

	LFGraph_PanelVec	_gdv;
};
//======================================================================
//======================================================================
class Cmd_graphView : public ETSLayoutFormView
{
public:
	virtual void OnInitialUpdate()
	{
		ETSLayoutFormView::OnInitialUpdate();
		GetParentFrame()->RecalcLayout();
		ResizeParentToFit(FALSE);
		ResizeParentToFit(TRUE);
		ShowWindow(SW_MAXIMIZE);
	}
	Cmd_monitorDoc* GetDocument() const;

	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;
	void OnSize(UINT nType, int cx, int cy);
	BOOL OnEraseBkgnd(CDC* pDC);
	BOOL OnCommand(WPARAM wp, LPARAM lp);
	BOOL PreTranslateMessage(MSG* pMsg);

	bool process_run();
	bool engage_main_thread();
	bool change_run();
	bool process_inputs_inactive();

	void set_dt(const std::vector<long>& bar_time_, int indx_);
	void set_play(const std::vector<long>& bar_time_, int indx_, const TCBusinessTimeRange& br_);
protected:
	Cmd_graphView(UINT nID = IDD_GRAPH);
	virtual ~Cmd_graphView();
	bool process_run_i();
	void PE_reinit_i();
	void PE_reinit_sub_i();

	void set_default_graph_resources();

	DECLARE_DYNCREATE(Cmd_graphView)
	DECLARE_MESSAGE_MAP()


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	void set_visible_table();
	bool init_visible_group(const std::string& gid_);

	void make_panel_table();
	void make_scope_table();

	void init_visible_subsets();
	void set_visual_subsets(const std::string& gid_, int show_, bool sub_group_ = false);

	BOOL MouseMove();
	BOOL KeyDown();
	BOOL CursorMove();
	BOOL Clicked();
//	BOOL ClickedEmpty();
	BOOL ClickedTable(const HOTSPOTDATA& hsd, int table_);
	BOOL ClickedAxis(const HOTSPOTDATA& hsd);
	BOOL CustomMenu();
	void SetSlope(const POINT& pt_);

	auto_ptr<BMStrategy>		_s;
	auto_ptr<BMStrategy>		_s_data;
	auto_ptr<Z::ActiveCallback>	_s_callback;

	HWND m_hPE;			// the proessentials control object

	auto_ptr<TC_Graph>		_g;
	ACE_Auto_Array_Ptr<int>	_visible_config; //array
	ACE_Auto_Array_Ptr<int>	_visible; //array
	size_t					_subsets;
	tc_vector<std::string>	_exclude_group;
	tc_vector<std::string>	_exclude_subset;
//	std::map<int, int>		_table_index_map; //row number -> table number
	double					_min_zoom;
	double					_pct_zoom; // if 0 then use _min_zoom;
	std::pair<int, float>	_cursor_y;
	ScopeTables				_st;
	int						_panel_table_index;
	int						_scope_table_index;
	std::vector<std::string> _scope_tabe_rows;
	std::vector<POINT>		_slope_points_vec;
	size_t					_slope_points_ind;

	ACE_Atomic_Op<ACE_Thread_Mutex, long> _inputs_inactive;

};

#ifndef _DEBUG  // debug version in md_graphView.cpp
inline Cmd_monitorDoc* Cmd_graphView::GetDocument() const
   { return reinterpret_cast<Cmd_monitorDoc*>(m_pDocument); }
#endif

