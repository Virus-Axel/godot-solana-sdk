#include "honeycomb/types/UsedByGuild.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/GuildRole.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByGuild::set_id(const Variant& val) {
this->id = val;
}

Variant UsedByGuild::get_id() const {
return this->id;
}

void UsedByGuild::set_role(const Variant& val) {
this->role = val;
}

Variant UsedByGuild::get_role() const {
return this->role;
}

void UsedByGuild::set_order(const int32_t& val) {
this->order = val;
}

int32_t UsedByGuild::get_order() const {
return this->order;
}

void UsedByGuild::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_id"), &UsedByGuild::get_id);
ClassDB::bind_method(D_METHOD("set_id", "value"), &UsedByGuild::set_id);
ClassDB::add_property("UsedByGuild", PropertyInfo(Variant::Type::OBJECT, "id"), "set_id", "get_id");
ClassDB::bind_method(D_METHOD("get_role"), &UsedByGuild::get_role);
ClassDB::bind_method(D_METHOD("set_role", "value"), &UsedByGuild::set_role);
ClassDB::add_property("UsedByGuild", PropertyInfo(Variant::Type::OBJECT, "role"), "set_role", "get_role");
ClassDB::bind_method(D_METHOD("get_order"), &UsedByGuild::get_order);
ClassDB::bind_method(D_METHOD("set_order", "value"), &UsedByGuild::set_order);
ClassDB::add_property("UsedByGuild", PropertyInfo(Variant::Type::INT, "order"), "set_order", "get_order");
ClassDB::bind_method(D_METHOD("to_dict"), &UsedByGuild::to_dict);
}
Dictionary UsedByGuild::to_dict() {
Dictionary res;
if (id.has_method("to_string")) {
res["id"] = id.call("to_string");
} else {
res["id"] = id;
}

          if (role.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::GuildRole>(role);
              if (ptr) {
                  res["role"] = ptr->to_dict();
              }
          }
res["order"] = order;
return res;
}
} // namespace honeycomb_resource
} // namespace godot