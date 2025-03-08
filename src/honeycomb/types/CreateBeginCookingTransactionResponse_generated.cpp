#include "honeycomb/types/CreateBeginCookingTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateBeginCookingTransactionResponse::set_transaction(const Variant &val) {
	this->transaction = val;
}

Variant CreateBeginCookingTransactionResponse::get_transaction() const {
	return this->transaction;
}

void CreateBeginCookingTransactionResponse::set_cooking(const PackedByteArray &val) {
	this->cooking = val;
}

PackedByteArray CreateBeginCookingTransactionResponse::get_cooking() const {
	return this->cooking;
}

void CreateBeginCookingTransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_transaction"), &CreateBeginCookingTransactionResponse::get_transaction);
	ClassDB::bind_method(D_METHOD("set_transaction", "value"), &CreateBeginCookingTransactionResponse::set_transaction);
	ClassDB::add_property("CreateBeginCookingTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "transaction"), "set_transaction", "get_transaction");
	ClassDB::bind_method(D_METHOD("get_cooking"), &CreateBeginCookingTransactionResponse::get_cooking);
	ClassDB::bind_method(D_METHOD("set_cooking", "value"), &CreateBeginCookingTransactionResponse::set_cooking);
	ClassDB::add_property("CreateBeginCookingTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "cooking"), "set_cooking", "get_cooking");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateBeginCookingTransactionResponse::to_dict);
}
Dictionary CreateBeginCookingTransactionResponse::to_dict() {
	Dictionary res;

	if (transaction.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(transaction);
		if (ptr) {
			res["transaction"] = ptr->to_dict();
		}
	}
	res["cooking"] = cooking;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot