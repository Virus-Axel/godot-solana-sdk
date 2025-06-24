#include "honeycomb/types/StakingRecipient_generated.hpp"

#include "honeycomb/types/Proof_generated.hpp"
#include "honeycomb/types/StakerClaimed_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void StakingRecipient::set_address(const PackedByteArray &val) {
	this->address = val;
}

PackedByteArray StakingRecipient::get_address() const {
	return this->address;
}

void StakingRecipient::set_tree_id(const PackedByteArray &val) {
	this->tree_id = val;
}

PackedByteArray StakingRecipient::get_tree_id() const {
	return this->tree_id;
}

void StakingRecipient::set_leaf_idx(const int64_t &val) {
	this->leaf_idx = val;
}

int64_t StakingRecipient::get_leaf_idx() const {
	return this->leaf_idx;
}

void StakingRecipient::set_staker(const Variant &val) {
	this->staker = val;
}

Variant StakingRecipient::get_staker() const {
	return this->staker;
}

void StakingRecipient::set_stakedAmount(const int64_t &val) {
	this->stakedAmount = val;
}

int64_t StakingRecipient::get_stakedAmount() const {
	return this->stakedAmount;
}

void StakingRecipient::set_rewardsClaimed(const Variant &val) {
	this->rewardsClaimed = val;
}

Variant StakingRecipient::get_rewardsClaimed() const {
	return this->rewardsClaimed;
}

void StakingRecipient::set_multiplier(const int32_t &val) {
	this->multiplier = val;
}

int32_t StakingRecipient::get_multiplier() const {
	return this->multiplier;
}

void StakingRecipient::set_stakeStartTime(const int64_t &val) {
	this->stakeStartTime = val;
}

int64_t StakingRecipient::get_stakeStartTime() const {
	return this->stakeStartTime;
}

void StakingRecipient::set_lokedTill(const int64_t &val) {
	this->lokedTill = val;
}

int64_t StakingRecipient::get_lokedTill() const {
	return this->lokedTill;
}

void StakingRecipient::set_proof(const Variant &val) {
	this->proof = val;
}

Variant StakingRecipient::get_proof() const {
	return this->proof;
}

void StakingRecipient::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &StakingRecipient::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &StakingRecipient::set_address);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_tree_id"), &StakingRecipient::get_tree_id);
	ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &StakingRecipient::set_tree_id);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
	ClassDB::bind_method(D_METHOD("get_leaf_idx"), &StakingRecipient::get_leaf_idx);
	ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &StakingRecipient::set_leaf_idx);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
	ClassDB::bind_method(D_METHOD("get_staker"), &StakingRecipient::get_staker);
	ClassDB::bind_method(D_METHOD("set_staker", "value"), &StakingRecipient::set_staker);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::OBJECT, "staker"), "set_staker", "get_staker");
	ClassDB::bind_method(D_METHOD("get_stakedAmount"), &StakingRecipient::get_stakedAmount);
	ClassDB::bind_method(D_METHOD("set_stakedAmount", "value"), &StakingRecipient::set_stakedAmount);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::INT, "stakedAmount"), "set_stakedAmount", "get_stakedAmount");
	ClassDB::bind_method(D_METHOD("get_rewardsClaimed"), &StakingRecipient::get_rewardsClaimed);
	ClassDB::bind_method(D_METHOD("set_rewardsClaimed", "value"), &StakingRecipient::set_rewardsClaimed);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::OBJECT, "rewardsClaimed"), "set_rewardsClaimed", "get_rewardsClaimed");
	ClassDB::bind_method(D_METHOD("get_multiplier"), &StakingRecipient::get_multiplier);
	ClassDB::bind_method(D_METHOD("set_multiplier", "value"), &StakingRecipient::set_multiplier);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::INT, "multiplier"), "set_multiplier", "get_multiplier");
	ClassDB::bind_method(D_METHOD("get_stakeStartTime"), &StakingRecipient::get_stakeStartTime);
	ClassDB::bind_method(D_METHOD("set_stakeStartTime", "value"), &StakingRecipient::set_stakeStartTime);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::INT, "stakeStartTime"), "set_stakeStartTime", "get_stakeStartTime");
	ClassDB::bind_method(D_METHOD("get_lokedTill"), &StakingRecipient::get_lokedTill);
	ClassDB::bind_method(D_METHOD("set_lokedTill", "value"), &StakingRecipient::set_lokedTill);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::INT, "lokedTill"), "set_lokedTill", "get_lokedTill");
	ClassDB::bind_method(D_METHOD("get_proof"), &StakingRecipient::get_proof);
	ClassDB::bind_method(D_METHOD("set_proof", "value"), &StakingRecipient::set_proof);
	ClassDB::add_property("StakingRecipient", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
	ClassDB::bind_method(D_METHOD("to_dict"), &StakingRecipient::to_dict);
}
Dictionary StakingRecipient::to_dict() {
	Dictionary res;
	res["address"] = address;
	res["tree_id"] = tree_id;
	res["leaf_idx"] = leaf_idx;
	if (staker.has_method("to_string")) {
		res["staker"] = staker.call("to_string");
	} else {
		res["staker"] = staker;
	}
	res["stakedAmount"] = stakedAmount;

	if (rewardsClaimed.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::StakerClaimed>(rewardsClaimed);
		if (ptr) {
			res["rewardsClaimed"] = ptr->to_dict();
		}
	}
	res["multiplier"] = multiplier;
	res["stakeStartTime"] = stakeStartTime;
	res["lokedTill"] = lokedTill;

	if (proof.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
		if (ptr) {
			res["proof"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot