#include "meta_data/collection.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "pubkey.hpp"

namespace godot {

void MetaDataCollection::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_key"), &MetaDataCollection::get_key);
	ClassDB::bind_method(D_METHOD("set_key", "p_value"), &MetaDataCollection::set_key);
	ClassDB::bind_method(D_METHOD("get_verified"), &MetaDataCollection::get_verified);
	ClassDB::bind_method(D_METHOD("set_verified", "p_value"), &MetaDataCollection::set_verified);

	ClassDB::add_property("MetaDataCollection", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,PhantomController", PROPERTY_USAGE_DEFAULT), "set_key", "get_key");
	ClassDB::add_property("MetaDataCollection", PropertyInfo(Variant::BOOL, "verified", PROPERTY_HINT_NONE), "set_verified", "get_verified");
}

void MetaDataCollection::set_key(const Variant &p_value) {
	key = p_value;
}
Variant MetaDataCollection::get_key() const {
	return key;
}

void MetaDataCollection::set_verified(const bool p_value) {
	verified = p_value;
}
bool MetaDataCollection::get_verified() const {
	return verified;
}

PackedByteArray MetaDataCollection::serialize() const {
	PackedByteArray res;
	res.append(static_cast<int64_t>(verified));
	res.append_array(Pubkey::bytes_from_variant(key));

	return res;
}

} //namespace godot