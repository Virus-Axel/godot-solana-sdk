#include "honeycomb/types/TransactionResponse_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void TransactionResponse::set_signature(const String &val) {
	this->signature = val;
}

String TransactionResponse::get_signature() const {
	return this->signature;
}

void TransactionResponse::set_error(const Dictionary &val) {
	this->error = val;
}

Dictionary TransactionResponse::get_error() const {
	return this->error;
}

void TransactionResponse::set_status(const String &val) {
	this->status = val;
}

String TransactionResponse::get_status() const {
	return this->status;
}

void TransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_signature"), &TransactionResponse::get_signature);
	ClassDB::bind_method(D_METHOD("set_signature", "value"), &TransactionResponse::set_signature);
	ClassDB::add_property("TransactionResponse", PropertyInfo(Variant::Type::STRING, "signature"), "set_signature", "get_signature");
	ClassDB::bind_method(D_METHOD("get_error"), &TransactionResponse::get_error);
	ClassDB::bind_method(D_METHOD("set_error", "value"), &TransactionResponse::set_error);
	ClassDB::add_property("TransactionResponse", PropertyInfo(Variant::Type::DICTIONARY, "error"), "set_error", "get_error");
	ClassDB::bind_method(D_METHOD("get_status"), &TransactionResponse::get_status);
	ClassDB::bind_method(D_METHOD("set_status", "value"), &TransactionResponse::set_status);
	ClassDB::add_property("TransactionResponse", PropertyInfo(Variant::Type::STRING, "status"), "set_status", "get_status");
	ClassDB::bind_method(D_METHOD("to_dict"), &TransactionResponse::to_dict);
}
Dictionary TransactionResponse::to_dict() {
	Dictionary res;
	if (!signature.is_empty()) {
		res["signature"] = signature;
	}
	res["error"] = error;
	if (!status.is_empty()) {
		res["status"] = status;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot