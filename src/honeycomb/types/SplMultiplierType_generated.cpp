#include "honeycomb/types/SplMultiplierType_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplMultiplierType::set_kind(const String &val) {
	this->kind = val;
}

String SplMultiplierType::get_kind() const {
	return this->kind;
}

void SplMultiplierType::set_params(const Dictionary &val) {
	this->params = val;
}

Dictionary SplMultiplierType::get_params() const {
	return this->params;
}

void SplMultiplierType::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &SplMultiplierType::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &SplMultiplierType::set_kind);
	ClassDB::add_property("SplMultiplierType", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_params"), &SplMultiplierType::get_params);
	ClassDB::bind_method(D_METHOD("set_params", "value"), &SplMultiplierType::set_params);
	ClassDB::add_property("SplMultiplierType", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplMultiplierType::to_dict);
}
Dictionary SplMultiplierType::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	res["params"] = params;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot