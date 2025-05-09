#include "honeycomb/types/StakerClaimed_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void StakerClaimed::set_kind(const String &val) {
	this->kind = val;
}

String StakerClaimed::get_kind() const {
	return this->kind;
}

void StakerClaimed::set_params(const Dictionary &val) {
	this->params = val;
}

Dictionary StakerClaimed::get_params() const {
	return this->params;
}

void StakerClaimed::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &StakerClaimed::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &StakerClaimed::set_kind);
	ClassDB::add_property("StakerClaimed", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_params"), &StakerClaimed::get_params);
	ClassDB::bind_method(D_METHOD("set_params", "value"), &StakerClaimed::set_params);
	ClassDB::add_property("StakerClaimed", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
	ClassDB::bind_method(D_METHOD("to_dict"), &StakerClaimed::to_dict);
}
Dictionary StakerClaimed::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	res["params"] = params;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot