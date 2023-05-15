#include "pubkey.hpp"
#include "utils.hpp"

#include <solana_sdk.hpp>
#include <godot_cpp/core/class_db.hpp>

using internal::gde_interface;

namespace godot{

void Pubkey::_update_pointer(){
    if (type == "UNIQUE"){
        data_pointer = create_unique_pubkey();
    }
    else if (type == "SEED"){
        void *base_ptr = variant_to_type<Pubkey>(base);
        if(base_ptr == nullptr){
            gde_interface->print_warning("Bad base pubkey", "_update_pointer", "pubkey.cpp", 18, false);
            return;
        }

        void *owner_ptr = variant_to_type<Pubkey>(owner);
        if(owner_ptr == nullptr){
            gde_interface->print_warning("Bad base pubkey", "_update_pointer", "pubkey.cpp", 24, false);
            return;
        }

        data_pointer = create_pubkey_with_seed(base_ptr, (const char*) seed.to_utf8_buffer().ptr(), seed.length(), owner_ptr);
        if(data_pointer == nullptr){
            gde_interface->print_warning("Creating pubkey with seed failed", "_update_pointer", "pubkey.cpp", 30, false);
        }
    }
    else if (bytes.size() == PUBKEY_LENGTH){
        data_pointer = create_pubkey_from_array(bytes.ptr());
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
    ClassDB::bind_method(D_METHOD("get_seed"), &Pubkey::get_seed);
    ClassDB::bind_method(D_METHOD("set_seed", "p_value"), &Pubkey::set_seed);
    ClassDB::bind_method(D_METHOD("get_type"), &Pubkey::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "p_value"), &Pubkey::set_type);
    ClassDB::bind_method(D_METHOD("get_base"), &Pubkey::get_base);
    ClassDB::bind_method(D_METHOD("set_base", "p_value"), &Pubkey::set_base);
    ClassDB::bind_method(D_METHOD("get_owner"), &Pubkey::get_owner);
    ClassDB::bind_method(D_METHOD("set_owner", "p_value"), &Pubkey::set_owner);
}


bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "value") {
		set_value(p_value);
	}
    else if(name == "bytes"){
        set_bytes(p_value);
    }
    else if(name == "seed"){
        set_seed(p_value);
    }
    else if(name == "type"){
        set_type(p_value);
    }
    else if(name == "base"){
        set_base(p_value);
    }
    else if(name == "owner"){
        set_owner(p_value);
    }
    else{
	    return false;
    }
    return true;
}

bool Pubkey::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "value") {
		r_ret = value;
	}
    else if (name == "bytes") {
		r_ret = bytes;
	}
    else if (name == "seed"){
        r_ret = seed;
    }
    else if (name == "type"){
        r_ret = type;
    }
    else if (name == "base"){
        r_ret = base;
    }
    else if (name == "owner"){
        r_ret = owner;
    }
    else{
	    return false;
    }
    return true;
}

void Pubkey::set_value(const String& p_value){
    value = p_value;
    type = "CUSTOM";

    // Update bytes accordingly.
    PackedByteArray decoded_value = SolanaSDK::bs58_decode(value);
    bytes = decoded_value;

    // Print warnings if key length is bad.
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

void Pubkey::set_seed(const String& p_value){
    seed = p_value;
}

String Pubkey::get_seed(){
    return seed;
}

void Pubkey::set_bytes(const PackedByteArray& p_value){
    bytes = p_value;
    type = "CUSTOM";

    // Do not feed zero bytes into encode function.
    if (bytes.size() == 0){
        value = "";
    }
    else{
        String encoded_value = SolanaSDK::bs58_encode(bytes);
        value = encoded_value;
    }

    // Print warnings if byte length is bad.
    if (bytes.size() != 32){
        internal::gde_interface->print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 14, false);
    }
}
PackedByteArray Pubkey::get_bytes(){
    return bytes;
}

void Pubkey::set_type(const String p_value){
    type = p_value;
    notify_property_list_changed();
}

String Pubkey::get_type(){
    return type;
}

void Pubkey::set_base(const Variant p_value){
    base = p_value;
}
Variant Pubkey::get_base(){
    return base;
}

void Pubkey::set_owner(const Variant p_value){
    owner = p_value;
}
Variant Pubkey::get_owner(){
    return owner;
}

void Pubkey::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags seed_visibility = PROPERTY_USAGE_DEFAULT;
    PropertyUsageFlags custom_visibility = PROPERTY_USAGE_DEFAULT;

    p_list->push_back(PropertyInfo(Variant::STRING, "type", PROPERTY_HINT_ENUM, "UNIQUE,SEED,CUSTOM"));

    if(type == "UNIQUE"){
        seed_visibility = PROPERTY_USAGE_NO_EDITOR;
        custom_visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    else if(type == "SEED"){
        custom_visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    else if(type == "CUSTOM"){
        seed_visibility = PROPERTY_USAGE_NO_EDITOR;
    }

    p_list->push_back(PropertyInfo(Variant::STRING, "seed", PROPERTY_HINT_NONE, "", seed_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "base", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "owner", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", custom_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", custom_visibility, ""));
}

Pubkey::Pubkey() {
}

Pubkey::~Pubkey() {
    _free_pointer_if_not_null();
}

}