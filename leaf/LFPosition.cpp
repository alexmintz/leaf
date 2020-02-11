/*
*
*	© Copyright 2012 by TAF Co. All rights reserved.
*
*/


#include "leaf/LFPosition.h"
#include "leaf/LFSide.h"
#include "znet/TCIdGenerator.h"

//======================================================================
LFPositionImpl::LFPositionImpl(const std::string& instr_, long strategy_id_)
:	_d(strategy_id_, instr_),
	_cqg_InstrumentID(-1),
	_cqg_AccountID(LFSecurityMaster::instance()->account_id(strategy_id_))//Z::get_setting_integer("LFStrategy::cqg_account_id", -1))
{
}
//======================================================================
LFPositionImpl::~LFPositionImpl()
{
}
//======================================================================
void LFPositionImpl::reset()
{
	_d.reset();
	_md = LFDepthData();
	_cqg_InstrumentID = -1;
	_input_time = ACE_Time_Value::zero;
	_orders.clear();
}
//======================================================================
void LFPositionImpl::reset(double open_p_, double open_q_, double realized_pnl_)
{
	Z::report_info("LFPositionImpl -- reset with open = %f@%f, realized_pnl=%f",
					open_q_, open_p_, realized_pnl_);
	reset();
	bool updated = false;
	if(realized_pnl_ != 0.)
	{
		_d._realized_pnl = realized_pnl_;
		updated = true;
	}
	if(open_p_ != 0. && open_q_ != 0.)
	{
		if(open_q_ > 0.)
		{
			_d._side = LF::s_BUY;
			_d._open.reset(open_p_, open_q_);
		}
		else
		{
			_d._side = LF::s_SELL;
			_d._open.reset(open_p_, -open_q_);
		}
		calc_in_progress();
		updated = true;
	}
	if(updated)
		BMModel::instance()->send(new LFPositionData(_d));
}
//======================================================================
void LFPositionImpl::set_market_data(const LFDepthData& md_)
{
	_md = md_;
	_cqg_InstrumentID = _md._cqg_InstrumentID;
	_d.mark_to_market(_md);
}
//======================================================================
void LFPositionImpl::calc_in_progress()
{
	_d._opening.reset();
	_d._closing.reset();

	if(_d._open._q == 0)
		_d._side = LF::s_UNKNOWN;

	for(LFOrderMap::iterator i = _orders.begin(); i != _orders.end(); ++i)
	{
		LFOrderUpdate& o = *(*i).second;
		if(_d._side == LF::s_UNKNOWN)
			_d._side = o._order_side;
		TC_pqp qp;
		if(o._order_type == LF::otMarket)
			qp.reset(LFSide::s(o._order_side)->contra_p(_md), o._remain_q);
		else if(o._order_type == LF::otLimit)
			qp.reset(o._limit_p, o._remain_q);

		if(o._order_side == _d._side)
			_d._opening += qp;
		else
			_d._closing += qp;

	}
	_d._p_remain_q = _d._open._q - _d._closing._q;
	_d._p_open = _d._open + _d._opening;
}
//======================================================================
void LFPositionImpl::book_order(LFNewOrder* r_)
{
	auto_ptr<LFNewOrder> r(r_);
	LFOrderUpdatePtr o(new LFOrderUpdate(*r));

	double book_p = 0;
	if(o->_order_type != LF::otLimit)
		book_p = LFSide::s(o->_order_side)->contra_p(_md);
	else
		book_p = o->_limit_p;

	// make fill
	auto_ptr<LFOrderFill> f(new LFOrderFill);
	f->_t = o->get_t();
	f->_strategy_key = o->_strategy_key;
	f->_order_id = o->_order_id;
	f->_strategy_id = o->_strategy_id;
	f->_sub_strategy_id = o->_sub_strategy_id;
	f->_instr = o->_instr;
	f->_cqg_InstrumentID = o->_cqg_InstrumentID;
	f->_fill.reset(book_p, o->_q);
	f->_fill_id = TCIdGenerator::instance()->sid("Fill");
	f->_side = o->_order_side;

	// update position
	_d.add_fill(*f);
	calc_in_progress();
	_d.mark_to_market(_md);
	// update order status
	o->_order_status = LF::osFilled;
	o->_filled_q = (long)f->_fill._q;
	o->_avg_fill_p = f->_fill._p;
	o->_remain_q = 0;
	o->_fills.push_back(*f);

	// for the record
	BMModel::instance()->send(f.release());
	BMModel::instance()->send(new LFOrderUpdate(*o));
	BMModel::instance()->send(new LFPositionData(_d));
}
//======================================================================
long LFPositionImpl::new_order(LF::Side side_, double qty_, LF::OrderType type_, long sub_strategy_id_, double limit_price_, double stop_price_)
{
	if (BMModel::instance()->is_bootstrap())
	{
		Z::report_info("ORDER -- new_order -- IGNORED in bootstrap mode");
		return 0;
	}
	if (!_md.is_valid())
		z_throw("LFPositionImpl::new_order - no market data");

	long id = TCIdGenerator::instance()->id("Order");
	auto_ptr<LFNewOrder> r(new LFNewOrder);
//	r->_t = _md.get_t();
	r->_t = _input_time;

	r->_strategy_key = _d._strategy_key;
	r->_order_id = id;
	r->_strategy_id = _d._strategy_id;
	r->_sub_strategy_id = sub_strategy_id_;
	r->_instr = _d._instr;
	r->_cqg_InstrumentID = _cqg_InstrumentID;
	r->_cqg_AccountID = _cqg_AccountID;
	r->_order_type = type_;
	r->_order_side = side_;
	r->_q = (long)qty_;
	r->_limit_p = limit_price_;
	r->_stop_p = stop_price_;

	if(r->_sub_strategy_id < 0)
		book_order(r.release());
	else
		place_order(r.release());
	return id;
}
void LFPositionImpl::cancel_order(long order_id_)
{
	auto_ptr<LFCancelOrder> r(new LFCancelOrder);

	r->_strategy_id = _d._strategy_id;
	r->_instr = _d._instr;
	r->_cqg_InstrumentID = _cqg_InstrumentID;
	r->_cqg_AccountID = _cqg_AccountID;
	r->_order_id = order_id_;
	if(order_id_ != 0)
	{
		LFOrderMap::iterator i = _orders.find(order_id_);
		if(i != _orders.end())
		{
			r->_cqg_GWOrderID = (*i).second->_cqg_GWOrderID;
			r->_cqg_OriginalOrderId = (*i).second->_cqg_OriginalOrderId;
		}
		else
			Z::report_error("ORDER -- cancel_order -- order <%d> was not found among open orders", order_id_);
	}
	cancel_order(r.release());
}
//======================================================================
//======================================================================
LFLifePositionImpl::LFLifePositionImpl(const std::string& instr_, long strategy_id_, BMStrategy& strategy_)
	: LFPositionImpl(instr_, strategy_id_), _strategy(strategy_)
{
	if (_cqg_AccountID <= 0)
		z_throw_abort("LFLifePositionImpl -- cqg_account_id is not set");
}
LFLifePositionImpl::~LFLifePositionImpl()
{
}
void LFLifePositionImpl::order_update(LFOrderUpdatePtr* update_)
{
	auto_ptr<LFOrderUpdatePtr> op(update_);
	LFOrderUpdatePtr o = *update_;
	if (o->_cqg_AccountID != _cqg_AccountID)
		return;
	if(o->_sub_strategy_id < 0)
	{
		// booking
		return;
	}
	LFOrderMap::iterator i = _orders.find(o->_order_id);
	if (i != _orders.end())
	{
		if (o->_order_status == LF::osRejectGW
			|| o->_order_status == LF::osInTransitTimeout
			|| o->_order_status == LF::osRejectFCM
			|| o->_order_status == LF::osExpired
			|| o->_order_status == LF::osCanceled
			|| o->_order_status == LF::osFilled)
		{
			Z::report_info("order_update -- is not active -- %s", o->as_string().c_str());
			_orders.erase(o->_order_id);
		}
		else
		{
			Z::report_info("order_update -- active -- %s", o->as_string().c_str());
			_orders[o->_order_id] = o;
		}
	}
	for(size_t i = 0; i < o->_fills.size(); ++i)
	{
		_d.add_fill(o->_fills[i]);
	}

	calc_in_progress();
	_d.mark_to_market(_md);
	BMModel::instance()->send(new LFPositionData(_d));

}
//void LFLifePositionImpl::order_fill(LFOrderFillPtr* fill_)
//{
//	auto_ptr<LFOrderFillPtr> fp(fill_);
//
//	// update position
//	LFOrderFillPtr f = *fill_;
//	_d.add_fill(*f);
//	_d.mark_to_market(_md);
//
////	calc_in_progress();
//	// for the record
//	BMModel::instance()->send(new LFPositionData(_d));
//}
void LFLifePositionImpl::place_order(LFNewOrder* r_)
{
	auto_ptr<LFNewOrder> r(r_);
	r->_t = TCTimestamp::stamp();
	LFOrderUpdatePtr o(new LFOrderUpdate(*r));
	Z::report_info("place_order -- %s",  o->as_string().c_str() );
	_orders[o->_order_id] = o;
	calc_in_progress();
	BMModel::instance()->send(new LFPositionData(_d));

	// send to BMModel
	tc_log->report_t(lg_OPR, lp_DEBUG, "LFLifePositionImpl::place_order", *r);
	BMModel::instance()->send(r.release());
}
void LFLifePositionImpl::cancel_order(LFCancelOrder* c_)
{
	auto_ptr<LFCancelOrder> r(c_);
	r->_t = TCTimestamp::stamp();
	// send to BMModel
	tc_log->report_t(lg_OPR, lp_DEBUG, "LFLifePositionImpl::cancel_order", *r);
	BMModel::instance()->send(r.release());
}
//======================================================================
//======================================================================
LFMockPositionImpl::LFMockPositionImpl(const std::string& instr_, long strategy_id_)
	: LFPositionImpl(instr_, strategy_id_)
{
}
//======================================================================
LFMockPositionImpl::~LFMockPositionImpl()
{
}
//======================================================================
void LFMockPositionImpl::place_order(LFNewOrder* r_)
{
	auto_ptr<LFNewOrder> r(r_);
	LFOrderUpdatePtr o(new LFOrderUpdate(*r));

	// for the record
	BMModel::instance()->send(r.release());

	if(o->_order_type != LF::otMarket)
	{
		o->_order_status = LF::osCanceled;
		o->_err_code = 1;
		o->_err_desc = "Mock Trading -- order type is not supported";
		// for the record
		BMModel::instance()->send(new LFOrderUpdate(*o));
		return;
	}
	TC_pqp p;
	LFSide::s(o->_order_side)->fill_contra(p, _md);
	if(!p.good())
	{
		o->_order_status = LF::osCanceled;
		o->_err_code = 1;
		o->_err_desc = "Mock Trading -- no contra market for order";
		// for the record
		BMModel::instance()->send(new LFOrderUpdate(*o));
		return;
	}
	// make fill
	auto_ptr<LFOrderFill> f(new LFOrderFill);
	f->_t = o->get_t();
	f->_strategy_key = o->_strategy_key;
	f->_order_id = o->_order_id;
	f->_strategy_id = o->_strategy_id;
	f->_sub_strategy_id = o->_sub_strategy_id;
	f->_instr = o->_instr;
	f->_cqg_InstrumentID = o->_cqg_InstrumentID;
	f->_fill.reset(p._p, o->_q);
	f->_fill_id = TCIdGenerator::instance()->sid("Fill");
	f->_side = o->_order_side;

	// update position
	_d.add_fill(*f);
	calc_in_progress();
	_d.mark_to_market(_md);
	// update order status
	o->_order_status = LF::osFilled;
	o->_filled_q = (long)f->_fill._q;
	o->_avg_fill_p = f->_fill._p;
	o->_remain_q = 0;
	o->_fills.push_back(*f);

	// for the record
	BMModel::instance()->send(f.release());
	BMModel::instance()->send(new LFOrderUpdate(*o));
	BMModel::instance()->send(new LFPositionData(_d));
}
void LFMockPositionImpl::cancel_order(LFCancelOrder* c_)
{

}
//======================================================================
//======================================================================
LFPosition::LFPosition(const std::string& instr_, long strategy_id_)
: _s(new BMStrategy(0))
{
	if(BMTS<LFNewOrder>::instance()->_out == BMTS_Properties::OUT_NONE)
		_impl.reset(new LFMockPositionImpl(instr_, strategy_id_));
	else
	{
		_impl.reset(new LFLifePositionImpl(instr_, strategy_id_, *_s));
		_s->open();
	}

}
//======================================================================
LFPosition::LFPosition(const std::string& instr_, long strategy_id_, BMStrategy& strategy_)
{
	if(BMTS<LFNewOrder>::instance()->_out == BMTS_Properties::OUT_NONE)
		_impl.reset(new LFMockPositionImpl(instr_, strategy_id_));
	else
	{
		_impl.reset(new LFLifePositionImpl(instr_, strategy_id_, strategy_));
//		strategy_.subscribe_input((LFLifePositionImpl*)_impl.get(), &LFLifePositionImpl::order_fill, _impl->data().get_key(), "order_fill");
		strategy_.subscribe_input((LFLifePositionImpl*)_impl.get(), &LFLifePositionImpl::order_update, _impl->data().get_key(), "order_update");
	}
}
//======================================================================
LFPosition::~LFPosition()
{
/*
	if(BMTS<LFNewOrder>::instance()->_out != BMTS_Properties::OUT_NONE)
	{
		strategy_.unsubscribe_input<LFOrderFillPtr>("order_fill");
		strategy_.unsubscribe_input<LFOrderUpdatePtr>("order_update");
	}
*/
	if(_s.get() != 0)
	{
		_s->close();
	}
	_impl.reset();
	_s.reset();
}
//======================================================================
void LFPosition::reset()
{
	_impl->reset();
}
//======================================================================
void LFPosition::reset(double open_p_, double open_q_,double realized_pnl_)
{
	_impl->reset(open_p_, open_q_, realized_pnl_);
}
//======================================================================
