#include "honeycomb/types/UsedByCustom_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByCustom::set_user(const Variant &val) {
	this->user = val;
}

Variant UsedByCustom::get_user() const {
	return this->user;
}

void UsedByCustom::set_data(const Dictionary &val) {
	this->data = val;
}

Dictionary UsedByCustom::get_data() const {
	return this->data;
}

void UsedByCustom::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_user"), &UsedByCustom::get_user);
	ClassDB::bind_method(D_METHOD("set_user", "value"), &UsedByCustom::set_user);
	ClassDB::add_property("UsedByCustom", PropertyInfo(Variant::Type::OBJECT, "user"), "set_user", "get_user");
	ClassDB::bind_method(D_METHOD("get_data"), &UsedByCustom::get_data);
	ClassDB::bind_method(D_METHOD("set_data", "value"), &UsedByCustom::set_data);
	ClassDB::add_property("UsedByCustom", PropertyInfo(Variant::Type::DICTIONARY, "data"), "set_data", "get_data");
	ClassDB::bind_method(D_METHOD("to_dict"), &UsedByCustom::to_dict);
}
Dictionary UsedByCustom::to_dict() {
	Dictionary res;
	if (user.has_method("to_string")) {
		res["user"] = user.call("to_string");
	} else {
		res["user"] = user;
	}
	res["data"] = data;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot