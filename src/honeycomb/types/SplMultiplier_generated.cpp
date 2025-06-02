#include "honeycomb/types/SplMultiplier_generated.hpp"

#include "honeycomb/types/SplMultiplierType_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplMultiplier::set_value(const int64_t &val) {
	this->value = val;
}

int64_t SplMultiplier::get_value() const {
	return this->value;
}

void SplMultiplier::set_multiplierType(const Variant &val) {
	this->multiplierType = val;
}

Variant SplMultiplier::get_multiplierType() const {
	return this->multiplierType;
}

void SplMultiplier::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_value"), &SplMultiplier::get_value);
	ClassDB::bind_method(D_METHOD("set_value", "value"), &SplMultiplier::set_value);
	ClassDB::add_property("SplMultiplier", PropertyInfo(Variant::Type::INT, "value"), "set_value", "get_value");
	ClassDB::bind_method(D_METHOD("get_multiplierType"), &SplMultiplier::get_multiplierType);
	ClassDB::bind_method(D_METHOD("set_multiplierType", "value"), &SplMultiplier::set_multiplierType);
	ClassDB::add_property("SplMultiplier", PropertyInfo(Variant::Type::OBJECT, "multiplierType"), "set_multiplierType", "get_multiplierType");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplMultiplier::to_dict);
}
Dictionary SplMultiplier::to_dict() {
	Dictionary res;
	res["value"] = value;

	if (multiplierType.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::SplMultiplierType>(multiplierType);
		if (ptr) {
			res["multiplierType"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot