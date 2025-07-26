#include "honeycomb/types/AddSplMultiplierMetadataInput_generated.hpp"

#include "honeycomb/types/SplMultiplierInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void AddSplMultiplierMetadataInput::set_value(const int64_t &val) {
	this->value = val;
}

int64_t AddSplMultiplierMetadataInput::get_value() const {
	return this->value;
}

void AddSplMultiplierMetadataInput::set_type(const Variant &val) {
	this->type = val;
}

Variant AddSplMultiplierMetadataInput::get_type() const {
	return this->type;
}

void AddSplMultiplierMetadataInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_value"), &AddSplMultiplierMetadataInput::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &AddSplMultiplierMetadataInput::set_value);
	ClassDB::add_property("AddSplMultiplierMetadataInput", PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
	ClassDB::bind_method(D_METHOD("get_type"), &AddSplMultiplierMetadataInput::get_type);
	ClassDB::bind_method(D_METHOD("set_type", "value"), &AddSplMultiplierMetadataInput::set_type);
	ClassDB::add_property("AddSplMultiplierMetadataInput", PropertyInfo(Variant::Type::OBJECT, "type"), "set_type", "get_type");
	ClassDB::bind_method(D_METHOD("to_dict"), &AddSplMultiplierMetadataInput::to_dict);
}
Dictionary AddSplMultiplierMetadataInput::to_dict() {
	Dictionary res;
	res["value"] = value;

	if (type.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::SplMultiplierInput>(type);
		if (ptr) {
			res["type"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot