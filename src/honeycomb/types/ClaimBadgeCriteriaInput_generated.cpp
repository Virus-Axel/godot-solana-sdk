#include "honeycomb/types/ClaimBadgeCriteriaInput_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ClaimBadgeCriteriaInput::set_projectAddress(const Variant &val) {
	this->projectAddress = val;
}

Variant ClaimBadgeCriteriaInput::get_projectAddress() const {
	return this->projectAddress;
}

void ClaimBadgeCriteriaInput::set_profileAddress(const Variant &val) {
	this->profileAddress = val;
}

Variant ClaimBadgeCriteriaInput::get_profileAddress() const {
	return this->profileAddress;
}

void ClaimBadgeCriteriaInput::set_payer(const Variant &val) {
	this->payer = val;
}

Variant ClaimBadgeCriteriaInput::get_payer() const {
	return this->payer;
}

void ClaimBadgeCriteriaInput::set_criteriaIndex(const int32_t &val) {
	this->criteriaIndex = val;
}

int32_t ClaimBadgeCriteriaInput::get_criteriaIndex() const {
	return this->criteriaIndex;
}

void ClaimBadgeCriteriaInput::set_proof(const String &val) {
	this->proof = val;
}

String ClaimBadgeCriteriaInput::get_proof() const {
	return this->proof;
}

void ClaimBadgeCriteriaInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_projectAddress"), &ClaimBadgeCriteriaInput::get_projectAddress);
	ClassDB::bind_method(D_METHOD("set_projectAddress", "value"), &ClaimBadgeCriteriaInput::set_projectAddress);
	ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "projectAddress"), "set_projectAddress", "get_projectAddress");
	ClassDB::bind_method(D_METHOD("get_profileAddress"), &ClaimBadgeCriteriaInput::get_profileAddress);
	ClassDB::bind_method(D_METHOD("set_profileAddress", "value"), &ClaimBadgeCriteriaInput::set_profileAddress);
	ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "profileAddress"), "set_profileAddress", "get_profileAddress");
	ClassDB::bind_method(D_METHOD("get_payer"), &ClaimBadgeCriteriaInput::get_payer);
	ClassDB::bind_method(D_METHOD("set_payer", "value"), &ClaimBadgeCriteriaInput::set_payer);
	ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::OBJECT, "payer"), "set_payer", "get_payer");
	ClassDB::bind_method(D_METHOD("get_criteriaIndex"), &ClaimBadgeCriteriaInput::get_criteriaIndex);
	ClassDB::bind_method(D_METHOD("set_criteriaIndex", "value"), &ClaimBadgeCriteriaInput::set_criteriaIndex);
	ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::Type::INT, "criteriaIndex"), "set_criteriaIndex", "get_criteriaIndex");
	ClassDB::bind_method(D_METHOD("get_proof"), &ClaimBadgeCriteriaInput::get_proof);
	ClassDB::bind_method(D_METHOD("set_proof", "value"), &ClaimBadgeCriteriaInput::set_proof);
	ClassDB::add_property("ClaimBadgeCriteriaInput", PropertyInfo(Variant::STRING, "proof"), "set_proof", "get_proof");
	ClassDB::bind_method(D_METHOD("to_dict"), &ClaimBadgeCriteriaInput::to_dict);
}
Dictionary ClaimBadgeCriteriaInput::to_dict() {
	Dictionary res;
	if (projectAddress.has_method("to_string")) {
		res["projectAddress"] = projectAddress.call("to_string");
	} else {
		res["projectAddress"] = projectAddress;
	}
	if (profileAddress.has_method("to_string")) {
		res["profileAddress"] = profileAddress.call("to_string");
	} else {
		res["profileAddress"] = profileAddress;
	}
	if (payer.has_method("to_string")) {
		res["payer"] = payer.call("to_string");
	} else {
		res["payer"] = payer;
	}
	res["criteriaIndex"] = criteriaIndex;
	res["proof"] = this->proof;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot