/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MODEL_H
#define LF_MODEL_H

#include "znet/ZNet.h"
#include "leaf/LFCommonData.h"
#include "znet/TCTradingTime.h"

//======================================================================
class LEAF_Export LFModel
{
public:
	LFModel() : _trace(false) {}
	virtual ~LFModel() {}

	const std::string& get_name() const { return _name; }
	void set_name(const std::string& name_) {  _name = name_; }

	const ACE_Time_Value& get_last_t() const { return _last_t; }

	// returns the new value if the bar has switched, TCCompare::Nan otherwise
	virtual double add_data(double p_, const ACE_Time_Value& t_, double w_ = 1.) = 0;

	virtual bool is_collected() const = 0;
	virtual double calc() = 0;

	virtual void set_trace(bool trace_) {_trace = trace_;}

protected:
	ACE_Time_Value	_last_t;
	std::string		_name;
	bool			_trace;

};

//======================================================================
class LEAF_Export LFBarModel : public LFModel
{
public:
	LFBarModel(long intervals_, long dt_msec_, long bars_per_interval_, bool average_current_bar_, bool use_average_price_)
		:
	_intervals(intervals_), 
	_dt(dt_msec_),
	_dt_l(LF::length_from_msec(dt_msec_)),
	_bars_per_interval(bars_per_interval_),
	_average_current_bar(average_current_bar_), 
	_use_average_price(use_average_price_), 
	_bar_interval_index(0),
	_last_p(0),
	_last_w(0),
	_last_l(0)
	{}
	virtual ~LFBarModel() {}

	const BMTT_msec_s& dt() const { return _dt; }
	long intervals() const { return _intervals; }
	bool average_current_bar() const { return _average_current_bar; }
	bool use_average_price() const { return _use_average_price; }
	const ACE_Time_Value& get_bar_t() const { return _bar_t; }
	long get_bar_interval_index() const { return _bar_interval_index; }
	double get_last_p() const { return _last_p; }
	const TC_pqp& get_avg_p() const { return _avg_p; }

	void set_bar_t(const ACE_Time_Value& t_) { _bar_t = t_; }
	virtual double add_data(double p_, const ACE_Time_Value& t_, double w_ = 1.);

	virtual bool is_collected() const = 0;
	virtual double calc() = 0;

protected:
	long			_intervals;
	BMTT_msec_s		_dt;
	double			_dt_l;
	long			_bars_per_interval;
	bool			_average_current_bar;
	bool			_use_average_price;
	ACE_Time_Value	_bar_t;
	long			_bar_interval_index;

	double			_last_p;
	TC_pqp			_avg_p;
	double			_last_w;
	double			_last_l;

	virtual double add_point(double p_, double l_, double w_) = 0;


};

typedef Z::smart_ptr<LFBarModel> LFBarModelPtr;

//======================================================================
template<typename M>
class LFModel_T : public LFBarModel
{
public:
	LFModel_T(long intervals_, long dt_msec_, long bars_per_interval_, bool average_current_bar_, bool use_average_price_)
		: LFBarModel(intervals_, dt_msec_, bars_per_interval_, average_current_bar_, use_average_price_),
		_m(intervals_)
	{}

	virtual ~LFModel_T() {}

	virtual bool is_collected() const { return _m.is_collected(); }
	virtual double calc()
	{
		M m(_m);
		return m.add_point(_average_current_bar ? _avg_p.p_p() : _last_p, _last_l, _last_w);
	}

protected:
	M	_m;

	virtual double add_point(double p_, double l_, double w_) { return _m.add_point(p_, l_, w_); }
};

//======================================================================
template<typename M>
class LFSmoothModel : public LFModel
{
public:
	LFSmoothModel(const std::string& name_, long intervals_, long dt_msec_, long step_msec_, bool average_current_bar_, bool use_average_price_) 
		: 
		_dt(dt_msec_),
		_step_dt(step_msec_)
	{
		set_name(name_);
		if(dt_msec_ % step_msec_ != 0)
			z_throw("LFSmoothModel -- bar_msec must be multiple of step_msec");
		long steps_per_bar = dt_msec_ / step_msec_;
		if(steps_per_bar == 0)
			z_throw("LFSmoothModel -- steps_per_bar must be > 0");

		for(long i = 0; i < steps_per_bar; ++i)
		{
			LFBarModelPtr m(new LFModel_T<M>(intervals_, step_msec_, steps_per_bar, 
									average_current_bar_, use_average_price_));
			std::ostringstream os;
			os << get_name() << "[" << i << "]";
			m->set_name(os.str());
			_v.push_back(m);
		}
	}
	virtual ~LFSmoothModel() {}
	virtual void set_trace(bool trace_)
	{
		LFModel::set_trace(trace_);
		for (size_t i = 0; i < _v.size(); ++i)
		{
			_v[i]->set_trace(trace_);
		}
	}

	virtual double add_data(double p_, const ACE_Time_Value& t_, double w_ = 1.)
	{
		ACE_Time_Value step_t = _step_dt.bar_time(t_);
		double ret = TCCompare::NaN;

//		Z::report_info("----- %s -- add_data -- %Q.%d", get_name().c_str(), t_.sec(), t_.usec());
		for(size_t i = 0; i < _v.size(); ++i)
		{
			if(_last_t == ACE_Time_Value::zero)
				_v[i]->set_bar_t(_step_dt.by_index(_dt.bar_time(step_t), (long)i));
			double m = _v[i]->add_data(p_, t_, w_);
			if(!TCCompare::is_NaN(m))
				ret = m;
		}
		_last_t = t_;
		return ret;
	}
	virtual bool is_collected() const { return (*_v.rbegin())->is_collected(); }
	virtual double calc()
	{
		if(_last_t == ACE_Time_Value::zero)
			return 0.;
		ACE_Time_Value step_t = _step_dt.bar_time(_last_t);
		long ind = _step_dt.index(_dt.bar_time(step_t), step_t);
		double m_t = _v[ind]->calc();
		ACE_Time_Value step_t_1 = _step_dt.by_index(step_t, -1);
		long ind_1 = _step_dt.index(_dt.bar_time(step_t_1), step_t_1);
		double m_t_1 = _v[ind_1]->calc();
		double w = (double)((_last_t - step_t).msec())/(double)_step_dt._msec;
		//Z::report_info("===== %s == calc == %Q.%d == ind = %d, ind_1 = %d", 
		//	get_name().c_str(), _last_t.sec(), _last_t.usec(), ind, ind_1);

		return w * m_t + (1. - w) * m_t_1;
//		return _v[ind]->calc();
	}

protected:
	long _smooth_factor;
	BMTT_msec_s		_dt;
	BMTT_msec_s		_step_dt;

	typedef std::vector<LFBarModelPtr> LFBarModelVec;
	LFBarModelVec _v;

};
//======================================================================
template<typename M>
class LFAvgModel : public LFModel
{
public:
	LFAvgModel(const std::string& name_, long intervals_, const std::vector<long> dt_msec_, bool average_current_bar_, bool use_average_price_) 
	{
		set_name(name_);
		if(dt_msec_.size() == 0)
			z_throw("LFAvgModel -- bar_msec can not be empty");

		for(size_t i = 0; i < dt_msec_.size(); ++i)
		{
			LFBarModelPtr m(new LFModel_T<M>(intervals_, dt_msec_[i], 1, 
									average_current_bar_, use_average_price_));
			std::ostringstream os;
			os << get_name() << "[" << i << "]";
			m->set_name(os.str());
			_v.push_back(m);
		}
	}
	virtual ~LFAvgModel() {}

	virtual void set_trace(bool trace_)
	{
		LFModel::set_trace(trace_);
		for (size_t i = 0; i < _v.size(); ++i)
		{
			_v[i]->set_trace(trace_);
		}
	}

	virtual double add_data(double p_, const ACE_Time_Value& t_, double w_ = 1.)
	{
		double ret = TCCompare::NaN;

//		Z::report_info("----- %s -- add_data -- %Q.%d", get_name().c_str(), t_.sec(), t_.usec());
		for(size_t i = 0; i < _v.size(); ++i)
		{
			//if(_last_t == ACE_Time_Value::zero)
			//	_v[i]->set_bar_t(_step_dt.by_index(_dt.bar_time(step_t), (long)i));
			double m = _v[i]->add_data(p_, t_, w_);
			if(!TCCompare::is_NaN(m))
				ret = m;
		}
		_last_t = t_;
		return ret;
	}
	virtual bool is_collected() const { return (*_v.rbegin())->is_collected(); }
	virtual double calc()
	{
		if(_last_t == ACE_Time_Value::zero)
			return 0.;
		double ret = 0;
		for(size_t i = 0; i < _v.size(); ++i)
		{
			ret += _v[i]->calc();
		}

		return ret/_v.size();
	}

protected:
	typedef std::vector<LFBarModelPtr> LFBarModelVec;
	LFBarModelVec _v;

};
//======================================================================
struct LEAF_Export LFM_Incremental
{
	LFM_Incremental(long intervals_)
		: _intervals(intervals_), _count(0), _last_p(0)
	{}

	bool is_collected() const { return _count > _intervals; }

	void add_interval(double p_)
	{
		_last_p = p_;
		_count++;
	}

	long	_intervals;
	long	_count;
	double	_last_p;
};

//======================================================================
struct LEAF_Export LFM_Integral : public LFM_Incremental
{
	LFM_Integral(long intervals_)
		: LFM_Incremental(intervals_), _sum(0)
	{}

	// returns integral
	double add_point(double p_, double l_, double w_);

	double	_sum;

};

//======================================================================
struct LEAF_Export LFM_Chande : public LFM_Incremental
{
	LFM_Chande(long intervals_)
		: LFM_Incremental(intervals_), _su(0), _sd(0)
	{}

	// returns momentum
	double add_point(double p_, double l_, double w_);

	double	_su;
	double	_sd;

};

//======================================================================
class LEAF_Export LFM_Trix : public LFM_Incremental
{
public:
	LFM_Trix(long intervals_)
		:  LFM_Incremental(intervals_), 
		_k(2./(1. + (double)intervals_)), 
		_ema1(0),
		_ema2(0),
		_ema3(0)
	{}
	virtual ~LFM_Trix() {}

	double add_point(double p_, double l_, double w_);

private:
	double _k;
	double _ema1;
	double _ema2;
	double _ema3;

};

//======================================================================
class LEAF_Export LFM_Trix_VIDYA : public LFM_Incremental
{
public:
	LFM_Trix_VIDYA(long intervals_)
		:  LFM_Incremental(intervals_), 
		_k(2./(1. + (double)intervals_)), 
		_ema1(0),
		_ema2(0),
		_ema3(0)
	{}
	virtual ~LFM_Trix_VIDYA() {}

	double add_point(double p_, double l_, double w_);

private:
	double _k;
	double _ema1;
	double _ema2;
	double _ema3;

};

//======================================================================
class LEAF_Export LFM_VIDYA_diff : public LFM_Incremental
{
public:
	LFM_VIDYA_diff(long intervals_)
		:  LFM_Incremental(intervals_), 
		_k(2./(1. + (double)intervals_)), 
		_ema1(0)
	{}
	virtual ~LFM_VIDYA_diff() {}

	double add_point(double p_, double l_, double w_);

private:
	double _k;
	double _ema1;

};

//======================================================================
struct LEAF_Export LFM_Full
{
	LFM_Full(long intervals_)
		: _intervals(intervals_)
	{}

	bool is_collected() const { return (long)_p.size() >= _intervals; }

	void add_interval(double p_)
	{
		if((long)_p.size() >= _intervals)
			_p.pop_front();
		_p.push_back(p_);
	}

	long	_intervals;
	std::list<double>	_p;
};
//======================================================================
class LEAF_Export LFM_LinRegSlope : public LFM_Full
{
public:
	LFM_LinRegSlope(long intervals_)
		:  LFM_Full(intervals_),
		_sumX(0), _sumXSqr(0), _divisor(0)
	{
		_sumX = _intervals * (_intervals - 1) * 0.5 ;
		_sumXSqr = _intervals * (_intervals - 1) * ( 2 * _intervals - 1) / 6;
		_divisor = _sumX * _sumX - _intervals * _sumXSqr;
	}
	virtual ~LFM_LinRegSlope() {}

	double add_point(double p_, double l_, double w_);

private:
	double _sumX;
	double _sumXSqr;
	double _divisor;

};
//======================================================================
//======================================================================
#endif /*LF_MODEL_H*/
