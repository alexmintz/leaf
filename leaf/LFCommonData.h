/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef LF_COMMON_DATA_H
#define LF_COMMON_DATA_H

#include "znet/ZNet.h"
#include "leaf/LFMarketData.h"
#include "leaf/LFSide.h"
#include "leaf/LEAF_Export.h"
#include <list>
#include <bitset>
#include <cmath>

//======================================================================
namespace LF
{
	inline double round(double v_, double prec_ = 1.)
	{
		if (prec_ <= 0.)
			return v_;
		double rv = (long)(v_ / prec_ + 0.5);
		return prec_*rv;
	}
	LEAF_Export double length_from_msec(double msec_);
	LEAF_Export double length_from_t(const ACE_Time_Value& t_);
	LEAF_Export ACE_Time_Value t_from_length(double l_);

	LEAF_Export double ratio_in_range(double v_, double from_, double to_);
	LEAF_Export double value_in_range(double ratio_, double from_, double to_);

	inline double diff_from_ratio(double r_) 
	{
		if(r_ <= 0.)
			return 0.;
		return r_ > 1.? (r_- 1.)*100. : (1. - 1./r_)*100.;
	}

	template <typename T>
	T by_ind(const std::vector<T>& v_, double ind_, bool inter_ = false)
	{
		if(v_.empty())
			return T();
		if (ind_ <= 0)
			return v_[0];

		double indi = 0;
		double ratio = ::modf(ind_, &indi);
		size_t i = (size_t)indi;
		if (ratio == 0 && i < v_.size())
			return v_[i];
		if (i + 1 < v_.size())
			return inter_ ? (v_[i] + value_in_range(ratio, v_[i], v_[i + 1])) : v_[i + 1];
		return v_.back();
	}

	template <typename T>
	double by_val(const std::vector<T>& v_, T val_, bool inter_ = false)
	{
		if (v_.empty())
			return 0;

		for (size_t i = 0; i < v_.size(); ++i)
		{
			if (val_ < v_[i])
			{
				if(i == 0 || !inter_)
					return (double)i;
				return (double)(i - 1) + ratio_in_range(val_, v_[i - 1], v_[i]);
			}
		}

		return (double)v_.size();
	}
	//======================================================================
	template<typename S>
	void body_to_stream(std::ostream& os, const Z::smart_ptr<S>& s_)
	{
		if (s_.get() == 0)
		{
			S s;
			s.body_to_stream(os);
		}
		else
		{
			s_->body_to_stream(os);
		}
	}
	template<typename S>
	void summary_to_stream(std::ostream& os, const Z::smart_ptr<S>& s_)
	{
		if (s_.get() == 0)
		{
			S s;
			s.summary_to_stream(os);
		}
		else
		{
			s_->summary_to_stream(os);
		}
	}

	template <typename S>
	void model_send(const S& s_, const std::string& comment_, const ACE_Time_Value& t_, bool out_file_, const std::string& key_ = "")
	{
		if(!out_file_)
			return;
		S* s(new S(s_));
		s->_t = t_;
		if(!comment_.empty())
			s->_comment = comment_;
		if(!key_.empty())
			s->set_key(key_);

		BMModel::instance()->send(s);
	}

	enum PatternBit
	{
		pb_ACCELERATED, pb_COUNTINUOUS, pb_STRAIGHT, pb_INTERRUPTED,
		pb_LAST
	};
	enum TEPatternBit
	{
		te_ROLL_ENTER,

		te_LAST
	};

	class LEAF_Export Pattern : public std::bitset<pb_LAST>
	{
	public:
		typedef std::bitset<pb_LAST> Papa;
		Pattern(unsigned long from_ = 0) : Papa(from_) {}
		
		std::string to_alfa() const;
	};
	class LEAF_Export TEPattern : public std::bitset<te_LAST>
	{
	public:
		typedef std::bitset<te_LAST> Papa;
		TEPattern(unsigned long from_ = 0) : Papa(from_) {}
		
		std::string to_alfa() const;
	};

	enum MatchPatternBit
	{
		mp_SUPPORT,
		mp_NO_SUPPORT,
		mp_PIAS,
		mp_DENSITY,
		mp_SLOPE,
		mp_A_0,
		mp_A_1,
		mp_A_2,
		mp_P_IN,
		mp_P_MIN,
		mp_P_WEAK,
		mp_P_BUST,
		mp_N_MAX,
		mp_N_WEAK,
		mp_N_BUST,
		mp_B_WEAK,
		mp_B_BUST,
		mp_T_WEAK,
		mp_T_BUST,
		mp_LAST
	};
	class LEAF_Export MatchPatternMask : public std::bitset<mp_LAST>
	{
	public:
		typedef std::bitset<mp_LAST> Papa;
		MatchPatternMask(unsigned long from_ = 0) : Papa(from_) {}
		
		std::string to_alfa() const;
	};
	enum MatchBit
	{
		mb_CHARGE,
		mb_CHARGE_DIFF,
		mb_T_DIFF,
		mb_ROLL,
		mb_BLOCK,
		mb_NO_EXP,
		mb_VOLUME,
		mb_IN_VOLUME,
		mb_PIAS,
		mb_RISK,
		mb_TNT,
		mb_INBAND,
		mb_LAST
	};
	class LEAF_Export MatchMask : public std::bitset<mb_LAST>
	{
	public:
		typedef std::bitset<mb_LAST> Papa;
		MatchMask(unsigned long from_ = 0) : Papa(from_) {}
		
		std::string to_alfa() const;
	};
	enum MatchExecBit
	{
		me_CUTLOSS,
		me_SECONDARY,
		me_SINGLEENTRY,
		me_PROFIT,
		me_CROSS,
		me_LAST
	};
	class LEAF_Export MatchExecMask : public std::bitset<me_LAST>
	{
	public:
		typedef std::bitset<me_LAST> Papa;
		MatchExecMask(unsigned long from_ = 0) : Papa(from_) {}
		
		std::string to_alfa() const;
	};

	//======================================================================
	struct LEAF_Export Id
	{
		Id() :
			_scale(-1),
			_roll(-1),
			_child(-1),
			_top_scale(-1)
		{}

		void reset();

		bool valid() const { return _scale >= 0 && _roll >= 0; }

		std::string to_alfa() const;

		long _scale;
		long _roll;
		long _child;
		long _top_scale;
	};

	typedef std::vector<Id> IdVec;
};

//======================================================================
LEAF_Export bool operator<<(ACE_OutputCDR& strm, const LF::Id& d_);
LEAF_Export bool operator>>(ACE_InputCDR& strm, LF::Id& d_);
//======================================================================
template<class T>
bool operator<<(ACE_OutputCDR& strm, const Z::smart_ptr<T>& s)
{
	ACE_CDR::ULong exists = (ACE_CDR::ULong)(s.get() == 0)?0:1;
	if (!(strm << exists))
		return false;
	if(exists > 0)
	{
		if (!(strm << *s))
			return false;
	}
	return true;
}
template<class T>
bool operator>>(ACE_InputCDR& strm, Z::smart_ptr<T>& s)
{
	s.reset(new T);
	ACE_CDR::ULong exists = 0;
	if (!(strm >> exists))
		return false;

	if (exists > 0)
	{
		if(!(strm >> *s))
			return false;
	}
	return true;
}
//======================================================================
typedef std::pair<long, long> LFPeakId;
typedef std::pair<bool,bool> LFEnterExit;

//======================================================================
struct LEAF_Export LFPeakAction : public LF::Pair<LFEnterExit>
{
	typedef LF::Pair<LFEnterExit> Papa;
	LFPeakAction();

	void reset();

	void set(const LFSide* side_, double q_, bool a_ = true);
	bool test(const LFSide* side_, double q_) const;

	std::string to_string() const;

	bool _need_exit;
	//AK TODO to_ulong, constructor from ulong
};
//======================================================================
struct LEAF_Export LFCutLoss : public LF::Pair<double>
{
	typedef LF::Pair<double> Papa;
	LFCutLoss();

	void reset();
	void reset(const LFSide* side_);

	void set(const LFSide* side_, double p_, bool force_ = false);
	bool cross(const LFSide* side_, double p_) const;
	bool cross_reset(const LFSide* side_, double p_);
	const LFSide* cross_reset(double p_);

	std::string to_string() const;
	LF::PositionAction _action;
	double _pre_cl_ticks;
};
//======================================================================
#endif /*LF_COMMON_DATA_H*/
