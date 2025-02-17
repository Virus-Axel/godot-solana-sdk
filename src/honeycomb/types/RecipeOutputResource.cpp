#include "honeycomb/types/RecipeOutputResource.hpp"


namespace godot {
namespace honeycomb_resource {

void RecipeOutputResource::set_resource(const String& val) {
this->resource = val;
}

String RecipeOutputResource::get_resource() const {
return this->resource;
}

void RecipeOutputResource::set_amount(const int32_t& val) {
this->amount = val;
}

int32_t RecipeOutputResource::get_amount() const {
return this->amount;
}

void RecipeOutputResource::set_characteristics(const Dictionary& val) {
this->characteristics = val;
}

Dictionary RecipeOutputResource::get_characteristics() const {
return this->characteristics;
}

void RecipeOutputResource::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_resource"), &RecipeOutputResource::get_resource);
ClassDB::bind_method(D_METHOD("set_resource", "value"), &RecipeOutputResource::set_resource);
ClassDB::add_property("RecipeOutputResource", PropertyInfo(Variant::Type::STRING, "resource"), "set_resource", "get_resource");
ClassDB::bind_method(D_METHOD("get_amount"), &RecipeOutputResource::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &RecipeOutputResource::set_amount);
ClassDB::add_property("RecipeOutputResource", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("get_characteristics"), &RecipeOutputResource::get_characteristics);
ClassDB::bind_method(D_METHOD("set_characteristics", "value"), &RecipeOutputResource::set_characteristics);
ClassDB::add_property("RecipeOutputResource", PropertyInfo(Variant::Type::DICTIONARY, "characteristics"), "set_characteristics", "get_characteristics");
ClassDB::bind_method(D_METHOD("to_dict"), &RecipeOutputResource::to_dict);
}
Dictionary RecipeOutputResource::to_dict() {
Dictionary res;
if (!resource.is_empty()) {
    res["resource"] = resource;
}
res["amount"] = amount;
res["characteristics"] = characteristics;
return res;
}
} // namespace honeycomb_resource
} // namespace godot