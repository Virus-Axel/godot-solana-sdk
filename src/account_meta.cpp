#include "account_meta.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <keypair.hpp>

namespace godot {

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
	ClassDB::add_property("AccountMeta", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,PhantomController", PROPERTY_USAGE_DEFAULT), "set_pubkey", "get_pubkey");
	ClassDB::bind_method(D_METHOD("create_new", "account_key", "is_signer", "writeable"), &AccountMeta::create_new);

	ClassDB::bind_static_method("AccountMeta", D_METHOD("new_account_meta", "account_key", "is_signer", "writeable"), &AccountMeta::new_account_meta);
}

bool AccountMeta::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "is_signer") {
		set_is_signer(p_value);
	} else if (name == "writeable") {
		set_writeable(p_value);
	} else if (name == "key") {
		set_pubkey(p_value);
	} else {
		return false;
	}
	return true;
}
bool AccountMeta::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "is_signer") {
		r_ret = is_signer;
	} else if (name == "writeable") {
		r_ret = writeable;
	} else if (name == "key") {
		r_ret = key;
	} else {
		return false;
	}
	return true;
}

void AccountMeta::set_pubkey(const Variant &p_value) {
	key = p_value;
}

Variant AccountMeta::get_pubkey() const {
	//return key.duplicate(true);
	return Pubkey::new_from_variant(key);
}

Variant AccountMeta::get_signer() const {
	if (key.has_method("get_public_bytes")) {
		Keypair *res = memnew(Keypair());
		res->set_public_bytes(Object::cast_to<Keypair>(key)->get_public_bytes());
		res->set_private_bytes(Object::cast_to<Keypair>(key)->get_private_bytes());

		return res;
	} else {
		return key;
	}
}

void AccountMeta::set_key(const Variant &p_value) {
	key = p_value;
}

Variant AccountMeta::get_key() const {
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

AccountMeta::AccountMeta() :
		Resource() {
}

AccountMeta::AccountMeta(const Variant &pid, bool signer, bool writeable) {
	//const Pubkey *temp = memnew(Pubkey(pid));
	this->key = pid;
	this->is_signer = signer;
	this->writeable = writeable;
}

AccountMeta::AccountMeta(const Variant &other) {
	if (other.has_method("get_pubkey")) {
		const AccountMeta *meta_ptr = Object::cast_to<AccountMeta>(other);
		if (Object::cast_to<AccountMeta>(other)->get_is_signer()) {
			this->key = meta_ptr->get_signer();
		} else {
			this->key = meta_ptr->get_key();
		}
		this->is_signer = meta_ptr->get_is_signer();
		this->writeable = meta_ptr->get_writeable();
	} else {
		internal::gdextension_interface_print_warning("Assigning AccountMeta with an unexpected object.", "assignment constructor", __FILE__, __LINE__, false);
	}
}

void AccountMeta::create_new(const Variant &account_key, bool is_signer, bool writeable) {
	Object *account_key_cast = account_key;
	Pubkey *account_key_ptr = Object::cast_to<Pubkey>(account_key_cast);
}

bool AccountMeta::is_account_meta(const Variant &other) {
	return ((Object *)other)->is_class("AccountMeta");
}

Variant AccountMeta::new_account_meta(const Variant &account_key, bool is_signer, bool writeable) {
	AccountMeta *result = memnew(AccountMeta);

	result->set_is_signer(is_signer);
	result->set_writeable(writeable);
	result->set_pubkey(account_key);

	return result;
}

bool AccountMeta::operator==(const AccountMeta &other) const {
	return Pubkey::bytes_from_variant(key) == Pubkey::bytes_from_variant(other.key);
}

AccountMeta::~AccountMeta() {
	key.clear();
}
} //namespace godot