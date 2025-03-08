#include "honeycomb/types/RecipeInputResource_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void RecipeInputResource::set_resource(const String& val) {
this->resource = val;
}

String RecipeInputResource::get_resource() const {
return this->resource;
}

void RecipeInputResource::set_amount(const int32_t& val) {
this->amount = val;
}

int32_t RecipeInputResource::get_amount() const {
return this->amount;
}

void RecipeInputResource::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_resource"), &RecipeInputResource::get_resource);
ClassDB::bind_method(D_METHOD("set_resource", "value"), &RecipeInputResource::set_resource);
ClassDB::add_property("RecipeInputResource", PropertyInfo(Variant::Type::STRING, "resource"), "set_resource", "get_resource");
ClassDB::bind_method(D_METHOD("get_amount"), &RecipeInputResource::get_amount);
ClassDB::bind_method(D_METHOD("set_amount", "value"), &RecipeInputResource::set_amount);
ClassDB::add_property("RecipeInputResource", PropertyInfo(Variant::Type::INT, "amount"), "set_amount", "get_amount");
ClassDB::bind_method(D_METHOD("to_dict"), &RecipeInputResource::to_dict);
}
Dictionary RecipeInputResource::to_dict() {
Dictionary res;
if (!resource.is_empty()) {
    res["resource"] = resource;
}
res["amount"] = amount;
return res;
}
} // namespace honeycomb_resource
} // namespace godot