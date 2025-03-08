#include "honeycomb/types/SerializableActionsInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void SerializableActionsInput::set_kind(const String &val) {
	this->kind = val;
}

String SerializableActionsInput::get_kind() const {
	return this->kind;
}

void SerializableActionsInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &SerializableActionsInput::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &SerializableActionsInput::set_kind);
	ClassDB::add_property("SerializableActionsInput", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("to_dict"), &SerializableActionsInput::to_dict);
}
Dictionary SerializableActionsInput::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot