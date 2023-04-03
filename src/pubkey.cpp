#include "pubkey.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Pubkey::_update_pointer(){
    if (unique){
        data_pointer = create_unique_pubkey();
    }
    else if (bytes.size() == PUBKEY_LENGTH){
        // Allocate space for rust to free
        unsigned char* key_array = new unsigned char[PUBKEY_LENGTH];

        memcpy(key_array, bytes.ptr(), PUBKEY_LENGTH);

        data_pointer = create_pubkey_from_array(key_array);
    }
}

void Pubkey::_free_pointer(){
    free_pubkey(data_pointer);
}

void Pubkey::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_value"), &Pubkey::get_value);
    ClassDB::bind_method(D_METHOD("set_value", "p_value"), &Pubkey::set_value);
    ClassDB::bind_method(D_METHOD("get_bytes"), &Pubkey::get_bytes);
    ClassDB::bind_method(D_METHOD("set_bytes", "p_value"), &Pubkey::set_bytes);
    ClassDB::bind_method(D_METHOD("get_unique"), &Pubkey::get_unique);
    ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Pubkey::set_unique);
}


bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
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

bool Pubkey::_get(const StringName &p_name, Variant &r_ret) const {
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

void Pubkey::set_value(const String& p_value){
    value = p_value;
    PackedByteArray decoded_value = bs58_decode(value);
    bytes = decoded_value;
    if(decoded_value.is_empty() && value.length() != 0){
        internal::gde_interface->print_warning("Value contains non-base58 characters", "_set", "pubkey.cpp", 18, false);
    }
    else if (decoded_value.size() != 32){
        internal::gde_interface->print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 21, false);
    }
}

String Pubkey::get_value(){
    return value;
}

void Pubkey::set_bytes(const PackedByteArray& p_value){
    bytes = p_value;
    if (bytes.size() == 0){
        value = "";
        return;
    }
    String encoded_value = bs58_encode(bytes);
    value = encoded_value;
    if (bytes.size() != 32){
        internal::gde_interface->print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 14, false);
    }
}
PackedByteArray Pubkey::get_bytes(){
    return bytes;
}

void Pubkey::set_unique(const bool p_value){
    unique = p_value;
    notify_property_list_changed();
}

bool Pubkey::get_unique(){
    return unique;
}

void Pubkey::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;
    if(unique){
        visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

Pubkey::Pubkey() {
}

Pubkey::~Pubkey() {
    _free_pointer_if_not_null();
}

}