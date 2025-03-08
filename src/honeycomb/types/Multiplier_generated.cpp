#include "honeycomb/types/Multiplier_generated.hpp"

#include "honeycomb/types/MultiplierType_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Multiplier::set_value(const int64_t &val) {
	this->value = val;
}

int64_t Multiplier::get_value() const {
	return this->value;
}

void Multiplier::set_multiplierType(const Variant &val) {
	this->multiplierType = val;
}

Variant Multiplier::get_multiplierType() const {
	return this->multiplierType;
}

void Multiplier::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_value"), &Multiplier::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &Multiplier::set_value);
	ClassDB::add_property("Multiplier", PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
	ClassDB::bind_method(D_METHOD("get_multiplierType"), &Multiplier::get_multiplierType);
	ClassDB::bind_method(D_METHOD("set_multiplierType", "value"), &Multiplier::set_multiplierType);
	ClassDB::add_property("Multiplier", PropertyInfo(Variant::Type::OBJECT, "multiplierType"), "set_multiplierType", "get_multiplierType");
	ClassDB::bind_method(D_METHOD("to_dict"), &Multiplier::to_dict);
}
Dictionary Multiplier::to_dict() {
	Dictionary res;
	res["value"] = value;

	if (multiplierType.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::MultiplierType>(multiplierType);
		if (ptr) {
			res["multiplierType"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot