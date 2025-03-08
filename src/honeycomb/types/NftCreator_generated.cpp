#include "honeycomb/types/NftCreator_generated.hpp"

#include "pubkey.hpp"

namespace godot {
namespace honeycomb_resource {

void NftCreator::set_address(const Variant &val) {
	this->address = val;
}

Variant NftCreator::get_address() const {
	return this->address;
}

void NftCreator::set_share(const int32_t &val) {
	this->share = val;
}

int32_t NftCreator::get_share() const {
	return this->share;
}

void NftCreator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &NftCreator::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "value"), &NftCreator::set_address);
	ClassDB::add_property("NftCreator", PropertyInfo(Variant::Type::OBJECT, "address"), "set_address", "get_address");
	ClassDB::bind_method(D_METHOD("get_share"), &NftCreator::get_share);
	ClassDB::bind_method(D_METHOD("set_share", "value"), &NftCreator::set_share);
	ClassDB::add_property("NftCreator", PropertyInfo(Variant::Type::INT, "share"), "set_share", "get_share");
	ClassDB::bind_method(D_METHOD("to_dict"), &NftCreator::to_dict);
}
Dictionary NftCreator::to_dict() {
	Dictionary res;
	if (address.has_method("to_string")) {
		res["address"] = address.call("to_string");
	} else {
		res["address"] = address;
	}
	res["share"] = share;
	return res;
}
} // namespace honeycomb_resource
} // namespace godot