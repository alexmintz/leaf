/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFModel.h"
#include <sstream>
#include <cmath>

//======================================================================
double LFBarModel::add_data(double p_, const ACE_Time_Value& t_, double w_)
{
	double ret = TCCompare::NaN;
	if(t_ < _bar_t)
		return ret;
	ACE_Time_Value bar_t = _dt.bar_time(t_);

	bool p_changed = TCComparePrice::diff(p_, _last_p) != 0;
	if(_bar_t == ACE_Time_Value::zero)

	{
		_bar_t = bar_t;
		_bar_interval_index = 0;
		ret = add_point(p_, _dt_l*_bars_per_interval, w_);
	}
	else if(_bar_t != bar_t)
	{
		long bars = _dt.index(_bar_t, bar_t);
		while(bars-- > 0)
		{
			if(++_bar_interval_index ==_bars_per_interval)
			{
				_bar_interval_index = 0;
				ret = add_point(_use_average_price ? _avg_p.p_p() : _last_p, _dt_l*_bars_per_interval, w_);
				p_changed = true;
			}
		}
		_bar_t = bar_t;
		if(_bar_interval_index == 0)
			_avg_p.reset();
	}
	_last_p = p_;
	_last_w = w_;
	_last_t = t_;
	_last_l = _dt_l*_bar_interval_index + LF::length_from_t(_last_t - _bar_t);

	if(p_changed)
		_avg_p += TC_pqp(_last_p, 1);

	return ret;

/*
	if(_bar_t == ACE_Time_Value::zero)
	{
		_bar_t = bar_t;
		_bar_interval_index = 0;
		ret = add_point(p_);
	}
	if(_bar_t != bar_t)
	{
		long bars = _dt.index(_bar_t, bar_t);
		if(_bar_interval_index == 0)
		{
			long points =  bars/_bars_per_interval;
			if(points == 0)
				points++;
			double p = p_;
			if(_avg_p.good())
				p = _avg_p.p_p();
			for(long i = 0; i < points; ++i)
			{
				ret = add_point(p);
			}
		}
		_bar_interval_index = bars%_bars_per_interval;
		if( _bar_interval_index == 0)
		{
			_avg_p.reset();
			p_changed = true;
			_bar_t = bar_t;
		}
	}
	if(_bar_t == bar_t)
	{
		_last_p = p_;
		_last_t = t_;
		if(p_changed)
			_avg_p += TC_pqp(_last_p, 1);
	}

	return ret;
*/
}
//======================================================================
//======================================================================
double LFM_Integral::add_point(double p_, double l_, double w_)
{
	add_interval(p_);

	_sum += p_*l_;
	return _sum;

}

//======================================================================
//======================================================================
double LFM_Chande::add_point(double p_, double l_, double w_)
{
	double c = TCComparePrice::diff(p_, _last_p);
	add_interval(p_);
	if(_count <= 1)
		return 0.;

	if(_count > _intervals)
	{
		_su = _su * (double)(_intervals - 1)/(double)_intervals;
		_sd = _sd * (double)(_intervals - 1)/(double)_intervals;
	}

	if(c >= 0)
	{
		_su += c;
	}
	else
	{
		_sd -= c;
	}
	if((_count <= _intervals))
	{
		return 0.;
	}

	double m = 0.;
	if((_su + _sd) != 0)
		m = (_su - _sd)* 100. / (_su + _sd);

	return m;

}

//======================================================================
//======================================================================
double LFM_Trix::add_point(double p_, double l_, double w_)
{
	add_interval(p_);
	if(_count <= _intervals)
	{
		_ema1 = (_ema1*(double)(_count - 1) + p_)/(double)_count;
		_ema2 = _ema1;
		_ema3 = _ema2;
		return 0.;
	}
	_ema1 += _k *  (p_ - _ema1);
	_ema2 += _k * (_ema1 - _ema2);
	double prev_ema3 = _ema3;
	_ema3 += _k * (_ema2 - _ema3);

	if(prev_ema3 == 0)
		return 0.;

	double m = (_ema3 - prev_ema3)*100000./prev_ema3;

	return m;

}
//======================================================================
//======================================================================
double LFM_Trix_VIDYA::add_point(double p_, double l_, double w_)
{
	add_interval(p_);
	if(_count <= _intervals)
	{
		_ema1 = (_ema1*(double)(_count - 1) + p_)/(double)_count;
		_ema2 = _ema1;
		_ema3 = _ema2;
		return 0.;
	}
	_ema1 += _k * w_ *  (p_ - _ema1);
	_ema2 += _k * w_ * (_ema1 - _ema2);
	double prev_ema3 = _ema3;
	_ema3 += _k * w_ * (_ema2 - _ema3);

	if(prev_ema3 == 0)
		return 0.;

	double m = (_ema3 - prev_ema3)*100000./prev_ema3;

	return m;

}

//======================================================================
//======================================================================
double LFM_VIDYA_diff::add_point(double p_, double l_, double w_)
{
	add_interval(p_);
	if(_count <= _intervals)
	{
		_ema1 = (_ema1*(double)(_count - 1) + p_)/(double)_count;
		return 0.;
	}
	_ema1 += _k * w_ * (p_ - _ema1);

	return p_ - _ema1;

}
//======================================================================
//======================================================================
double LFM_LinRegSlope::add_point(double p_, double l_, double w_)
{
	add_interval(p_);
	if((long)_p.size() < _intervals)
		return 0.;

	double sumXY = 0;
	double sumY = 0;
	int n = 0;
	for(std::list<double>::reverse_iterator i = _p.rbegin(); i != _p.rend()&& n <= _intervals; ++i, ++n)
	{
		sumY += (*i);
		sumXY += (double)n * (*i);
	}
	double m = ( _intervals * sumXY - _sumX * sumY) / _divisor;
//	 std_atan(m) * ( 180.0 / PI );


	return m;

}

//======================================================================
