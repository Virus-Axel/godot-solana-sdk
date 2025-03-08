#include "honeycomb/types/ServiceParamsAssetManager_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void ServiceParamsAssetManager::set_assetManagerId(const Variant& val) {
this->assetManagerId = val;
}

Variant ServiceParamsAssetManager::get_assetManagerId() const {
return this->assetManagerId;
}

void ServiceParamsAssetManager::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_assetManagerId"), &ServiceParamsAssetManager::get_assetManagerId);
ClassDB::bind_method(D_METHOD("set_assetManagerId", "value"), &ServiceParamsAssetManager::set_assetManagerId);
ClassDB::add_property("ServiceParamsAssetManager", PropertyInfo(Variant::Type::OBJECT, "assetManagerId"), "set_assetManagerId", "get_assetManagerId");
ClassDB::bind_method(D_METHOD("to_dict"), &ServiceParamsAssetManager::to_dict);
}
Dictionary ServiceParamsAssetManager::to_dict() {
Dictionary res;
if (assetManagerId.has_method("to_string")) {
res["assetManagerId"] = assetManagerId.call("to_string");
} else {
res["assetManagerId"] = assetManagerId;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot