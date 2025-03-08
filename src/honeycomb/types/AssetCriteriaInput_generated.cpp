#include "honeycomb/types/AssetCriteriaInput_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void AssetCriteriaInput::set_kind(const String& val) {
this->kind = val;
}

String AssetCriteriaInput::get_kind() const {
return this->kind;
}

void AssetCriteriaInput::set_params(const PackedByteArray& val) {
this->params = val;
}

PackedByteArray AssetCriteriaInput::get_params() const {
return this->params;
}

void AssetCriteriaInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &AssetCriteriaInput::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &AssetCriteriaInput::set_kind);
ClassDB::add_property("AssetCriteriaInput", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("get_params"), &AssetCriteriaInput::get_params);
ClassDB::bind_method(D_METHOD("set_params", "value"), &AssetCriteriaInput::set_params);
ClassDB::add_property("AssetCriteriaInput", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "params"), "set_params", "get_params");
ClassDB::bind_method(D_METHOD("to_dict"), &AssetCriteriaInput::to_dict);
}
Dictionary AssetCriteriaInput::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
res["params"] = params;
return res;
}
} // namespace honeycomb_resource
} // namespace godot