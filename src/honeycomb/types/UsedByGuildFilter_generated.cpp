#include "honeycomb/types/UsedByGuildFilter_generated.hpp"

#include "honeycomb/types/GuildRoleFilter_generated.hpp"
#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void UsedByGuildFilter::set_id(const Variant &val) {
	this->id = val;
}

Variant UsedByGuildFilter::get_id() const {
	return this->id;
}

void UsedByGuildFilter::set_role(const Variant &val) {
	this->role = val;
}

Variant UsedByGuildFilter::get_role() const {
	return this->role;
}

void UsedByGuildFilter::set_order(const int32_t &val) {
	this->order = val;
}

int32_t UsedByGuildFilter::get_order() const {
	return this->order;
}

void UsedByGuildFilter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_id"), &UsedByGuildFilter::get_id);
	ClassDB::bind_method(D_METHOD("set_id", "value"), &UsedByGuildFilter::set_id);
	ClassDB::add_property("UsedByGuildFilter", PropertyInfo(Variant::Type::OBJECT, "id"), "set_id", "get_id");
	ClassDB::bind_method(D_METHOD("get_role"), &UsedByGuildFilter::get_role);
	ClassDB::bind_method(D_METHOD("set_role", "value"), &UsedByGuildFilter::set_role);
	ClassDB::add_property("UsedByGuildFilter", PropertyInfo(Variant::Type::OBJECT, "role"), "set_role", "get_role");
	ClassDB::bind_method(D_METHOD("get_order"), &UsedByGuildFilter::get_order);
	ClassDB::bind_method(D_METHOD("set_order", "value"), &UsedByGuildFilter::set_order);
	ClassDB::add_property("UsedByGuildFilter", PropertyInfo(Variant::Type::INT, "order"), "set_order", "get_order");
	ClassDB::bind_method(D_METHOD("to_dict"), &UsedByGuildFilter::to_dict);
}
Dictionary UsedByGuildFilter::to_dict() {
	Dictionary res;
	if (id.has_method("to_string")) {
		res["id"] = id.call("to_string");
	} else {
		res["id"] = id;
	}

	if (role.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::GuildRoleFilter>(role);
		if (ptr) {
			res["role"] = ptr->to_dict();
		}
	}
	res["order"] = order;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot