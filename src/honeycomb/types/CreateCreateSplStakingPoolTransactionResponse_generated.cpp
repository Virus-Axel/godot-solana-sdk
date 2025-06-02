#include "honeycomb/types/CreateCreateSplStakingPoolTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateSplStakingPoolTransactionResponse::set_tx(const Variant &val) {
	this->tx = val;
}

Variant CreateCreateSplStakingPoolTransactionResponse::get_tx() const {
	return this->tx;
}

void CreateCreateSplStakingPoolTransactionResponse::set_splStakingPoolAddress(const PackedByteArray &val) {
	this->splStakingPoolAddress = val;
}

PackedByteArray CreateCreateSplStakingPoolTransactionResponse::get_splStakingPoolAddress() const {
	return this->splStakingPoolAddress;
}

void CreateCreateSplStakingPoolTransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateSplStakingPoolTransactionResponse::get_tx);
	ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateSplStakingPoolTransactionResponse::set_tx);
	ClassDB::add_property("CreateCreateSplStakingPoolTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
	ClassDB::bind_method(D_METHOD("get_splStakingPoolAddress"), &CreateCreateSplStakingPoolTransactionResponse::get_splStakingPoolAddress);
	ClassDB::bind_method(D_METHOD("set_splStakingPoolAddress", "value"), &CreateCreateSplStakingPoolTransactionResponse::set_splStakingPoolAddress);
	ClassDB::add_property("CreateCreateSplStakingPoolTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "splStakingPoolAddress"), "set_splStakingPoolAddress", "get_splStakingPoolAddress");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateSplStakingPoolTransactionResponse::to_dict);
}
Dictionary CreateCreateSplStakingPoolTransactionResponse::to_dict() {
	Dictionary res;

	if (tx.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
		if (ptr) {
			res["tx"] = ptr->to_dict();
		}
	}
	res["splStakingPoolAddress"] = splStakingPoolAddress;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot