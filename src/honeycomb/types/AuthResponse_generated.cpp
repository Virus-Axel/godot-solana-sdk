#include "honeycomb/types/AuthResponse_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AuthResponse::set_message(const String &val) {
	this->message = val;
}

String AuthResponse::get_message() const {
	return this->message;
}

void AuthResponse::set_tx(const PackedByteArray &val) {
	this->tx = val;
}

PackedByteArray AuthResponse::get_tx() const {
	return this->tx;
}

void AuthResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_message"), &AuthResponse::get_message);
	ClassDB::bind_method(D_METHOD("set_message", "value"), &AuthResponse::set_message);
	ClassDB::add_property("AuthResponse", PropertyInfo(Variant::Type::STRING, "message"), "set_message", "get_message");
	ClassDB::bind_method(D_METHOD("get_tx"), &AuthResponse::get_tx);
	ClassDB::bind_method(D_METHOD("set_tx", "value"), &AuthResponse::set_tx);
	ClassDB::add_property("AuthResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tx"), "set_tx", "get_tx");
	ClassDB::bind_method(D_METHOD("to_dict"), &AuthResponse::to_dict);
}
Dictionary AuthResponse::to_dict() {
	Dictionary res;
	if (!message.is_empty()) {
		res["message"] = message;
	}
	res["tx"] = tx;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot