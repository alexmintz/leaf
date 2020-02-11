/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef LF_MATCH_MAKER_H
#define LF_MATCH_MAKER_H

#include "znet/ZNet.h"
#include "leaf/LFMomentumData.h"

//======================================================================
class LEAF_Export LFMatchMaker
{
public:
	LFMatchMaker(const std::string& name_, const std::string& type_, const std::string& signal_name_, const std::string& resource_base_);
	virtual ~LFMatchMaker();

	const std::string& get_name() const { return _name; }
	const std::string& get_signal_name() const { return _signal_name; }
	const std::string& get_resource_base() const { return _resource_base; }
	const std::string& get_match_key() const { return _match_key; }
	const std::string& get_match_full_key() const { return _match_full_key; }

	virtual LFMatchPtr make(const LFPoint& s_point_, const LFMomentumData& d_);

protected:
	std::string				_name;
	std::string				_type;
	std::string				_signal_name;
	std::string				_signal_resource_base;
	std::string				_resource_base;
	std::string				_match_key;
	std::string				_match_full_key;

};
typedef Z::smart_ptr<LFMatchMaker> LFMatchMakerPtr;

//======================================================================
#endif /*LF_MATCH_MAKER_H*/
