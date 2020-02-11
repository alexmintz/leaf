/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFCluster.h"
#include <cstdlib> 
#include <set>
#include <cfloat>

//======================================================================
struct LFClusterHeader
{
	LFClusterHeader()
		: _h(LFPoint::header()),
		_sum_h(LFPoint::summary_header())
	{
		_sum_h += ",L,V,S,VOL,TRAN,DEN,FREQ";
		_h += LFDataPoint::summary_header();
		_h += ",L,V,S,VOL,TRAN,DEN,FREQ,PATTERN";
	}
	std::string _h;
	std::string _sum_h;
};
//======================================================================
//======================================================================
//======================================================================
//======================================================================
LFCluster::LFCluster(const std::string& key_)
 :LFPoint(key_),
	_tick_size(0),
	_cluster_l(0),
	_cluster_v(0),
	_cluster_s(0),
	_cluster_vol(0),
	_cluster_tran(0),
	_cluster_density(0),
	_cluster_frequency(0)
{
}
//======================================================================
LFCluster::~LFCluster()
{
}
//======================================================================
const char* LFCluster::summary_header()
{
	return ACE_Singleton <LFClusterHeader, ACE_Thread_Mutex>::instance()->_sum_h.c_str();
}
//======================================================================
void LFCluster::summary_to_stream(std::ostream& os) const
{
	LFPoint::summary_to_stream(os);
	os << "," << _cluster_l ;
	os << "," << _cluster_v ;
	os << "," << _cluster_s ;
	os << "," << _cluster_vol;
	os << "," << _cluster_tran;
	os << "," << _cluster_density;
	os << "," << _cluster_frequency;
}

//======================================================================
const char* LFCluster::header()
{
	return ACE_Singleton <LFClusterHeader, ACE_Thread_Mutex>::instance()->_h.c_str();
}
//======================================================================
void LFCluster::body_to_stream(std::ostream& os) const
{
	LFPoint::body_to_stream(os);
	LF::summary_to_stream(os, _cluster_dp);
	os << "," << _cluster_l ;
	os << "," << _cluster_v ;
	os << "," << _cluster_s ;
	os << "," << _cluster_vol;
	os << "," << _cluster_tran;
	os << "," << _cluster_density;
	os << "," << _cluster_frequency;
	os << "," << _pattern.to_alfa();
		
}
//======================================================================
std::ostream& operator<<(std::ostream& os, const LFCluster& d_)
{
	d_.to_stream(os);

	d_.body_to_stream(os);
	return os;
}
//======================================================================
 bool operator<<(ACE_OutputCDR& strm, const LFCluster& d_)
{
	const LFPoint& p = d_;
	if (!(strm << p)) 
	  return false;

	if (!(strm << d_._cluster_dp)) 
	  return false;
	if (!(strm << d_._cluster_l)) 
	  return false;
	if (!(strm << d_._cluster_v)) 
	  return false;
	if (!(strm << d_._cluster_s)) 
	  return false;
	if (!(strm << d_._cluster_vol))
		return false;
	if (!(strm << d_._cluster_tran))
		return false;
	if (!(strm << d_._cluster_density))
		return false;
	if (!(strm << d_._cluster_frequency))
		return false;
	if (!(strm << d_._pattern.to_ulong()))
	  return false;
	if (!(strm << d_._te_pattern.to_ulong())) 
	  return false;
	return true;
 }

bool operator>>(ACE_InputCDR& strm, LFCluster& d_)
{
	LFPoint& p = d_;
	if (!(strm >> p)) 
	  return false;
	if (!(strm >> d_._cluster_dp)) 
	  return false;
	if (!(strm >> d_._cluster_l)) 
	  return false;
	if (!(strm >> d_._cluster_v)) 
	  return false;
	if (!(strm >> d_._cluster_s)) 
	  return false;
	if (!(strm >> d_._cluster_vol))
		return false;
	if (!(strm >> d_._cluster_tran))
		return false;
	if (!(strm >> d_._cluster_density))
		return false;
	if (!(strm >> d_._cluster_frequency))
		return false;
	unsigned long pat = 0;
	if (!(strm >> pat)) 
	  return false;
	d_._pattern = LF::Pattern(pat);
	unsigned long te_pat = 0;
	if (!(strm >> te_pat)) 
	  return false;
	d_._te_pattern = LF::TEPattern(te_pat);

	return true;
}
//======================================================================
void LFCluster::reset(const LFSide* side_, const LFDataPointPtr& dp_)
{
	LFPoint::reset(side_, dp_);
	_cluster_dp.reset();
	clear_cluster();
	_pattern.reset();
	_te_pattern.reset();
}
void LFCluster::reset(const LFPoint& point_)
{
	reset(point_._side, point_._dp);
}
//======================================================================
void LFCluster::reset_cluster(const LFSide* side_, const LFDataPointPtr& dp_, const LFDataPointPtr& c_dp_, double tick_size_)
{
	reset(side_, dp_);
	set_cluster(c_dp_, tick_size_);
}
//======================================================================
void LFCluster::set_cluster(const LFDataPointPtr& c_dp_, double tick_size_)
{
	_tick_size = tick_size_;
	_cluster_dp = c_dp_;
	calc_cluster();
}
//======================================================================
void LFCluster::set_cluster(const LFPoint& c_point_, double tick_size_)
{
	set_cluster(c_point_._dp, tick_size_);
}

//======================================================================
void LFCluster::calc_cluster()
{
	clear_cluster();
	if (!cluster_valid())
		return;

	_cluster_v = value(_cluster_dp->_p, _tick_size);
	_cluster_l = (_dp->_t <= _cluster_dp->_t) ? 0. : LF::length_from_t(_dp->_t - _cluster_dp->_t);
	_cluster_s = _cluster_l == 0 ? 0. : _cluster_v / _cluster_l;
	_cluster_vol = LF::t_volume_i(_cluster_dp, _dp);
	_cluster_tran = LF::t_tran_i(_cluster_dp, _dp);
	//	_cluster_density = _cluster_s <= 0 ? 0 : _cluster_vol / _cluster_s;
	_cluster_density = _cluster_v <= 0 ? 0 : _cluster_vol / _cluster_v;
	_cluster_frequency = _cluster_l <= 0.1 ? 0 : _cluster_tran / _cluster_l;
}
//======================================================================
void LFCluster::clear_cluster()
{
	_cluster_l = 0.;
	_cluster_v = 0.;
	_cluster_s = 0.;
	_cluster_vol = 0;
	_cluster_tran = 0;
	_cluster_density = 0;
	_cluster_frequency = 0;
}
//======================================================================
double LFCluster::continuity(const LFCluster& bnc_, const LFMarketModel& _model) const
{
	double cnty = 0;
	if (!t_in(bnc_._cluster_dp))
		return cnty;
	LFCluster v0;
	LFCluster v1;
	v0.reset_cluster(_side, bnc_._cluster_dp, _cluster_dp, _tick_size);

	if (bnc_._dp->older(*_dp))
	{
		v1.reset_cluster(_side, _dp,  bnc_._dp, _tick_size);
		if (_model._crl_continuity_weighted)
		{
			cnty = _cluster_v* 2. / (2.*v0._cluster_v + 3.*bnc_._cluster_v + v1._cluster_v);
		}
		else
		{
			cnty = _cluster_v / (v0._cluster_v + bnc_._cluster_v + v1._cluster_v);
		}
	}
	else
	{
		cnty = _cluster_v / (v0._cluster_v + 2 * bnc_._cluster_v);
	}

	return cnty;
}
//======================================================================
//======================================================================
LFBounce::LFBounce()
	: _v_pct(0), _l_pct(0), _vol_pct(0)
{}
LFBounce::~LFBounce()
{
}

void LFBounce::reset(const LFSide* side_, const LFDataPointPtr& dp_)
{
	LFCluster::reset(side_, dp_);
	_v_pct = 0;
	_l_pct = 0;
	_vol_pct = 0;
}

void LFBounce::set_bounce(const LFCluster& peak_)
{
	if (!cluster_valid() || !peak_.cluster_valid())
		return;

	_v_pct = _cluster_v*100. / peak_._cluster_v;
	_l_pct = _cluster_l*100. / peak_._cluster_l;
	_vol_pct = peak_._cluster_vol <= 0.? 0 : _cluster_vol*100. / peak_._cluster_vol;
}
//======================================================================

