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

bool AccountMeta::_set(const StringName &p_name, const Variant &p_value){
    String name = p_name;
    if (name == "is_signer") {
		set_is_signer(p_value);
	}
    else if (name == "writeable") {
		set_writeable(p_value);
	}
    else if (name == "key"){
        set_pubkey(p_value);
    }
    else{
	    return false;
    }
    return true;
}
bool AccountMeta::_get(const StringName &p_name, Variant &r_ret) const{
    String name = p_name;
	if (name == "is_signer") {
		r_ret = is_signer;
	}
    else if (name == "writeable") {
		r_ret = writeable;
	}
    else if (name == "key"){
        r_ret = key;
    }
    else{
	    return false;
    }
    return true;
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

AccountMeta::AccountMeta() : Resource(){
}

AccountMeta::AccountMeta(const Variant& pid, bool signer, bool writeable){
    const Pubkey *temp = memnew(Pubkey(pid));
    this->key = temp;
    this->is_signer = signer;
    this->writeable = writeable;
}

AccountMeta::AccountMeta(const Variant& other){
    if(other.has_method("get_pubkey")){
        const AccountMeta* meta_ptr = Object::cast_to<AccountMeta>(other);
        this->key = meta_ptr->get_pubkey();
        this->is_signer = meta_ptr->get_is_signer();
        this->writeable = meta_ptr->get_writeable();
    }
    else{
        internal::gdextension_interface_print_warning("Assigning AccountMeta with an unexpected object.", "assignment constructor", __FILE__, __LINE__, false);
    }
}

void AccountMeta::create_new(const Variant& account_key, bool is_signer, bool writeable){
    Object *account_key_cast = account_key;
    Pubkey *account_key_ptr = Object::cast_to<Pubkey>(account_key_cast);
}

bool AccountMeta::operator==(const AccountMeta& other) const{
    Pubkey *this_key = Object::cast_to<Pubkey>(key);
    Pubkey *other_key = Object::cast_to<Pubkey>(other.key);

    return this_key->get_bytes() == other_key->get_bytes();
}

AccountMeta::~AccountMeta(){
    key.clear();
}
}