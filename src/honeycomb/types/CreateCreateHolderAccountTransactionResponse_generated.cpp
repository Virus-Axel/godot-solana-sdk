#include "honeycomb/types/CreateCreateHolderAccountTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateHolderAccountTransactionResponse::set_tx(const Variant &val) {
	this->tx = val;
}

Variant CreateCreateHolderAccountTransactionResponse::get_tx() const {
	return this->tx;
}

void CreateCreateHolderAccountTransactionResponse::set_holderAccount(const PackedByteArray &val) {
	this->holderAccount = val;
}

PackedByteArray CreateCreateHolderAccountTransactionResponse::get_holderAccount() const {
	return this->holderAccount;
}

void CreateCreateHolderAccountTransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateHolderAccountTransactionResponse::get_tx);
	ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateHolderAccountTransactionResponse::set_tx);
	ClassDB::add_property("CreateCreateHolderAccountTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
	ClassDB::bind_method(D_METHOD("get_holderAccount"), &CreateCreateHolderAccountTransactionResponse::get_holderAccount);
	ClassDB::bind_method(D_METHOD("set_holderAccount", "value"), &CreateCreateHolderAccountTransactionResponse::set_holderAccount);
	ClassDB::add_property("CreateCreateHolderAccountTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "holderAccount"), "set_holderAccount", "get_holderAccount");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateHolderAccountTransactionResponse::to_dict);
}
Dictionary CreateCreateHolderAccountTransactionResponse::to_dict() {
	Dictionary res;

	if (tx.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
		if (ptr) {
			res["tx"] = ptr->to_dict();
		}
	}
	res["holderAccount"] = holderAccount;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot