#include "honeycomb/types/ResourceKind.hpp"


namespace godot {
namespace honeycomb_resource {

void ResourceKind::set_kind(const String& val) {
this->kind = val;
}

String ResourceKind::get_kind() const {
return this->kind;
}

void ResourceKind::set_params(const Dictionary& val) {
this->params = val;
}

Dictionary ResourceKind::get_params() const {
return this->params;
}

void ResourceKind::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &ResourceKind::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &ResourceKind::set_kind);
ClassDB::add_property("ResourceKind", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &ResourceKind::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &ResourceKind::set_params);
ClassDB::add_property("ResourceKind", PropertyInfo(Variant::Type::DICTIONARY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &ResourceKind::to_dict);
}
Dictionary ResourceKind::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot