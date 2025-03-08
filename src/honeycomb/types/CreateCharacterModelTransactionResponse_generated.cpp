#include "honeycomb/types/CreateCharacterModelTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCharacterModelTransactionResponse::set_tx(const Variant &val) {
	this->tx = val;
}

Variant CreateCharacterModelTransactionResponse::get_tx() const {
	return this->tx;
}

void CreateCharacterModelTransactionResponse::set_characterModel(const PackedByteArray &val) {
	this->characterModel = val;
}

PackedByteArray CreateCharacterModelTransactionResponse::get_characterModel() const {
	return this->characterModel;
}

void CreateCharacterModelTransactionResponse::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_tx"), &CreateCharacterModelTransactionResponse::get_tx);
	ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCharacterModelTransactionResponse::set_tx);
	ClassDB::add_property("CreateCharacterModelTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
	ClassDB::bind_method(D_METHOD("get_characterModel"), &CreateCharacterModelTransactionResponse::get_characterModel);
	ClassDB::bind_method(D_METHOD("set_characterModel", "value"), &CreateCharacterModelTransactionResponse::set_characterModel);
	ClassDB::add_property("CreateCharacterModelTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "characterModel"), "set_characterModel", "get_characterModel");
	ClassDB::bind_method(D_METHOD("to_dict"), &CreateCharacterModelTransactionResponse::to_dict);
}
Dictionary CreateCharacterModelTransactionResponse::to_dict() {
	Dictionary res;

	if (tx.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
		if (ptr) {
			res["tx"] = ptr->to_dict();
		}
	}
	res["characterModel"] = characterModel;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot