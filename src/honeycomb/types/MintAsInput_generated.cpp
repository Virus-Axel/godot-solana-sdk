#include "honeycomb/types/MintAsInput_generated.hpp"

#include "honeycomb/enums_generated.hpp"
#include "honeycomb/types/MintAsMplBubblegumInput_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void MintAsInput::set_kind(const String &val) {
	this->kind = val;
}

String MintAsInput::get_kind() const {
	return this->kind;
}

void MintAsInput::set_mplBubblegum(const Variant &val) {
	this->mplBubblegum = val;
}

Variant MintAsInput::get_mplBubblegum() const {
	return this->mplBubblegum;
}

void MintAsInput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_kind"), &MintAsInput::get_kind);
	ClassDB::bind_method(D_METHOD("set_kind", "value"), &MintAsInput::set_kind);
	ClassDB::add_property("MintAsInput", PropertyInfo(Variant::STRING, "kind"), "set_kind", "get_kind");
	ClassDB::bind_method(D_METHOD("get_mplBubblegum"), &MintAsInput::get_mplBubblegum);
	ClassDB::bind_method(D_METHOD("set_mplBubblegum", "value"), &MintAsInput::set_mplBubblegum);
	ClassDB::add_property("MintAsInput", PropertyInfo(Variant::Type::OBJECT, "mplBubblegum"), "set_mplBubblegum", "get_mplBubblegum");
	ClassDB::bind_method(D_METHOD("to_dict"), &MintAsInput::to_dict);
}
Dictionary MintAsInput::to_dict() {
	Dictionary res;
	res["kind"] = this->kind;

	if (mplBubblegum.get_type() != Variant::NIL) {
		auto *ptr = Object::cast_to<godot::honeycomb_resource::MintAsMplBubblegumInput>(mplBubblegum);
		if (ptr) {
			res["mplBubblegum"] = ptr->to_dict();
		}
	}
	return res;
}
} // namespace honeycomb_resource
} // namespace godot