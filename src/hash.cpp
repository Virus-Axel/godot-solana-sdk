#include "hash.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Hash::_update_pointer(){
    if (unique){
        data_pointer = create_unique_hash();
    }
    else if (bytes.size() == HASH_LENGTH){
        data_pointer = create_hash_from_array(bytes.ptr());
    }
}

void Hash::_free_pointer(){
    free_hash(data_pointer);
}

void Hash::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_value"), &Hash::get_value);
    ClassDB::bind_method(D_METHOD("set_value", "p_value"), &Hash::set_value);
    ClassDB::bind_method(D_METHOD("get_bytes"), &Hash::get_bytes);
    ClassDB::bind_method(D_METHOD("set_bytes", "p_value"), &Hash::set_bytes);
    ClassDB::bind_method(D_METHOD("get_unique"), &Hash::get_unique);
    ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Hash::set_unique);
}


bool Hash::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "value") {
		set_value(p_value);
		return true;
	}
    else if(name == "bytes"){
        set_bytes(p_value);
		return true;
    }
    else if(name == "unique"){
        set_unique(p_value);
        return true;
    }
	return false;
}

bool Hash::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "value") {
		r_ret = value;
		return true;
	}
    else if (name == "bytes") {
		r_ret = bytes;
		return true;
	}
    else if (name == "unique") {
		r_ret = unique;
		return true;
	}
	return false;
}

void Hash::set_value(const String& p_value){
    value = p_value;

    // Hash is not unique if value is set.
    unique = false;

    // Update byte representation accordingly.
    PackedByteArray decoded_value = bs58_decode(value);
    bytes = decoded_value;

    // Print warnings if bytes are bad.
    if(decoded_value.is_empty() && value.length() != 0){
        internal::gde_interface->print_warning("Value contains non-base58 characters", "_set", "pubkey.cpp", 18, false);
    }
    else if (decoded_value.size() != HASH_LENGTH){
        internal::gde_interface->print_warning("Hash must be 32 bytes", "_set", "hash.cpp", 78, false);
    }
}

String Hash::get_value(){
    return value;
}

void Hash::set_bytes(const PackedByteArray& p_value){
    bytes = p_value;

    // Hash is not unique anymore.
    unique = false;

    // Empty byte array could give 1 as encoded string. Avoid this.
    if (bytes.size() == 0){
        value = "";
    }
    else{
        String encoded_value = bs58_encode(bytes);
        value = encoded_value;
    }
    
    // Print warning if key is bad
    if (bytes.size() != HASH_LENGTH){
        internal::gde_interface->print_warning("Hash must be 32 bytes", "_set", "hash.cpp", 105, false);
    }
}
PackedByteArray Hash::get_bytes(){
    return bytes;
}

void Hash::set_unique(const bool p_value){
    unique = p_value;
    notify_property_list_changed();
}

bool Hash::get_unique(){
    return unique;
}

void Hash::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;

    // Make other properties invisible if key is unique
    if(unique){
        visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    
    p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

Hash::Hash() {
}

Hash::~Hash() {
    _free_pointer_if_not_null();
}

}