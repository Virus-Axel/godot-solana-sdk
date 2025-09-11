#include "meta_data/creator.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"

#include "pubkey.hpp"

namespace godot {

void MetaDataCreator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_address"), &MetaDataCreator::get_address);
	ClassDB::bind_method(D_METHOD("set_address", "p_value"), &MetaDataCreator::set_address);
	ClassDB::bind_method(D_METHOD("get_verified"), &MetaDataCreator::get_verified);
	ClassDB::bind_method(D_METHOD("set_verified", "p_value"), &MetaDataCreator::set_verified);
	ClassDB::bind_method(D_METHOD("get_share"), &MetaDataCreator::get_share);
	ClassDB::bind_method(D_METHOD("set_share", "p_value"), &MetaDataCreator::set_share);

	ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::OBJECT, "address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,PhantomController", PROPERTY_USAGE_DEFAULT), "set_address", "get_address");
	ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::BOOL, "verified", PROPERTY_HINT_NONE), "set_verified", "get_verified");
	ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::INT, "share", PROPERTY_HINT_NONE), "set_share", "get_share");
}

void MetaDataCreator::set_address(const Variant &p_value) {
	address = p_value;
}
Variant MetaDataCreator::get_address() const {
	return address;
}

void MetaDataCreator::set_verified(const bool p_value) {
	verified = p_value;
}
bool MetaDataCreator::get_verified() const {
	return verified;
}

void MetaDataCreator::set_share(const uint32_t p_value) {
	share = p_value;
}
uint32_t MetaDataCreator::get_share() const {
	return share;
}

PackedByteArray MetaDataCreator::serialize() const {
	PackedByteArray res;
	res.append_array(Pubkey::bytes_from_variant(address));
	res.append(static_cast<uint8_t>(verified));
	res.append(share);

	return res;
}

} //namespace godot