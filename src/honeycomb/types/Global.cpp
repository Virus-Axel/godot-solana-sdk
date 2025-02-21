#include "honeycomb/types/Global.hpp"

#include "honeycomb/types/ControlledMerkleTrees.hpp"

namespace godot {
namespace honeycomb_resource {

void Global::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Global::get_address() const {
return this->address;
}

void Global::set_bump(const int32_t& val) {
this->bump = val;
}

int32_t Global::get_bump() const {
return this->bump;
}

void Global::set_config(const Array& val) {
this->config = val;
}

Array Global::get_config() const {
return this->config;
}

void Global::set_userTrees(const Variant& val) {
this->userTrees = val;
}

Variant Global::get_userTrees() const {
return this->userTrees;
}

void Global::set_totalUsers(const int32_t& val) {
this->totalUsers = val;
}

int32_t Global::get_totalUsers() const {
return this->totalUsers;
}

void Global::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Global::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Global::set_address);
ClassDB::add_property("Global", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_bump"), &Global::get_bump);
ClassDB::bind_method(D_METHOD("set_bump", "value"), &Global::set_bump);
ClassDB::add_property("Global", PropertyInfo(Variant::Type::INT, "bump"), "set_bump", "get_bump");
ClassDB::bind_method(D_METHOD("get_config"), &Global::get_config);
ClassDB::bind_method(D_METHOD("set_config", "value"), &Global::set_config);
ClassDB::add_property("Global", PropertyInfo(Variant::Type::ARRAY, "config"), "set_config", "get_config");
ClassDB::bind_method(D_METHOD("get_userTrees"), &Global::get_userTrees);
ClassDB::bind_method(D_METHOD("set_userTrees", "value"), &Global::set_userTrees);
ClassDB::add_property("Global", PropertyInfo(Variant::Type::OBJECT, "userTrees"), "set_userTrees", "get_userTrees");
ClassDB::bind_method(D_METHOD("get_totalUsers"), &Global::get_totalUsers);
ClassDB::bind_method(D_METHOD("set_totalUsers", "value"), &Global::set_totalUsers);
ClassDB::add_property("Global", PropertyInfo(Variant::Type::INT, "totalUsers"), "set_totalUsers", "get_totalUsers");
ClassDB::bind_method(D_METHOD("to_dict"), &Global::to_dict);
}
Dictionary Global::to_dict() {
Dictionary res;
res["address"] = address;
res["bump"] = bump;
res["config"] = config;

          if (userTrees.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::ControlledMerkleTrees>(userTrees);
              if (ptr) {
                  res["userTrees"] = ptr->to_dict();
              }
          }
res["totalUsers"] = totalUsers;
return res;
}
} // namespace honeycomb_resource
} // namespace godot