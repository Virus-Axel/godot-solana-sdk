#include "account_meta.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void AccountMeta::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_is_signer"), &AccountMeta::get_is_signer);
    ClassDB::bind_method(D_METHOD("set_is_signer", "p_value"), &AccountMeta::set_is_signer);
    ClassDB::bind_method(D_METHOD("get_writeable"), &AccountMeta::get_writeable);
    ClassDB::bind_method(D_METHOD("set_writeable", "p_value"), &AccountMeta::set_writeable);
    ClassDB::bind_method(D_METHOD("get_pubkey"), &AccountMeta::get_pubkey);
    ClassDB::bind_method(D_METHOD("set_pubkey", "p_value"), &AccountMeta::set_pubkey);
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::BOOL, "is_signer"), "set_is_signer", "get_is_signer");
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::BOOL, "writeable"), "set_writeable", "get_writeable");
    ClassDB::add_property("AccountMeta", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_NODE_PATH_TO_EDITED_NODE, "Pubkey", PROPERTY_USAGE_DEFAULT), "set_pubkey", "get_pubkey");
    ClassDB::bind_method(D_METHOD("create_new", "account_key", "is_signer", "writeable"), &AccountMeta::create_new);
}

void AccountMeta::_update_pointer(){
    _free_pointer_if_not_null();
}

void AccountMeta::_free_pointer_if_not_null(){
    if(data_pointer != nullptr){
        free_account(data_pointer);
        data_pointer = nullptr;
    }
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
    data_pointer = nullptr;
}

void *AccountMeta::to_ptr(){
    return data_pointer;
}

void AccountMeta::create_new(const Variant& account_key, bool is_signer, bool writeable){
    Object *account_key_cast = account_key;
    Pubkey *account_key_ptr = Object::cast_to<Pubkey>(account_key_cast);
    data_pointer = create_account_meta(account_key_ptr->to_ptr(), is_signer, writeable);
}

AccountMeta::~AccountMeta() {
    _free_pointer_if_not_null();
}
}