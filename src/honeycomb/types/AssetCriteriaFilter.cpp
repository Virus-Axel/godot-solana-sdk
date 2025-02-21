#include "honeycomb/types/AssetCriteriaFilter.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void AssetCriteriaFilter::set_kind(const String& val) {
this->kind = val;
}

String AssetCriteriaFilter::get_kind() const {
return this->kind;
}

void AssetCriteriaFilter::set_params(const Variant& val) {
this->params = val;
}

Variant AssetCriteriaFilter::get_params() const {
return this->params;
}

void AssetCriteriaFilter::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &AssetCriteriaFilter::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &AssetCriteriaFilter::set_kind);
ClassDB::add_property("AssetCriteriaFilter", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &AssetCriteriaFilter::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &AssetCriteriaFilter::set_params);
ClassDB::add_property("AssetCriteriaFilter", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &AssetCriteriaFilter::to_dict);
}
Dictionary AssetCriteriaFilter::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
if (params.has_method("to_string")) {
res["params"] = params.call("to_string");
} else {
res["params"] = params;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot