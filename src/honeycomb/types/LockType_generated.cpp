#include "honeycomb/types/LockType_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void LockType::set_kind(const String &val) {
	this->kind = val;
}

String LockType::get_kind() const {
	return this->kind;
}

void LockType::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &LockType::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &LockType::set_kind);
	ClassDB::add_property("LockType", PropertyInfo(Variant::Type::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("to_dict"), &LockType::to_dict);
}
Dictionary LockType::to_dict() {
	Dictionary res;
	if (!kind.is_empty()) {
		res["kind"] = kind;
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot