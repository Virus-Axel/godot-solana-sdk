#include "honeycomb/types/AssetCriteria.hpp"

#include "honeycomb/enums.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void AssetCriteria::set_kind(const String& val) {
    this->kind = val;
}

String AssetCriteria::get_kind() const {
    return this->kind;
}

void AssetCriteria::set_params(const Variant& val) {
this->params = val;
}

Variant AssetCriteria::get_params() const {
return this->params;
}

void AssetCriteria::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &AssetCriteria::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &AssetCriteria::set_kind);
ClassDB::add_property("AssetCriteria", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &AssetCriteria::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &AssetCriteria::set_params);
ClassDB::add_property("AssetCriteria", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &AssetCriteria::to_dict);
}
Dictionary AssetCriteria::to_dict() {
Dictionary res;
res["kind"] = this->kind;
if (params.has_method("to_string")) {
res["params"] = params.call("to_string");
} else {
res["params"] = params;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot