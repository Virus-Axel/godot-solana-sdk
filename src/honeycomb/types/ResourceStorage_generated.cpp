#include "honeycomb/types/ResourceStorage_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "honeycomb/types/ResourceStorageParams_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void ResourceStorage::set_kind(const String &val) {
	this->kind = val;
}

String ResourceStorage::get_kind() const {
	return this->kind;
}

void ResourceStorage::set_params(const Variant &val) {
	this->params = val;
}

Variant ResourceStorage::get_params() const {
	return this->params;
}

void ResourceStorage::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &ResourceStorage::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &ResourceStorage::set_kind);
	ClassDB::add_property("ResourceStorage", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_params"), &ResourceStorage::get_params);
	ClassDB::bind_method(D_METHOD("set_params", "value"), &ResourceStorage::set_params);
	ClassDB::add_property("ResourceStorage", PropertyInfo(Variant::Type::OBJECT, "params"), "set_params", "get_params");
	ClassDB::bind_method(D_METHOD("to_dict"), &ResourceStorage::to_dict);
}
Dictionary ResourceStorage::to_dict() {
	Dictionary res;
	res["kind"] = this->kind;

	if (params.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::ResourceStorageParams>(params);
		if (ptr) {
			res["params"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot