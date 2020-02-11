/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_POINT_STORE_H
#define LF_POINT_STORE_H

#include "znet/ZNet.h"
#include "znet/TCTradingTime.h"
#include "leaf/LFMatch.h"
#include "leaf/LFTeaser.h"
#include "leaf/LFCluster.h"

//======================================================================
//======================================================================
struct LEAF_Export LFPointStore
{
	LFPointStore(LFMarketModel* model_);
	virtual ~LFPointStore();

	void reset();
	bool add_point(const ACE_Time_Value& t_, double p_, double u_vol_, double d_vol_, double u_tran_, double d_tran_);
	bool add_virt_point(const ACE_Time_Value& t_, double p_, double u_vol_, double d_vol_, double u_tran_, double d_tran_);
	bool would_add_virt_point(const ACE_Time_Value& t_) const;

	size_t get_n(double l_, double weight_factor_ = -1, double weight_prec_ = 0, size_t min_n_ = 3);

	void set_last_point(double obv_ = 0, double vma_ = 0, double mfi_ = 0);
	void set_prev_point();

	void fill_last_point(LFDataPoint& dp_) const;
	void fill_prev_point(LFDataPoint& dp_) const;
	void fill_point(LFDataPoint& dp_, size_t ind_) const;
	void find_point(LFDataPoint& dp_, double p_limit_, const LFSide* p_side_, double max_l_) const;

	void find_tip(LFCluster& tip_, const LFCluster& c_, double min_v_) const;

	// reporting
	LFMatchPtr make_report(const std::string& name_, const std::string& key_base_, const LFSide* side_) const;
	LFMatchPtr make_report(const std::string& name_, const std::string& key_base_, const LFBox& box_) const;

	void report(const LFMatchPtr& match_);
	void report(const std::string& name_, const std::string& key_base_, const LFBox& box_, bool out_file_);
	void report(const std::string& name_, const std::string& key_base_, bool out_file_);

	// fill out to_ from _boxes
	void init_merge(LFBox& to_, const LFBox& cur_b_) const;
	void init_merge(LFBox& to_) const;

	LFMarketModel*		_model;
	ACE_Time_Value		_s_t;

	std::vector<ACE_Time_Value>	_tt;
	std::vector<double>	_t;
	std::vector<double>	_w;
	std::vector<double>	_w_linear;
	std::vector<double>	_p;

	std::vector<double>	_p_activity;
	std::vector<double>	_v_activity;
	std::vector<double>	_corr;
	std::vector<double>	_u_vol;
	std::vector<double>	_u_tran;
	std::vector<double>	_d_vol;
	std::vector<double>	_d_tran;

	const LFDataPointPtr& dp() const { return _dp; }
	const LFDataPointPtr& prev_dp() const { return _prev_dp; }

	LFBoxList	_boxes;

	LFMatchPtrList	_to_report;
private:

	LFDataPointPtr	_dp;
	LFDataPointPtr	_prev_dp;

};
typedef Z::smart_ptr<LFPointStore> LFPointStorePtr;

//======================================================================
#endif /*LF_POINT_STORE_H*/

