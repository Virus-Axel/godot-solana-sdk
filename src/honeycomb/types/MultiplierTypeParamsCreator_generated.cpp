#include "honeycomb/types/MultiplierTypeParamsCreator_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void MultiplierTypeParamsCreator::set_creator(const Variant &val) {
	this->creator = val;
}

Variant MultiplierTypeParamsCreator::get_creator() const {
	return this->creator;
}

void MultiplierTypeParamsCreator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_creator"), &MultiplierTypeParamsCreator::get_creator);
	ClassDB::bind_method(D_METHOD("set_creator", "value"), &MultiplierTypeParamsCreator::set_creator);
	ClassDB::add_property("MultiplierTypeParamsCreator", PropertyInfo(Variant::Type::OBJECT, "creator"), "set_creator", "get_creator");
	ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierTypeParamsCreator::to_dict);
}
Dictionary MultiplierTypeParamsCreator::to_dict() {
	Dictionary res;
	if (creator.has_method("to_string")) {
		res["creator"] = creator.call("to_string");
	} else {
		res["creator"] = creator;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot