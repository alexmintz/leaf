/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_SCOPE_STORE_H
#define LF_SCOPE_STORE_H

#include "znet/ZNet.h"
#include "znet/TCTradingTime.h"
#include "leaf/LFPointStore.h"


//======================================================================
struct LEAF_Export LFBoxTrend : public LFBoxVec
{
	enum { TREND_SIZE = 7 };
	typedef LFBoxVec Papa;
	LFBoxTrend();

	bool operator==(const LFBoxTrend& s_) const;
	bool operator!=(const LFBoxTrend& s_) const { return !operator==(s_); }

	const LFBox& operator[](size_t i) const;
	LFBox& operator[](size_t i);
	size_t size() const;

	void reset();

};

//======================================================================
struct LEAF_Export LFBoxTrends : public std::vector<LFBoxTrend>
{
	typedef std::vector<LFBoxTrend> Papa;
	LFBoxTrends();

	bool operator==(const LFBoxTrends& s) const;
	bool operator!=(const LFBoxTrends& s_) const { return !operator==(s_); }

	const LFBoxTrend& operator[](size_t i) const;
	LFBoxTrend& operator[](size_t i);
	size_t size() const;

	void reset();

};

//======================================================================
typedef std::pair<long, long> LFJoinIndex; // first - j_ind, second - offset
typedef std::vector<long> LFIndexVec; 

struct LFScopeStore;
//======================================================================
struct LEAF_Export LFScope : public LFBoxVec
{
	typedef LFBoxVec Papa;

	LFScope();
	virtual ~LFScope() {}

	void init(LFScopeStore* s_store_, LFPointStore* p_store_, const std::string& prefix_, size_t scale_);

	void reset();

	virtual std::string get_key() const { return _key; }
	virtual void set_key(const std::string& k_) { _key = k_; }

	void calc_bias(size_t ind_);

	double calc_exp_contra_p(size_t ind_, double exp_contra_pias_);

	void push_box(const LFBox& b_);
	void update_current(const LFDataPointPtr& dp_);
	void replace_box(size_t ind_, const LFBox& b_);
	void replace_box(long id_, const LFBox& b_) { replace_box((size_t)id_, b_);  }
	void update_avg();
	void update_roll(size_t ind_, bool just_pushed_ = false);

	bool valid_id(long id_) const { return id_ >= 0 && (size_t)id_ < size();  }
	LFBox& roll(long id_) { return (*this)[(size_t)id_]; }
	const LFBox& roll(long id_) const { return (*this)[(size_t)id_]; }

	// init with first_ and merge all < last (-1 till end)
	void init_merge(LFBox& to_, long first_, long last_) const;

	bool is_in(long id_) const;
	bool is_touch_or_out(long ind_) const;

	long join(size_t ind_, bool join_pinned_ = false);
	long check_join(std::string& reason_);
	long check_join(size_t ind_, std::string& reason_);
	long check_child_join(std::string& reason_);

	bool joinable(size_t ind_, std::string& reason_);
	bool set_ended(size_t ind_);

	bool check_not_ended(long id_, std::string& reason_);

	bool check_init(const LFBoxTrend& charge_trend_);
	bool check_init();

	bool check_set_ended(const LFBoxTrend& charge_trend_, bool force_ended_, std::string& action_);
	bool check_set_ended(std::string& action_);

	void check_set_continuous(long id_);

	void check_set_pattern();

	void set_identity(long id_);
	void update_identities(long first_);

	void update(const LFBox& b_, const LFBoxTrend& charge_trend_, bool force_ended_);
	void update();

	void set_min(double l_ = 0, double v_ = 0) { _min_l = l_; _min_v = v_; }
	std::string min_as_string() const;

	long find_lower_index(const LFPoint& p_) const;
	// trend
	size_t find_ind_best(const LFSide* side_, const ACE_Time_Value& t_) const;

	void find_indices(const LFCluster& c_, size_t& pin_, size_t& bin_) const;

	size_t get_ind(const LFSide* side_, bool ended_) const;

	long last_id() const { return (long)last_ind(); }
	size_t last_ind() const { return size() - 1; }

	size_t ended_ind() const
	{
		return get_ind(0, true);
	}
	size_t peak_ind(const LFSide* trend_side_) const
	{
		return get_ind(trend_side_, false);
	}
	size_t peak_ended_ind(const LFSide* trend_side_) const
	{
		return get_ind(trend_side_, true);
	}
	size_t bounce_ind(const LFSide* trend_side_) const
	{
		return get_ind(trend_side_->contra_s(), false);
	}
	size_t bounce_ended_ind(const LFSide* trend_side_) const
	{
		return get_ind(trend_side_->contra_s(), true);
	}

	void model_send(size_t ind_, const std::string& comment_, const ACE_Time_Value& t_, bool out_file_, const std::string& key_suffix_) const;
	void clear_just_flags();

	void fill_box_trend(LFBoxTrend& tr_) const;
	void add_joined(LFJoinIndex j_ind_);
	void reset_joined() {_joined = -1;	}


	LFScopeStore*	_s_store;
	LFPointStore*	_p_store;
	LFMarketModel*	_model;
	std::string		_prefix;
	std::string		_key;
	long			_scale;

	double	_avg_l;
	double	_avg_v;
	long	_avg_cnt;

	double	_min_l;
	double	_min_v;
	long	_joined;

	// just flags
	const LFSide* _just_ended_side;

	//settings
	double	_crl_bias_min_l;
	double	_crl_bias_min_v;


};
//======================================================================
struct LEAF_Export LFScopeStore : public std::vector<LFScope>
{
	enum { NUMBER_OF_SCOPES = 6 };

	typedef std::vector<LFScope> Papa;
	LFScopeStore(LFPointStore* p_store_);
	virtual ~LFScopeStore() {}

	//AK TODO
	size_t find_scope(const LFCluster& c_, size_t& pin_, size_t& bin_) const;
	//AK TODO
	size_t find_bounce_scope(const LFCluster& c_, bool exact_pin_) const;

	void reset();
	void clear_just_flags();

	bool valid_scale(long scale_id_) const { return scale_id_ >= 0 && (size_t)scale_id_ < size(); }
	LFScope& scope(long scale_id_) { return (*this)[(size_t)scale_id_]; }
	const LFScope& scope(long scale_id_) const { return (*this)[(size_t)scale_id_]; }

	long scope_size(long scale_id_) const { return(long) scope(scale_id_).size(); }

	bool valid_id(long scale_id_, long roll_id_) const { return scope(scale_id_).valid_id(roll_id_); }
	const LFBox& roll(long scale_id_, long roll_id_) const { return scope(scale_id_).roll(roll_id_); }
	LFBox& roll(long scale_id_, long roll_id_)  { return scope(scale_id_).roll(roll_id_); }

	const LFBox& roll(const LF::Id& id_) const { return scope(id_._scale).roll(id_._roll); }
	LFBox& roll(const LF::Id& id_)  { return scope(id_._scale).roll(id_._roll); }

	bool child_valid(const LFPoint& p_) const;
	void set_top_scale(long scale_id_, long roll_id_, long top_scale_);
	bool set_child_index(LFPoint& p_);

	// init with first_ and merge all < last (-1 till end)
	void init_merge(LFBox& to_, long scale_id_, long first_, long last_ = -1) const;
	bool child_confirm_bounce(const LFBox& p_) const;
	bool child_confirm_exp(const LFBox& p_) const;

	long find_parent_index(const LFPoint& p_) const;

	LF::Id first_child_id(const LF::Id& id_) const;
	LF::Id bounce_id(const LF::Id& id_) const;

	void set_children_top_scale(const LF::Id& id_);
	// starting from most recent
	void fill_blossom_ids(const LF::Id& id_, long top_scale_,  LF::IdVec& bvec_) const;

	void fill_box_trends(LFBoxTrends& trs_) const;
	void fill_lcy_trends(LFBoxVec& bvec_, LFBoxTrends& trs_) const;

	LFPointStore*	_p_store;
	LFMarketModel*	_model;
};

//======================================================================
#endif /*LF_SCOPE_STORE_H*/

