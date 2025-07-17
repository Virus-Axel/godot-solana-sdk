#include "honeycomb/types/SplStakingPool_generated.hpp"

#include "honeycomb/types/ControlledMerkleTrees_generated.hpp"
#include "honeycomb/types/SplRewardConfig_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void SplStakingPool::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray SplStakingPool::get_address() const {
	return this->address;
}

void SplStakingPool::set_bump(const int32_t &val) {
	this->bump = val;
}

int32_t SplStakingPool::get_bump() const {
	return this->bump;
}

void SplStakingPool::set_nonce(const int32_t &val) {
	this->nonce = val;
}

int32_t SplStakingPool::get_nonce() const {
	return this->nonce;
}

void SplStakingPool::set_project(const Variant &val) {
	this->project = val;
}

Variant SplStakingPool::get_project() const {
	return this->project;
}

void SplStakingPool::set_name(const String &val) {
	this->name = val;
}

String SplStakingPool::get_name() const {
	return this->name;
}

void SplStakingPool::set_stake_token_mint(const Variant &val) {
	this->stake_token_mint = val;
}

Variant SplStakingPool::get_stake_token_mint() const {
	return this->stake_token_mint;
}

void SplStakingPool::set_minStakeDurationSecs(const int64_t &val) {
	this->minStakeDurationSecs = val;
}

int64_t SplStakingPool::get_minStakeDurationSecs() const {
	return this->minStakeDurationSecs;
}

void SplStakingPool::set_maxStakeDurationSecs(const int64_t &val) {
	this->maxStakeDurationSecs = val;
}

int64_t SplStakingPool::get_maxStakeDurationSecs() const {
	return this->maxStakeDurationSecs;
}

void SplStakingPool::set_startTime(const int64_t &val) {
	this->startTime = val;
}

int64_t SplStakingPool::get_startTime() const {
	return this->startTime;
}

void SplStakingPool::set_endTime(const int64_t &val) {
	this->endTime = val;
}

int64_t SplStakingPool::get_endTime() const {
	return this->endTime;
}

void SplStakingPool::set_totalStakedAmount(const int64_t &val) {
	this->totalStakedAmount = val;
}

int64_t SplStakingPool::get_totalStakedAmount() const {
	return this->totalStakedAmount;
}

void SplStakingPool::set_multipliers(const Array &val) {
	this->multipliers = val;
}

Array SplStakingPool::get_multipliers() const {
	return this->multipliers;
}

void SplStakingPool::set_rewardConfig(const Variant &val) {
	this->rewardConfig = val;
}

Variant SplStakingPool::get_rewardConfig() const {
	return this->rewardConfig;
}

void SplStakingPool::set_merkleTrees(const Variant &val) {
	this->merkleTrees = val;
}

Variant SplStakingPool::get_merkleTrees() const {
	return this->merkleTrees;
}

void SplStakingPool::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &SplStakingPool::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &SplStakingPool::set_address);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_bump"), &SplStakingPool::get_bump);
	ClassDB::bind_method(D_METHOD("set_bump", "value"), &SplStakingPool::set_bump);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
	ClassDB::bind_method(D_METHOD("get_nonce"), &SplStakingPool::get_nonce);
	ClassDB::bind_method(D_METHOD("set_nonce", "value"), &SplStakingPool::set_nonce);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "nonce"), "set_nonce", "get_nonce");
	ClassDB::bind_method(D_METHOD("get_project"), &SplStakingPool::get_project);
	ClassDB::bind_method(D_METHOD("set_project", "value"), &SplStakingPool::set_project);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
	ClassDB::bind_method(D_METHOD("get_name"), &SplStakingPool::get_name);
	ClassDB::bind_method(D_METHOD("set_name", "value"), &SplStakingPool::set_name);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::STRING, "name"), "set_name", "get_name");
	ClassDB::bind_method(D_METHOD("get_stake_token_mint"), &SplStakingPool::get_stake_token_mint);
	ClassDB::bind_method(D_METHOD("set_stake_token_mint", "value"), &SplStakingPool::set_stake_token_mint);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::OBJECT, "stake_token_mint"), "set_stake_token_mint", "get_stake_token_mint");
	ClassDB::bind_method(D_METHOD("get_minStakeDurationSecs"), &SplStakingPool::get_minStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_minStakeDurationSecs", "value"), &SplStakingPool::set_minStakeDurationSecs);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "minStakeDurationSecs"), "set_minStakeDurationSecs", "get_minStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_maxStakeDurationSecs"), &SplStakingPool::get_maxStakeDurationSecs);
	ClassDB::bind_method(D_METHOD("set_maxStakeDurationSecs", "value"), &SplStakingPool::set_maxStakeDurationSecs);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "maxStakeDurationSecs"), "set_maxStakeDurationSecs", "get_maxStakeDurationSecs");
	ClassDB::bind_method(D_METHOD("get_startTime"), &SplStakingPool::get_startTime);
	ClassDB::bind_method(D_METHOD("set_startTime", "value"), &SplStakingPool::set_startTime);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "startTime"), "set_startTime", "get_startTime");
	ClassDB::bind_method(D_METHOD("get_endTime"), &SplStakingPool::get_endTime);
	ClassDB::bind_method(D_METHOD("set_endTime", "value"), &SplStakingPool::set_endTime);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
	ClassDB::bind_method(D_METHOD("get_totalStakedAmount"), &SplStakingPool::get_totalStakedAmount);
	ClassDB::bind_method(D_METHOD("set_totalStakedAmount", "value"), &SplStakingPool::set_totalStakedAmount);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::INT, "totalStakedAmount"), "set_totalStakedAmount", "get_totalStakedAmount");
	ClassDB::bind_method(D_METHOD("get_multipliers"), &SplStakingPool::get_multipliers);
	ClassDB::bind_method(D_METHOD("set_multipliers", "value"), &SplStakingPool::set_multipliers);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::ARRAY, "multipliers"), "set_multipliers", "get_multipliers");
	ClassDB::bind_method(D_METHOD("get_rewardConfig"), &SplStakingPool::get_rewardConfig);
	ClassDB::bind_method(D_METHOD("set_rewardConfig", "value"), &SplStakingPool::set_rewardConfig);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::OBJECT, "rewardConfig"), "set_rewardConfig", "get_rewardConfig");
	ClassDB::bind_method(D_METHOD("get_merkleTrees"), &SplStakingPool::get_merkleTrees);
	ClassDB::bind_method(D_METHOD("set_merkleTrees", "value"), &SplStakingPool::set_merkleTrees);
	ClassDB::add_property("SplStakingPool", PropertyInfo(Variant::Type::OBJECT, "merkleTrees"), "set_merkleTrees", "get_merkleTrees");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplStakingPool::to_dict);
}
Dictionary SplStakingPool::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["bump"] = bump;
	res["nonce"] = nonce;
	if (project.has_method("to_string")) {
		res["project"] = project.call("to_string");
	} else {
		res["project"] = project;
	}
	if (!name.is_empty()) {
		res["name"] = name;
	}
	if (stake_token_mint.has_method("to_string")) {
		res["stake_token_mint"] = stake_token_mint.call("to_string");
	} else {
		res["stake_token_mint"] = stake_token_mint;
	}
	res["minStakeDurationSecs"] = minStakeDurationSecs;
	res["maxStakeDurationSecs"] = maxStakeDurationSecs;
	res["startTime"] = startTime;
	res["endTime"] = endTime;
	res["totalStakedAmount"] = totalStakedAmount;
	res["multipliers"] = multipliers;

	if (rewardConfig.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::SplRewardConfig>(rewardConfig);
		if (ptr) {
			res["rewardConfig"] = ptr->to_dict();
		}
	}

	if (merkleTrees.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(merkleTrees);
		if (ptr) {
			res["merkleTrees"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot