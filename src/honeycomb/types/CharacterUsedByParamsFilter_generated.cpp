#include "honeycomb/types/CharacterUsedByParamsFilter_generated.hpp"

#include "honeycomb/types/GuildRoleFilter_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void CharacterUsedByParamsFilter::set_pool(const Variant &val) {
	this->pool = val;
}

Variant CharacterUsedByParamsFilter::get_pool() const {
	return this->pool;
}

void CharacterUsedByParamsFilter::set_staker(const Variant &val) {
	this->staker = val;
}

Variant CharacterUsedByParamsFilter::get_staker() const {
	return this->staker;
}

void CharacterUsedByParamsFilter::set_stakedAt(const int32_t &val) {
	this->stakedAt = val;
}

int32_t CharacterUsedByParamsFilter::get_stakedAt() const {
	return this->stakedAt;
}

void CharacterUsedByParamsFilter::set_claimedAt(const int32_t &val) {
	this->claimedAt = val;
}

int32_t CharacterUsedByParamsFilter::get_claimedAt() const {
	return this->claimedAt;
}

void CharacterUsedByParamsFilter::set_id(const Variant &val) {
	this->id = val;
}

Variant CharacterUsedByParamsFilter::get_id() const {
	return this->id;
}

void CharacterUsedByParamsFilter::set_rewards(const Array &val) {
	this->rewards = val;
}

Array CharacterUsedByParamsFilter::get_rewards() const {
	return this->rewards;
}

void CharacterUsedByParamsFilter::set_endTime(const int32_t &val) {
	this->endTime = val;
}

int32_t CharacterUsedByParamsFilter::get_endTime() const {
	return this->endTime;
}

void CharacterUsedByParamsFilter::set_rewardsCollected(const bool &val) {
	this->rewardsCollected = val;
}

bool CharacterUsedByParamsFilter::get_rewardsCollected() const {
	return this->rewardsCollected;
}

void CharacterUsedByParamsFilter::set_role(const Variant &val) {
	this->role = val;
}

Variant CharacterUsedByParamsFilter::get_role() const {
	return this->role;
}

void CharacterUsedByParamsFilter::set_order(const int32_t &val) {
	this->order = val;
}

int32_t CharacterUsedByParamsFilter::get_order() const {
	return this->order;
}

void CharacterUsedByParamsFilter::set_mint(const Variant &val) {
	this->mint = val;
}

Variant CharacterUsedByParamsFilter::get_mint() const {
	return this->mint;
}

void CharacterUsedByParamsFilter::set_user(const Variant &val) {
	this->user = val;
}

Variant CharacterUsedByParamsFilter::get_user() const {
	return this->user;
}

void CharacterUsedByParamsFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_pool"), &CharacterUsedByParamsFilter::get_pool);
	ClassDB::bind_method(D_METHOD("set_pool", "value"), &CharacterUsedByParamsFilter::set_pool);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "pool"), "set_pool", "get_pool");
	ClassDB::bind_method(D_METHOD("get_staker"), &CharacterUsedByParamsFilter::get_staker);
	ClassDB::bind_method(D_METHOD("set_staker", "value"), &CharacterUsedByParamsFilter::set_staker);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "staker"), "set_staker", "get_staker");
	ClassDB::bind_method(D_METHOD("get_stakedAt"), &CharacterUsedByParamsFilter::get_stakedAt);
	ClassDB::bind_method(D_METHOD("set_stakedAt", "value"), &CharacterUsedByParamsFilter::set_stakedAt);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::INT, "stakedAt"), "set_stakedAt", "get_stakedAt");
	ClassDB::bind_method(D_METHOD("get_claimedAt"), &CharacterUsedByParamsFilter::get_claimedAt);
	ClassDB::bind_method(D_METHOD("set_claimedAt", "value"), &CharacterUsedByParamsFilter::set_claimedAt);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::INT, "claimedAt"), "set_claimedAt", "get_claimedAt");
	ClassDB::bind_method(D_METHOD("get_id"), &CharacterUsedByParamsFilter::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "value"), &CharacterUsedByParamsFilter::set_id);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "id"), "set_id", "get_id");
	ClassDB::bind_method(D_METHOD("get_rewards"), &CharacterUsedByParamsFilter::get_rewards);
	ClassDB::bind_method(D_METHOD("set_rewards", "value"), &CharacterUsedByParamsFilter::set_rewards);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::ARRAY, "rewards"), "set_rewards", "get_rewards");
	ClassDB::bind_method(D_METHOD("get_endTime"), &CharacterUsedByParamsFilter::get_endTime);
	ClassDB::bind_method(D_METHOD("set_endTime", "value"), &CharacterUsedByParamsFilter::set_endTime);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::INT, "endTime"), "set_endTime", "get_endTime");
	ClassDB::bind_method(D_METHOD("get_rewardsCollected"), &CharacterUsedByParamsFilter::get_rewardsCollected);
	ClassDB::bind_method(D_METHOD("set_rewardsCollected", "value"), &CharacterUsedByParamsFilter::set_rewardsCollected);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::BOOL, "rewardsCollected"), "set_rewardsCollected", "get_rewardsCollected");
	ClassDB::bind_method(D_METHOD("get_role"), &CharacterUsedByParamsFilter::get_role);
	ClassDB::bind_method(D_METHOD("set_role", "value"), &CharacterUsedByParamsFilter::set_role);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "role"), "set_role", "get_role");
	ClassDB::bind_method(D_METHOD("get_order"), &CharacterUsedByParamsFilter::get_order);
	ClassDB::bind_method(D_METHOD("set_order", "value"), &CharacterUsedByParamsFilter::set_order);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::INT, "order"), "set_order", "get_order");
	ClassDB::bind_method(D_METHOD("get_mint"), &CharacterUsedByParamsFilter::get_mint);
	ClassDB::bind_method(D_METHOD("set_mint", "value"), &CharacterUsedByParamsFilter::set_mint);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "mint"), "set_mint", "get_mint");
	ClassDB::bind_method(D_METHOD("get_user"), &CharacterUsedByParamsFilter::get_user);
	ClassDB::bind_method(D_METHOD("set_user", "value"), &CharacterUsedByParamsFilter::set_user);
	ClassDB::add_property("CharacterUsedByParamsFilter", PropertyInfo(Variant::Type::OBJECT, "user"), "set_user", "get_user");
	ClassDB::bind_method(D_METHOD("to_dict"), &CharacterUsedByParamsFilter::to_dict);
}
Dictionary CharacterUsedByParamsFilter::to_dict() {
	Dictionary res;
	if (pool.has_method("to_string")) {
		res["pool"] = pool.call("to_string");
	} else {
		res["pool"] = pool;
	}
	if (staker.has_method("to_string")) {
		res["staker"] = staker.call("to_string");
	} else {
		res["staker"] = staker;
	}
	res["stakedAt"] = stakedAt;
	res["claimedAt"] = claimedAt;
	if (id.has_method("to_string")) {
		res["id"] = id.call("to_string");
	} else {
		res["id"] = id;
	}
	res["rewards"] = rewards;
	res["endTime"] = endTime;
	res["rewardsCollected"] = rewardsCollected;

	if (role.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::GuildRoleFilter>(role);
		if (ptr) {
			res["role"] = ptr->to_dict();
		}
	}
	res["order"] = order;
	if (mint.has_method("to_string")) {
		res["mint"] = mint.call("to_string");
	} else {
		res["mint"] = mint;
	}
	if (user.has_method("to_string")) {
		res["user"] = user.call("to_string");
	} else {
		res["user"] = user;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot