#include "honeycomb/types/ModifyDelegationInput_generated.hpp"

#include "honeycomb/types/ModifyServiceDelegationInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ModifyDelegationInput::set_delegation(const Variant& val) {
this->delegation = val;
}

Variant ModifyDelegationInput::get_delegation() const {
return this->delegation;
}

void ModifyDelegationInput::set_index(const int32_t& val) {
this->index = val;
}

int32_t ModifyDelegationInput::get_index() const {
return this->index;
}

void ModifyDelegationInput::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_delegation"), &ModifyDelegationInput::get_delegation);
ClassDB::bind_method(D_METHOD("set_delegation", "value"), &ModifyDelegationInput::set_delegation);
ClassDB::add_property("ModifyDelegationInput", PropertyInfo(Variant::Type::OBJECT, "delegation"), "set_delegation", "get_delegation");
ClassDB::bind_method(D_METHOD("get_index"), &ModifyDelegationInput::get_index);
ClassDB::bind_method(D_METHOD("set_index", "value"), &ModifyDelegationInput::set_index);
ClassDB::add_property("ModifyDelegationInput", PropertyInfo(Variant::Type::INT, "index"), "set_index", "get_index");
ClassDB::bind_method(D_METHOD("to_dict"), &ModifyDelegationInput::to_dict);
}
Dictionary ModifyDelegationInput::to_dict() {
Dictionary res;

          if (delegation.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ModifyServiceDelegationInput>(delegation);
              if (ptr) {
                  res["delegation"] = ptr->to_dict();
              }
          }
res["index"] = index;
return res;
}
} // namespace honeycomb_resource
} // namespace godot