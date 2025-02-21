#include "honeycomb/types/RecipeResources.hpp"

#include "honeycomb/types/RecipeInputResources.hpp"
#include "honeycomb/types/RecipeOutputResource.hpp"

namespace godot {
namespace honeycomb_resource {

void RecipeResources::set_input(const Variant& val) {
this->input = val;
}

Variant RecipeResources::get_input() const {
return this->input;
}

void RecipeResources::set_output(const Variant& val) {
this->output = val;
}

Variant RecipeResources::get_output() const {
return this->output;
}

void RecipeResources::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_input"), &RecipeResources::get_input);
ClassDB::bind_method(D_METHOD("set_input", "value"), &RecipeResources::set_input);
ClassDB::add_property("RecipeResources", PropertyInfo(Variant::Type::OBJECT, "input"), "set_input", "get_input");
ClassDB::bind_method(D_METHOD("get_output"), &RecipeResources::get_output);
ClassDB::bind_method(D_METHOD("set_output", "value"), &RecipeResources::set_output);
ClassDB::add_property("RecipeResources", PropertyInfo(Variant::Type::OBJECT, "output"), "set_output", "get_output");
ClassDB::bind_method(D_METHOD("to_dict"), &RecipeResources::to_dict);
}
Dictionary RecipeResources::to_dict() {
Dictionary res;

          if (input.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::RecipeInputResources>(input);
              if (ptr) {
                  res["input"] = ptr->to_dict();
              }
          }

          if (output.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::RecipeOutputResource>(output);
              if (ptr) {
                  res["output"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot