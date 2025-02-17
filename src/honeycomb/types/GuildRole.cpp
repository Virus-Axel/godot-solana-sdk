#include "honeycomb/types/GuildRole.hpp"


namespace godot {
namespace honeycomb_resource {

void GuildRole::set_kind(const String& val) {
this->kind = val;
}

String GuildRole::get_kind() const {
return this->kind;
}

void GuildRole::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &GuildRole::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &GuildRole::set_kind);
ClassDB::add_property("GuildRole", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("to_dict"), &GuildRole::to_dict);
}
Dictionary GuildRole::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot