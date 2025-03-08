#include "honeycomb/types/DisrcriminatorFilter_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void DisrcriminatorFilter::set_programId(const String &val) {
	this->programId = val;
}

String DisrcriminatorFilter::get_programId() const {
	return this->programId;
}

void DisrcriminatorFilter::set_accountName(const String &val) {
	this->accountName = val;
}

String DisrcriminatorFilter::get_accountName() const {
	return this->accountName;
}

void DisrcriminatorFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_programId"), &DisrcriminatorFilter::get_programId);
	ClassDB::bind_method(D_METHOD("set_programId", "value"), &DisrcriminatorFilter::set_programId);
	ClassDB::add_property("DisrcriminatorFilter", PropertyInfo(Variant::Type::STRING, "programId"), "set_programId", "get_programId");
	ClassDB::bind_method(D_METHOD("get_accountName"), &DisrcriminatorFilter::get_accountName);
	ClassDB::bind_method(D_METHOD("set_accountName", "value"), &DisrcriminatorFilter::set_accountName);
	ClassDB::add_property("DisrcriminatorFilter", PropertyInfo(Variant::Type::STRING, "accountName"), "set_accountName", "get_accountName");
	ClassDB::bind_method(D_METHOD("to_dict"), &DisrcriminatorFilter::to_dict);
}
Dictionary DisrcriminatorFilter::to_dict() {
	Dictionary res;
	if (!programId.is_empty()) {
		res["programId"] = programId;
	}
	if (!accountName.is_empty()) {
		res["accountName"] = accountName;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot