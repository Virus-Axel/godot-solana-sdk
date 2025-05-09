#include "honeycomb/types/SplRewardConfig_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SplRewardConfig::set_kind(const String &val) {
	this->kind = val;
}

String SplRewardConfig::get_kind() const {
	return this->kind;
}

void SplRewardConfig::set_params(const Dictionary &val) {
	this->params = val;
}

Dictionary SplRewardConfig::get_params() const {
	return this->params;
}

void SplRewardConfig::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &SplRewardConfig::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &SplRewardConfig::set_kind);
	ClassDB::add_property("SplRewardConfig", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_params"), &SplRewardConfig::get_params);
	ClassDB::bind_method(D_METHOD("set_params", "value"), &SplRewardConfig::set_params);
	ClassDB::add_property("SplRewardConfig", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
	ClassDB::bind_method(D_METHOD("to_dict"), &SplRewardConfig::to_dict);
}
Dictionary SplRewardConfig::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	res["params"] = params;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot