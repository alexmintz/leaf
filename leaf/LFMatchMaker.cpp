/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#include "leaf/LFMatchMaker.h"
//======================================================================
LFMatchMaker::LFMatchMaker(const std::string& name_, const std::string& type_, const std::string& signal_name_, const std::string& resource_base_)
	:_name(name_),
	_type(type_),
	_signal_name(signal_name_),
	_signal_resource_base(resource_base_),
	_resource_base(resource_base_ + type_ + ":"+ name_ + ":"),
	_match_key("e_" + name_),
	_match_full_key("e_" + name_ + "_" + signal_name_)
{
}

LFMatchMaker::~LFMatchMaker()
{
}

LFMatchPtr LFMatchMaker::make(const LFPoint& s_point_, const LFMomentumData& d_)
{
	return LFMatchPtr();
}
//======================================================================
