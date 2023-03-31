#include "pubkey.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Pubkey::_bind_methods() {
}

bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "value") {
		value = p_value;
        PackedByteArray decoded_value = bs58_decode(value);
        bytes = decoded_value;
        if (decoded_value.size() != 32){
            internal::gde_interface->print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 14, false);
        }
		return true;
	}
    else if(name == "bytes"){
        bytes = p_value;
        if (bytes.size() == 0){
            value = "";
            return true;
        }
        String encoded_value = bs58_encode(bytes);
        value = encoded_value;
        if (bytes.size() != 32){
            internal::gde_interface->print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 14, false);
        }
		return true;
    }
    else if(name == "unique"){
        unique = p_value;
        notify_property_list_changed();
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
    data_pointer = create_pubkey();
}

const void* Pubkey::to_ptr() const{
    return data_pointer;
}

Pubkey::~Pubkey() {
    free_pubkey(data_pointer);
}

}