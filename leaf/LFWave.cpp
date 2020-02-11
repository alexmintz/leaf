/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFWave.h"
#include "leaf/LFMove.h"
//======================================================================
void LFWave::reset()
{
	_in.reset();
	_out.reset();
	_prev.reset();
	_wave_id = 0;
	_next.reset();
}
//======================================================================
void LFWave::reset(const LFSide* side_, const ACE_Time_Value& t_, double p_, double eng_)
{
	reset();
	_in.reset(side_, t_, p_, eng_);
	if(_in.valid())
		_out.reset(side_->contra_s(), t_, p_, eng_);
	else
		_out.reset();

}
//======================================================================
void LFWave::reset(const LFSlope& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFSlope& prev_s_)
{
	if(s_._s_t != _in._s_t)
	{
		reset();
		_in.reset(s_);
		if(prev_s_.valid())
		{
			_prev.reset(prev_s_);
		}
		new_peak_i(t_, p_, eng_, true);
		_out.update_peak(_prev, false, _l->_tick_size);
	}
}
//======================================================================
void LFWave::reset(const LFPeak& s_, const ACE_Time_Value& t_, double p_, double eng_, const LFPeak& prev_p_)
{
	if(s_._s_t != _in._s_t)
	{
		reset();
		_wave_id = ++_l->_wave_id;
		_in.reset(s_);
		_in._wave_id = _wave_id;
		_in._peak_id = -1;
		_in._sub_id = 0;
		new_peak_i(t_, p_, eng_, false);
		LF::model_send(_in, "in -- wave reset",t_, _l->_out_file);
		if(prev_p_.valid())
		{
			_prev.reset(_out._side, 
						prev_p_._s_t + LF::t_from_length(prev_p_._l),
						prev_p_._p, prev_p_._eng,
						0, 0, _l->_tick_size);
			_prev.update(s_._s_t, s_._s_p, s_._s_eng, _l->_tick_size, true);
			_prev.update(s_._s_t+ LF::t_from_length(s_._l), s_._p, s_._eng, _l->_tick_size, false);
			_prev._confirmed = true;
			_prev._vp_v = prev_p_._v;
			_prev._vp_eng_v = prev_p_._eng_v;
			_prev._vp_den_v = prev_p_._den_v;
			_prev._vp_l = prev_p_._l;
			_prev._pv_v = prev_p_._vp_v;
			_prev._pv_eng_v = prev_p_._vp_eng_v;
			_prev._pv_den_v = prev_p_._vp_den_v;
			_prev._pv_l = prev_p_._vp_l;

			_prev._pp_v = _prev._vp_v - _prev._pv_v;
			_prev._pp_eng_v = _prev._vp_eng_v - _prev._pv_eng_v;

			_prev._wave_id = _wave_id;
			LF::model_send(_prev, "prev -- wave reset", t_, _l->_out_file_wave_update);
		}
		_out.update_peak(_prev, false, _l->_tick_size);

		LF::model_send(_out, "out -- wave reset", t_, _l->_out_file_wave_update);
	}
}
//======================================================================
void LFWave::reset(const LFWave& from_, const ACE_Time_Value& t_, double p_, double eng_)
{
	reset();
	_wave_id = ++_l->_wave_id;
	_in.reset(from_._in);
	_in._wave_id = _wave_id;
	_in._peak_id = -1;
	_in._sub_id = 0;
	new_peak_i(t_, p_, eng_, false);
	_prev.reset(from_._prev);
	_prev._wave_id = _wave_id;
	_out.update_peak(_prev, false, _l->_tick_size);

	LF::model_send(_in, "in -- wave reset from", t_, _l->_out_file);
	LF::model_send(_prev, "prev -- wave reset from", t_, _l->_out_file);
	LF::model_send(_out, "out -- wave reset from", t_, _l->_out_file_wave_update);
}

//======================================================================
void LFWave::reset_all(const ACE_Time_Value& t_)
{
	LF::model_send(_in, "in -- wave reset_all", t_, _l->_out_file);
	if(_next.get() != 0)
		_next->reset_all(t_);
	reset();
}
//======================================================================
void LFWave::update(const ACE_Time_Value& t_, double p_, double eng_, bool peak_confirmed_, double min_first_peak_)
{
	if(!valid())
		return;

	bool just_born = false;
	if(!_out.valid() || _out.value(p_, _l->_tick_size) < 0.)
	{
		bool same_peak = true;
		if(_out._confirmed || _out._u_p != 0)
		{
			just_born = true;
			if(_out._formed)
			{
				if(_prev.valid())
				{
					LF::model_send(_prev, "old prev -- replace", t_, _l->_out_file_wave_update);
				}
				_prev.reset(_out);
				LF::model_send(_prev, "new prev -- valley_confirmed", t_, _l->_out_file_wave_update);
				same_peak = false;
			}
			else
			{
				LF::model_send(_prev, "old out -- not_valley_confirmed", t_, _l->_out_file_wave_update);
			}
		}
		new_peak_i(t_, p_, eng_, same_peak);
		if(just_born)
		{
			_out._just_born = true;
			LF::model_send(_out, "out -- just_born" , t_, _l->_out_file_wave_update);
		}
	}
	else
	{
		if(!_out._confirmed && _out.eng_value(eng_) < 0)
		{
			_in.update(t_, p_, eng_, _l->_tick_size, false, true);
			_out.reset_s_eng(t_, eng_);
			_out._e_den_v = _in._den_v;
			_out.update(t_, p_, eng_, _l->_tick_size, false, true);
		}
		else
		{
			_in.update(t_, p_, eng_, _l->_tick_size, false, false);
			_out.update(t_, p_, eng_, _l->_tick_size, false, false);
		}
		_out._just_born = false;
	}
	if(!peak_confirmed_ && !_out._confirmed && _l->_confirmed_ticks > 0 && _out._c_v > _l->_confirmed_ticks)
		peak_confirmed_ = true;

	_out.update_peak(_prev, peak_confirmed_, _l->_tick_size, min_first_peak_);

}
//======================================================================
void LFWave::update(const ACE_Time_Value& t_, double p_, double eng_, const LFMove& cur_move_, const LFMove& prev_move_, LFSlope& folded_)
{
	if(!valid())
		return;

	double min_first_peak = 0;
	if(_out._peak_id == 1 && _prev.valid())
	{
		min_first_peak = _l->_min_first_peak;
	}
	// check if energy is greater
	bool was_unconfirmed = _out._u_p > 0;
	 
	if(_out._confirmed 
//		&& !_out._formed
		&& _out.eng_value(eng_) < 0
		&& _next->_next.get() == 0
		)
	{
		if(_out._formed)
		{
			_out._s_p_eng = eng_;
			if(_next->_out.valid() && !_next->_out._confirmed)
			{
				folded_ = _next->_out;
				LF::model_send(_next->_out, "_next_out -- folded_unform" , t_, _l->_out_file_wave_update);
			}
		}

		_out._u_p = _out._s_p;
		_out._u_eng = _out._s_eng;
		_out._uv_eng = _out._eng;
		_out._confirmed = false;
		_out._just_confirmed = false;
		_out._confirm_t = t_;
		_out._formed = false;
		_out._just_formed = false;
		//LF::model_send(_out, "out -- greater_eng" , t_, _l->_out_file_wave_update);
		//LF::model_send(_in, "in -- greater_eng" , t_, _l->_out_file);
		_next->reset_all(t_);
		_next.reset();
	}
	if(_out._u_p > 0 && !was_unconfirmed)
		_out._just_unconfirmed = true;

	bool move_conf = LFMove::move_confirmed(side(), cur_move_, prev_move_, _out._vp_eng_v > _l->_formed_eng);
	update(t_, p_, eng_, move_conf, min_first_peak);
	if(!_out._confirmed)
	{
		if(_next.get() != 0)
		{
			if(_out._sub_id == 1)
			{
				if(_next->_out.valid() && !_next->_out._confirmed)
				{
					folded_ = _next->_out;
					LF::model_send(_next->_out, "_next_out -- folded" , t_, _l->_out_file_wave_update);
				}
			}
			LF::model_send(_in, "in -- new_peak" , t_, _l->_out_file);
			_next->reset_all(t_);
		}
		_next.reset();
	}
	else if(_out._just_confirmed)
	{
		_out._confirm_t = t_;
		LF::model_send(_in, "in -- out just_confirmed" , t_, _l->_out_file);
		LF::model_send(_out, "out -- just_confirmed" , t_, _l->_out_file_wave_update);
//		_next.reset(new LFWave(_l, _out, t_, p_, eng_, _prev));
		_next.reset(new LFWave(_l));
		_next->init();
		_next->reset(_out, t_, p_, eng_, _prev);

	}

	if(_next.get() == 0)
		return;

	_next->update(t_, p_, eng_, cur_move_, prev_move_, folded_);
	_out.adjust_max(_next->_in);
	//
	bool w_formed = _out._formed;
	if(_out._confirmed && !_out._formed)
	{
		double eng_add = _l->_formed_on_stale ? _out.eng_value(_out._s_p_eng): 0.;

		if(_out._eng_v +  eng_add > _l->_formed_eng)
			_out._formed = true;
		if(_l->_formed_ticks > 0 && _out._v > _l->_formed_ticks)
			_out._formed = true;
	}
	_out._just_formed = _out._formed && !w_formed;
	if(_out._just_formed)
	{
		_out._formed_t = t_;
	}

}
//======================================================================
void LFWave::new_peak_i(const ACE_Time_Value& t_, double p_, double eng_, bool same_peak_)
{
	_in.update(t_, p_, eng_, _l->_tick_size, true);

	// new expectations
	double e_s = 2.*_l->_slope_mid - _in._s;
	if(e_s < _in._s)
	{
		if(e_s < _in._s/_l->_slope_max_ratio)
			e_s = _in._s/_l->_slope_max_ratio;
	}
	else
	{
		if(e_s > _in._s*_l->_slope_max_ratio)
			e_s = _in._s*_l->_slope_max_ratio;
	}
	double e_v = _in._v*_l->_recoil_cutoff;
	double e_l = 0;
	if(e_s > 0)
		e_l = e_v/e_s;

	LFPeak oldp = _out;

	_out.reset(side()->contra_s(), t_, p_, eng_, e_l, e_v, _l->_tick_size);
	_out._e_den_v = _in._den_v;
	_out._wave_id = _wave_id;
	if(same_peak_)
	{
		if(oldp.valid())
		{
			_out.set_identity(oldp);
		}
		if(oldp._confirmed || oldp._u_p != 0)
		{
			_out._sub_id++;
		}
	}
	else
	{
		_out._peak_id = oldp._peak_id + 1;
		_out._sub_id = 1;
	}
}
//======================================================================
void LFWave::model_send(const std::string& comment_, const ACE_Time_Value& t_, bool out_file_, const std::string& key_, long in_sub_id_) const
{
	LFPeak in(_in);
	in._sub_id = in_sub_id_;
	LF::model_send(in, comment_, t_, out_file_, key_);
	LF::model_send(_out, comment_, t_, out_file_, key_);
}

//======================================================================
