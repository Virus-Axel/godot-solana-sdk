#include "honeycomb/types/MultiplierType.hpp"


namespace godot {
namespace honeycomb_resource {

void MultiplierType::set_kind(const String& val) {
this->kind = val;
}

String MultiplierType::get_kind() const {
return this->kind;
}

void MultiplierType::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary MultiplierType::get_params() const {
return this->params;
}

void MultiplierType::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &MultiplierType::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &MultiplierType::set_kind);
ClassDB::add_property("MultiplierType", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &MultiplierType::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &MultiplierType::set_params);
ClassDB::add_property("MultiplierType", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &MultiplierType::to_dict);
}
Dictionary MultiplierType::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot