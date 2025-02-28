#include "honeycomb/types/SerializableActions_generated.hpp"


namespace godot {
namespace honeycomb_resource {

void SerializableActions::set_kind(const String& val) {
this->kind = val;
}

String SerializableActions::get_kind() const {
return this->kind;
}

void SerializableActions::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_kind"), &SerializableActions::get_kind);
ClassDB::bind_method(D_METHOD("set_kind", "value"), &SerializableActions::set_kind);
ClassDB::add_property("SerializableActions", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
ClassDB::bind_method(D_METHOD("to_dict"), &SerializableActions::to_dict);
}
Dictionary SerializableActions::to_dict() {
Dictionary res;
if (!kind.is_empty()) {
    res["kind"] = kind;
}
return res;
}
} // namespace honeycomb_resource
} // namespace godot