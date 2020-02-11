/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFBox.h"
#include <cstdlib> 
#include <set>
#include <cfloat>

//======================================================================
double LFBox::BIAS_MIN_VALUE = 0.00001;

//======================================================================
LFBox::LFBox(const std::string& key_)
 :_key(key_),
 	_side(0),
	_max_corr(0),
	_min_corr(-1),
	_w_l(0),
	_w_v(0),
	_w_dv(0),
	_e_w(0),
	_w_s(0),
	_e_v(TCCompare::NaN),
	_e_in_v(TCCompare::NaN),
	_exp_contra_p(TCCompare::NaN),
	_pinned(false),
	_w_changed(false),
	_bias(0),
	_s_bias(LFBox::UNKNOWN_BIAS),
	_pias(0),
	_d_pias(0),
	_s_pias(LFBox::UNKNOWN_BIAS)
{
}
//======================================================================
LFBox::~LFBox()
{
}
//======================================================================
const char* LFBox::summary_header()
{
	return ",B_T,B_P,B_SIDE,B_PIAS";
}

//======================================================================
void LFBox::summary_to_stream(std::ostream& os) const
{
	LF::summary_to_stream(os, _s_dp);
	os << "," << _side ;
	os << "," << _pias ;

}
//======================================================================
const char* LFBox::header()
{
	return ",B_KEY,ID,SIDE,PATTERN,COM,\
V,L,S,VOL,\
S_T,S_P,MAX_T,MAX_P,MIN_T,MIN_P,C_T,C_P,\
W_T,W_P,END_T,TURN_T,\
EXP_P,PIAS,DPIAS,W_S,W_DECAY_L,W_DECAY_V,EXP_W,\
TRIG,PINNED";
}
//======================================================================
void LFBox::body_to_stream(std::ostream& os) const
{
	os	<< "," << _key;
	os << "," << _w._id.to_alfa();
	os << "," << _side;
	os << "," << _w._pattern.to_alfa();
	os << "," << _comment;
	os << "," << _w._cluster_v;
	os << "," << _w._cluster_l;
	os << "," << _w._cluster_s;
	os << "," << _w._cluster_vol;
	LF::summary_to_stream(os, _s_dp);
	LF::summary_to_stream(os, _max_dp);
	LF::summary_to_stream(os, _min_dp);
	LF::summary_to_stream(os, _c_dp);
	LF::summary_to_stream(os, _w_dp);
	os << "," << TCTimestamp(_end_t).c_str();
	os << "," << TCTimestamp(_turned_t).c_str();
	os << "," << _exp_contra_p;
	os << "," << _pias ;
	os << "," << _d_pias;
	os << "," << _w_s;
	os << "," << _e_v;
	os << "," << _e_in_v;
	os << "," << _e_w;
	os << "," << triggered() ? 1 : 0;
	os << "," << _pinned?1:0 ;
	
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFBox& d_)
{
	d_.to_stream(os);

	d_.body_to_stream(os);
	return os;
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFBox& d_)
{
	if (!d_.to_cdr(strm)) 
	  return false;
	if (!(strm << d_._key)) 
	  return false;
	if (!(strm << d_._side))
		return false;
	if (!(strm << d_._s_dp))
		return false;
	if (!(strm << d_._c_dp))
		return false;
	if (!(strm << d_._max_dp))
		return false;
	if (!(strm << d_._min_dp))
		return false;
	if (!(strm << d_._w_dp))
		return false;
	if (!(strm << d_._max_corr)) 
	  return false;
	if (!(strm << d_._min_corr))
		return false;
	if (!(strm << d_._end_t))
	  return false;
	if (!(strm << d_._turned_t)) 
	  return false;
	if (!(strm << d_._w_l))
		return false;
	if (!(strm << d_._w_v))
		return false;
	if (!(strm << d_._w_dv))
		return false;
	if (!(strm << d_._e_w))
		return false;
	if (!(strm << d_._w_s))
		return false;
	if (!(strm << d_._e_v))
		return false;
	if (!(strm << d_._e_in_v))
		return false;
	if (!(strm << d_._exp_contra_p))
		return false;
	if (!(strm << d_._tip))
	  return false;
	if (!(strm << d_._pinned)) 
	  return false;
	if (!(strm << d_._bias)) 
	  return false;
	if (!(strm << d_._s_bias)) 
	  return false;
	if (!(strm << d_._pias)) 
	  return false;
	if (!(strm << d_._d_pias)) 
	  return false;
	if (!(strm << d_._s_pias)) 
	  return false;
	if (!(strm << d_._triggered_t)) 
	  return false;
	if (!(strm << d_._comment)) 
	  return false;
	if (!(strm << d_._w)) 
	  return false;

	return true;
 }

bool operator>>(ACE_InputCDR& strm, LFBox& d_)
{
	if (!d_.from_cdr(strm)) 
	  return false;
	if (!(strm >> d_._key)) 
	  return false;
	if (!(strm >> d_._side))
		return false;
	if (!(strm >> d_._s_dp))
		return false;
	if (!(strm >> d_._c_dp))
		return false;
	if (!(strm >> d_._max_dp))
		return false;
	if (!(strm >> d_._min_dp))
		return false;
	if (!(strm >> d_._w_dp))
		return false;
	if (!(strm >> d_._max_corr))
	  return false;
	if (!(strm >> d_._min_corr))
		return false;
	if (!(strm >> d_._end_t))
	  return false;
	if (!(strm >> d_._turned_t)) 
	  return false;
	if (!(strm >> d_._w_l))
		return false;
	if (!(strm >> d_._w_v))
		return false;
	if (!(strm >> d_._w_dv))
		return false;
	if (!(strm >> d_._e_w))
		return false;
	if (!(strm >> d_._w_s))
		return false;
	if (!(strm >> d_._e_v))
		return false;
	if (!(strm >> d_._e_in_v))
		return false;
	if (!(strm >> d_._exp_contra_p))
		return false;
	if (!(strm >> d_._tip))
	  return false;
	if (!(strm >> d_._pinned)) 
	  return false;
	if (!(strm >> d_._bias)) 
	  return false;
	if (!(strm >> d_._s_bias)) 
	  return false;
	if (!(strm >> d_._pias)) 
	  return false;
	if (!(strm >> d_._d_pias)) 
	  return false;
	if (!(strm >> d_._s_pias)) 
	  return false;
	if (!(strm >> d_._triggered_t)) 
	  return false;
	if (!(strm >> d_._comment)) 
	  return false;
	if (!(strm >> d_._w)) 
	  return false;

	return true;
}
//======================================================================
void LFBox::reset(const LFSide* side_, const LFDataPointPtr& dp_s_, double tick_size_, const LFCluster& prev_w_)
{
	_side = side_;
	_s_dp = dp_s_;
	_t = LF::get_t(_s_dp);
	
	_run = BMModel::instance()->run_num();
	_max_corr = 0;
	_min_corr = 0;
	_max_dp = _s_dp;
	_min_dp = _s_dp;
	_c_dp = _s_dp;
	_w_dp = _s_dp;

	_end_t = ACE_Time_Value::zero;
	_turned_t = ACE_Time_Value::zero;

	_w_l = 0;
	_w_v = 0;
	_w_dv = 0;
	_e_w = TCCompare::NaN;
	_w_s = 0;
	_e_v = TCCompare::NaN;
	_e_in_v = TCCompare::NaN;
	_exp_contra_p = TCCompare::NaN;
	_tip.reset();
	_pinned = false;
	_w_changed = false;
	_bias = 0;
	_s_bias = LFBox::UNKNOWN_BIAS;
	_pias = 0;
	_d_pias = 0;
	_s_pias = LFBox::UNKNOWN_BIAS;
	_triggered_t = ACE_Time_Value::zero;
	_comment.clear();
	_w.reset(_side, _max_dp);
	if (valid())
	{
		if (prev_w_.cluster_valid() && prev_w_._side != _side)
			_w_dp = prev_w_._dp;

		_w.set_cluster(_w_dp, tick_size_);
		_w_changed = true;
		if (is_trace())
		{
			LF::model_send(*this, "reset", _trace_t, true, _trace_key);
		}

	}
}

void LFBox::reset_from(const LFBox& from_)
{
	if (is_trace())
	{
		LF::model_send(*this, "reset_from<<", _trace_t, true, _trace_key);
	}
	*this = from_;
	_end_t = ACE_Time_Value::zero;
	_comment.clear();
	_w_changed = true;
	if (is_trace())
	{
		LF::model_send(*this, "reset_from>>", _trace_t, true, _trace_key);
	}
}

void LFBox::reset_side(const LFSide* side_, const LFCluster& prev_w_)
{
	if(!valid() || _side == side_)
	{
		return;
	}
	if (is_trace())
	{
		LF::model_send(*this, "reset_side<<", _trace_t, true, _trace_key);
	}

	_side = side_;
	std::swap(_max_dp, _min_dp);

	_w_dp = _s_dp;
	if (prev_w_.cluster_valid() && prev_w_._side != _side)
		_w_dp = prev_w_._dp;

	update_w();
	if (is_trace())
	{
		LF::model_send(*this, "reset_side>>", _trace_t, true, _trace_key);
	}
}

//======================================================================
void LFBox::update_w()
{
	if (!_w.cluster_valid())
		return;
	if (_w._dp->equal(*_max_dp) && _w._cluster_dp->equal(*_w_dp))
		return;
	double tick_size = _w._tick_size;
	_w.reset_cluster(_side, _max_dp, _w_dp, tick_size);
	_w_changed = true;
}
//======================================================================
void LFBox::update_current(const LFDataPointPtr& dp_)
{
	if (!valid() || !_c_dp->older(*dp_))
		return;

	if (is_trace())
	{
		LF::model_send(*this, "update_current<<", _trace_t, true, _trace_key);
	}
	_c_dp = dp_;
	
	if (_side->b_diff(_c_dp->_p, _max_dp->_p) <= 0)
		_max_dp = _c_dp;
	if (_side->b_diff(_c_dp->_p, _min_dp->_p) >= 0)
		_min_dp = _c_dp;

	update_w();
	if (is_trace())
	{
		LF::model_send(*this, "update_current>>", _trace_t, true, _trace_key);
	}

}
//======================================================================
void LFBox::merge(const LFBox& b_, bool keep_bias_)
{
	if (!valid())
		return;
	if (!_s_dp->older(*b_._s_dp))
	{
		merge_same(b_);
		return;
	}
	if (is_trace())
	{
		LF::model_send(*this, "merge<<", _trace_t, true, _trace_key);
		LF::model_send(b_, "merge_b_", _trace_t, true, _trace_key);
	}

	_c_dp = b_._c_dp;

	LFDataPointPtr max_dp = b_._side == _side ? b_._max_dp : b_._min_dp;
	LFDataPointPtr min_dp = b_._side == _side ? b_._min_dp : b_._max_dp;
	if (_side->b_diff(max_dp->_p, _max_dp->_p) <= 0)
	{
		_max_dp = max_dp;
	}
	if (_side->b_diff(min_dp->_p, _min_dp->_p) >= 0)
		_min_dp = min_dp;

	_max_corr = std::max(_max_corr, b_._max_corr);
	_min_corr = std::min(_min_corr, b_._min_corr);

	if(!keep_bias_)
	{
		_bias = b_._bias;
		_pias = b_._pias;
		_d_pias = b_._d_pias;
	}
	_triggered_t = b_._triggered_t;

	update_w();
	if (is_trace())
	{
		LF::model_send(*this, "merge>>", _trace_t, true, _trace_key);
	}

}

void LFBox::merge_same(const LFBox& b_)
{
	if (is_trace())
	{
		LF::model_send(*this, "merge_same<<", _trace_t, true, _trace_key);
		LF::model_send(b_, "merge_same_b_", _trace_t, true, _trace_key);
	}
	_c_dp = b_._c_dp;

	LFDataPointPtr max_dp = b_._side == _side ? b_._max_dp : b_._min_dp;
	LFDataPointPtr min_dp = b_._side == _side ? b_._min_dp : b_._max_dp;

	if (_s_dp->older(*max_dp))
		_max_dp = max_dp;
	else if (_side->b_diff(_c_dp->_p, _s_dp->_p) <= 0)
		_max_dp = _c_dp;
	else
		_max_dp = _s_dp;

	if (_s_dp->older(*min_dp))
		_min_dp = min_dp;
	else if (_side->b_diff(_c_dp->_p, _s_dp->_p) >= 0)
		_min_dp = _c_dp;
	else
		_min_dp = _s_dp;

	_max_corr = b_._max_corr;
	_min_corr = b_._min_corr;
	//
	_bias = b_._bias;
	_pias = b_._pias;
	_d_pias = b_._d_pias;
	_triggered_t = b_._triggered_t;
		
	update_w();
	if (is_trace())
	{
		LF::model_send(*this, "merge_same>>", _trace_t, true, _trace_key);
	}
}
//======================================================================
void LFBox::split_min(LFBox& m_, double tick_size_)
{
	m_.reset();
	if(!valid())
		return;

	if (is_trace())
	{
		LF::model_send(*this, "split_min<<", _trace_t, true, _trace_key);
	}

	m_.reset(_side->contra_s(), _s_dp, tick_size_);
	m_._max_dp = _min_dp;
	m_._c_dp = _min_dp;
	m_.update_w();

	if(_w_dp->equal(*_s_dp))
		_w_dp = _min_dp;
	_s_dp = _min_dp;
	_t = LF::get_t(_s_dp);

	if(_max_dp->older(*_min_dp))
	{
		m_._min_dp = _max_dp;
		_max_dp = _c_dp;
	}
	update_w();

	if (is_trace())
	{
		LF::model_send(*this, "split_min>>", _trace_t, true, _trace_key);
		LF::model_send(m_, "split_min_m_", _trace_t, true, _trace_key);
	}

}
//======================================================================
void LFBox::cut_max()
{
	if(!valid())
		return;

	if (is_trace())
	{
		LF::model_send(*this, "cut_max<<", _trace_t, true, _trace_key);
	}
	_c_dp = _max_dp;

	if (_max_dp->older(*_min_dp))
	{
		_min_dp = _s_dp;
	}
	update_w();

	if (is_trace())
	{
		LF::model_send(*this, "cut_max>>", _trace_t, true, _trace_key);
	}
}

//======================================================================
void LFBox::reset_w()
{
	_w_l = 0;
	_w_v = 0;
	_w_dv = 0;
	_w_s = 0;
}
//======================================================================
void LFBox::reset_exp()
{
	_exp_contra_p = TCCompare::NaN;
	_e_w = TCCompare::NaN;
	_e_v = TCCompare::NaN;
	_e_in_v = TCCompare::NaN;
}

//======================================================================
void LFBox::set_w(const LFBox& prev_, const LFMarketModel& model_)
{
	if(!valid())
	{
		reset_w();
		reset_exp();
		return;
	}
	if (!prev_.valid())
	{
		_w_l = _w._cluster_l;
		_w_v = _w._cluster_v;
		_w_dv = _w._cluster_v;
		_w_s = _w_l <= 0 ? 0 : _w_dv / _w_l;
		return;
	}

	_w_dp = prev_._w._dp;
	update_w();

	_w_l = _w._cluster_l + prev_._w._cluster_l;
	_w_v = std::max(_w._cluster_v, prev_._w._cluster_v);
	_w_dv = _w._cluster_v - prev_._w._cluster_v;
	_w_s = _w_l <= 0 ? 0 : _w_dv / _w_l;
	
}
//======================================================================




