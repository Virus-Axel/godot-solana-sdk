#include "honeycomb/types/Recipe_generated.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/Meal_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void Recipe::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Recipe::get_address() const {
return this->address;
}

void Recipe::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t Recipe::get_bump() const {
return this->bump;
}

void Recipe::set_project(const Variant& val) {
this->project = val;
}

Variant Recipe::get_project() const {
return this->project;
}

void Recipe::set_key(const Variant& val) {
this->key = val;
}

Variant Recipe::get_key() const {
return this->key;
}

void Recipe::set_xp(const int64_t& val) {
this->xp = val;
}

int64_t Recipe::get_xp() const {
return this->xp;
}

void Recipe::set_ingredients(const Array& val) {
this->ingredients = val;
}

Array Recipe::get_ingredients() const {
return this->ingredients;
}

void Recipe::set_meal(const Variant& val) {
this->meal = val;
}

Variant Recipe::get_meal() const {
return this->meal;
}

void Recipe::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Recipe::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Recipe::set_address);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &Recipe::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &Recipe::set_bump);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_project"), &Recipe::get_project);
ClassDB::bind_method(D_METHOD("set_project", "value"), &Recipe::set_project);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::OBJECT, "project"), "set_project", "get_project");
ClassDB::bind_method(D_METHOD("get_key"), &Recipe::get_key);
ClassDB::bind_method(D_METHOD("set_key", "value"), &Recipe::set_key);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::OBJECT, "key"), "set_key", "get_key");
ClassDB::bind_method(D_METHOD("get_xp"), &Recipe::get_xp);
ClassDB::bind_method(D_METHOD("set_xp", "value"), &Recipe::set_xp);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::INT, "xp"), "set_xp", "get_xp");
ClassDB::bind_method(D_METHOD("get_ingredients"), &Recipe::get_ingredients);
ClassDB::bind_method(D_METHOD("set_ingredients", "value"), &Recipe::set_ingredients);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::ARRAY, "ingredients"), "set_ingredients", "get_ingredients");
ClassDB::bind_method(D_METHOD("get_meal"), &Recipe::get_meal);
ClassDB::bind_method(D_METHOD("set_meal", "value"), &Recipe::set_meal);
ClassDB::add_property("Recipe", PropertyInfo(Variant::Type::OBJECT, "meal"), "set_meal", "get_meal");
ClassDB::bind_method(D_METHOD("to_dict"), &Recipe::to_dict);
}
Dictionary Recipe::to_dict() {
Dictionary res;
res["address"] = address;
res["bump"] = bump;
if (project.has_method("to_string")) {
res["project"] = project.call("to_string");
} else {
res["project"] = project;
}
if (key.has_method("to_string")) {
res["key"] = key.call("to_string");
} else {
res["key"] = key;
}
res["xp"] = xp;
res["ingredients"] = ingredients;

          if (meal.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Meal>(meal);
              if (ptr) {
                  res["meal"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot