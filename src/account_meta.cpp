#include "account_meta.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <keypair.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

void AccountMeta::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_is_signer"), &AccountMeta::get_is_signer);
    ClassDB::bind_method(D_METHOD("set_is_signer", "p_value"), &AccountMeta::set_is_signer);
    ClassDB::bind_method(D_METHOD("get_writeable"), &AccountMeta::get_writeable);
    ClassDB::bind_method(D_METHOD("set_writeable", "p_value"), &AccountMeta::set_writeable);
    ClassDB::bind_method(D_METHOD("get_pubkey"), &AccountMeta::get_pubkey);
    ClassDB::bind_method(D_METHOD("set_pubkey", "p_value"), &AccountMeta::set_pubkey);
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::BOOL, "is_signer"), "set_is_signer", "get_is_signer");
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::BOOL, "writeable"), "set_writeable", "get_writeable");
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair", PROPERTY_USAGE_DEFAULT), "set_pubkey", "get_pubkey");
    ClassDB::bind_method(D_METHOD("create_new", "account_key", "is_signer", "writeable"), &AccountMeta::create_new);
}

void AccountMeta::_update_pointer(){
    if (key.get_type() != Variant::OBJECT){
        return;
    }

    void *key_ref = nullptr;
    Object *object_cast = key;
    Pubkey temp_key;
    if(object_cast->is_class("Pubkey")){
        key_ref = variant_to_type<Pubkey>(key);
    }
    else if(object_cast->is_class("Keypair")){
        // Make sure we have a pubkey in the end.
        Keypair *keypair = Object::cast_to<Keypair>(key);
        temp_key.set_type("CUSTOM");
        temp_key.set_value(keypair->get_public_value());
        key_ref = temp_key.to_ptr();
        std::cout << "alalalala" << std::endl;
        //std::cout << ((Pubkey*)key_ref)->get_value().to_utf8_buffer().ptr() << std::endl;
    }
    else{
        gdextension_interface_print_warning("Account Meta does not have a valid key", "_update_pointer", "account_meta.cpp", 44, false);
        return;
    }

    if (key_ref == nullptr){
        gdextension_interface_print_warning("Account Meta does not have a valid key", "_update_pointer", "account_meta.cpp", 50, false);
        return;
    }

    data_pointer = create_account_meta(key_ref, is_signer, writeable);
}

void AccountMeta::_free_pointer(){
    free_account_meta(data_pointer);
}

void AccountMeta::set_pubkey(const Variant &p_value) {
    key = p_value;
}

Variant AccountMeta::get_pubkey() const {
	return key;
}

void AccountMeta::set_is_signer(const bool p_value) {
    is_signer = p_value;
}

bool AccountMeta::get_is_signer() const {
	return is_signer;
}

void AccountMeta::set_writeable(const bool p_value) {
    writeable = p_value;
}

bool AccountMeta::get_writeable() const {
	return writeable;
}

AccountMeta::AccountMeta() {
}

void AccountMeta::create_new(const Variant& account_key, bool is_signer, bool writeable){
    Object *account_key_cast = account_key;
    Pubkey *account_key_ptr = Object::cast_to<Pubkey>(account_key_cast);
    data_pointer = create_account_meta(account_key_ptr->to_ptr(), is_signer, writeable);
}

AccountMeta::~AccountMeta(){
    _free_pointer_if_not_null();
}
}