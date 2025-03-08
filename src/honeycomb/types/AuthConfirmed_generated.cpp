#include "honeycomb/types/AuthConfirmed_generated.hpp"

#include "honeycomb/types/User_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AuthConfirmed::set_accessToken(const String &val) {
	this->accessToken = val;
}

String AuthConfirmed::get_accessToken() const {
	return this->accessToken;
}

void AuthConfirmed::set_user(const Variant &val) {
	this->user = val;
}

Variant AuthConfirmed::get_user() const {
	return this->user;
}

void AuthConfirmed::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_accessToken"), &AuthConfirmed::get_accessToken);
	ClassDB::bind_method(D_METHOD("set_accessToken", "value"), &AuthConfirmed::set_accessToken);
	ClassDB::add_property("AuthConfirmed", PropertyInfo(Variant::Type::STRING, "accessToken"), "set_accessToken", "get_accessToken");
	ClassDB::bind_method(D_METHOD("get_user"), &AuthConfirmed::get_user);
	ClassDB::bind_method(D_METHOD("set_user", "value"), &AuthConfirmed::set_user);
	ClassDB::add_property("AuthConfirmed", PropertyInfo(Variant::Type::OBJECT, "user"), "set_user", "get_user");
	ClassDB::bind_method(D_METHOD("to_dict"), &AuthConfirmed::to_dict);
}
Dictionary AuthConfirmed::to_dict() {
	Dictionary res;
	if (!accessToken.is_empty()) {
		res["accessToken"] = accessToken;
	}

	if (user.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::User>(user);
		if (ptr) {
			res["user"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot