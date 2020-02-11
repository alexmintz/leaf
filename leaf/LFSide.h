/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/

#ifndef _LF_SIDE_H_
#define _LF_SIDE_H_

#include "znet/ZNet.h"
#include "leaf/LFMarketData.h"

//======================================================================
namespace LF
{
	inline double sign(double m_)
	{
		return (m_ < 0.) ? -1.: 1.;
	}
	inline double zsign(double m_)
	{
		return (m_ == 0.) ? 0: sign(m_);
	}
};
//======================================================================
class LEAF_Export LFSide
{
public:
	LFSide() {}
	virtual ~LFSide() {}

	virtual LF::Side value() const = 0;
	LF::Side c_value() const { return contra(value()); }

	virtual void fill_contra(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const = 0;
	virtual void fill_same(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const = 0;

	virtual void fill_contra(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const = 0;
	virtual void fill_same(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const = 0;

	virtual double get_contra(double bid_, double ask_) const = 0;
	virtual double get_same(double bid_, double ask_) const = 0;

	virtual double contra_p(const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const 
	{ 
		TC_pqp qp; 
		fill_contra(qp, md_, adjust_0_q_, scale_); 
		return qp._p; 
	}
	virtual double same_p(const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const 
	{ 
		TC_pqp qp; 
		fill_same(qp, md_, adjust_0_q_, scale_); 
		return qp._p; 
	}

	// diff > 0, if p1 better than p2
	virtual double b_diff(double p_, double point_p_) const = 0;
	// return true if p1 better or equal of p2
	virtual bool b_or_eq(double p_, double point_p_) const;
	// round to closest tick depending on side
	virtual double b_round(double p_, double tick_) const = 0;

	// return sign depending of position side 
	virtual double sign() const = 0;

	// value in ticks from point_p_
	double v(double p_, double point_p_, double tick_size_) const { return b_diff(p_, point_p_) / tick_size_; }

	double p_from_v(double point_p_, double v_, double tick_size_) const { return point_p_ - sign()*v_*tick_size_; }
	
	double zsign(double m_) const
	{
		return LF::zsign(m_)*sign();
	}
	// return a const pointer to a contra side
	virtual const LFSide* contra_s() const = 0;

	// fill integral part to iptr, return == 0 if p_ is on tick boundary
	static double mod_tick(double p_, double tick_, double* ticks_);

	// factory method
	static const LFSide* s(LF::Side v_);

	static LF::Side by_sign(double sgn_);
	static const LFSide* s_by_sign(double sgn_);

	static LF::Side contra(LF::Side v_);
	static LFSide* make_side(LF::Side v_);

	static double adjust_q(double q_, bool adjust_0_q_, double scale_ = 1.)
	{
		if(adjust_0_q_) 
			q_ += 1.;
		return q_ * scale_;
	}

};

//======================================================================
class LEAF_Export LFSideBuy : public LFSide
{
public:
	LFSideBuy() {}
	virtual ~LFSideBuy() {}

	virtual LF::Side value() const { return LF::s_BUY; }

	virtual void fill_contra(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;
	virtual void fill_same(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;

	virtual void fill_contra(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;
	virtual void fill_same(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;

	virtual double get_contra(double bid_, double ask_) const { return ask_; }
	virtual double get_same(double bid_, double ask_) const { return bid_; }

	virtual double b_diff(double p_, double point_p_) const;
	virtual double b_round(double p_, double tick_) const { return round(p_, tick_); }
	
	virtual double sign() const { return 1.; }

	virtual const LFSide* contra_s() const;

	static double round(double p_, double tick_);

	static const LFSideBuy _i;
};

//======================================================================
class LEAF_Export LFSideSell : public LFSide
{
public:
	LFSideSell() {}
	virtual ~LFSideSell() {}

	virtual LF::Side value() const { return LF::s_SELL; }

	virtual void fill_contra(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;
	virtual void fill_same(TC_pqp& pqp_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;

	virtual void fill_contra(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;
	virtual void fill_same(std::vector<TC_pqp>& pqpv_, const LFDepthData& md_, bool adjust_0_q_ = false, double scale_ = 1.) const;


	virtual double get_contra(double bid_, double ask_) const { return bid_; }
	virtual double get_same(double bid_, double ask_) const { return ask_; }

	virtual double b_diff(double p_, double point_p_) const;
	virtual double b_round(double p_, double tick_) const { return round(p_, tick_); }
	virtual double sign() const { return -1.; }

	static double round(double p_, double tick_);
	static const LFSideSell _i;

	virtual const LFSide* contra_s() const;
};


//======================================================================
LEAF_Export bool operator<<(ACE_OutputCDR& strm, const LFSide* s);
LEAF_Export bool operator>>(ACE_InputCDR& strm, const LFSide*& s);
LEAF_Export std::ostream& operator<<(std::ostream& os, const LFSide* s);

//======================================================================
namespace LF
{
	inline bool same_side(const LFSide* s1_, const LFSide* s2_)
	{
		return s1_ != 0 && s2_ != 0 && s1_->value() == s2_->value();
	}
};

//======================================================================
struct LEAF_Export LF_MD
{
	TC_pqp	_c;		// contra side
	TC_pqp	_s;		// same side

	void adjust_cross(const LFSide& v_)
	{
		if(_s.good() && _c.good() && v_.b_diff(_c._p, _s._p) > 0)
		{
			_s.reset();
			_c.reset();
		}
	}

	LEAF_Export friend ostream& operator<<(ostream& os_, const LF_MD& md_)
	{
		os_ << "[s=" << md_._s << ", c=" << md_._c << "]";
		return os_;
	}

};

namespace LF
{
	//======================================================================
	template<typename T>
	class Pair
	{
	public:
		Pair(const T& buy_, const T& sell_)
		{
			reset(buy_, sell_);
		}

		void reset(const T& buy_, const T& sell_)
		{
			_v.clear();
			_v.push_back(buy_);
			_v.push_back(sell_);
		}
		bool operator==(const Pair<T>& s_) const { return _v[0] == s_._v[0] && _v[1] == s_._v[1]; }
		const T& operator[](Side s_) const
		{
			return _v[size_t(s_) - 1];
		}
		T& operator[](Side s_)
		{
			return _v[size_t(s_) - 1];
		}
		const T& operator[](size_t i) const
		{
			return _v[i];
		}
		T& operator[](size_t i)
		{
			return _v[i];
		}

		size_t size() const { return _v.size(); }

	protected:
		std::vector<T> _v;
	};
	//======================================================================
	template<typename T>
	class RefPair
	{
	public:
		RefPair(T* buy_, T* sell_)
		{
			_v.push_back(buy_);
			_v.push_back(sell_);
		}
		bool valid() { return (size() == 2 && _v[0] != 0 && _v[1] != 0);}
		bool operator==(const Pair<T>& s_) const { return *_v[0] == *s_._v[0] && *_v[1] == *s_._v[1]; }
		const T& operator[](Side s_) const
		{
			return *_v[size_t(s_) - 1];
		}
		T& operator[](Side s_)
		{
			return *_v[size_t(s_) - 1];
		}
		const T& operator[](size_t i) const
		{
			return *_v[i];
		}
		T& operator[](size_t i)
		{
			return *_v[i];
		}

		size_t size() const { return _v.size(); }

	protected:
		std::vector<T*> _v;
	};
}
//======================================================================
#endif // _LF_SIDE_H_
