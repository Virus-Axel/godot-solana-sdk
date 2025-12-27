#include "account.hpp"

#include <cstdint>

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"

#include "pubkey.hpp"

namespace godot {

void Account::_bind_methods() {
	ClassDB::add_signal("Account", MethodInfo("synced_to_chain"));

	ClassDB::bind_method(D_METHOD("synchronize"), &Account::synchronize);
	ClassDB::bind_method(D_METHOD("get_min_fetch_interval"), &Account::get_min_fetch_interval);
	ClassDB::bind_method(D_METHOD("set_min_fetch_interval", "value"), &Account::set_min_fetch_interval);
	ClassDB::bind_method(D_METHOD("get_max_fetch_interval"), &Account::get_max_fetch_interval);
	ClassDB::bind_method(D_METHOD("set_max_fetch_interval", "value"), &Account::set_max_fetch_interval);
	ClassDB::bind_method(D_METHOD("get_key"), &Account::get_key);
	ClassDB::bind_method(D_METHOD("set_key", "value"), &Account::set_key);
	ClassDB::bind_method(D_METHOD("get_data"), &Account::get_data);
	ClassDB::bind_method(D_METHOD("set_data", "value"), &Account::set_data);
	ClassDB::bind_method(D_METHOD("get_executable"), &Account::get_executable);
	ClassDB::bind_method(D_METHOD("set_executable", "value"), &Account::set_executable);
	ClassDB::bind_method(D_METHOD("get_lamports"), &Account::get_lamports);
	ClassDB::bind_method(D_METHOD("set_lamports", "value"), &Account::set_lamports);
	ClassDB::bind_method(D_METHOD("get_sync_with_chain"), &Account::get_sync_with_chain);
	ClassDB::bind_method(D_METHOD("set_sync_with_chain", "value"), &Account::set_sync_with_chain);

	ClassDB::add_property("Account", PropertyInfo(Variant::FLOAT, "min_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_min_fetch_interval", "get_min_fetch_interval");
	ClassDB::add_property("Account", PropertyInfo(Variant::FLOAT, "max_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_max_fetch_interval", "get_max_fetch_interval");
	ClassDB::add_property("Account", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_key", "get_key");
	ClassDB::add_property("Account", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_data", "get_data");
	ClassDB::add_property("Account", PropertyInfo(Variant::INT, "lamports", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_lamports", "get_lamports");
	ClassDB::add_property("Account", PropertyInfo(Variant::BOOL, "executable", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_executable", "get_executable");
	ClassDB::add_property("Account", PropertyInfo(Variant::BOOL, "sync_with_chain", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_sync_with_chain", "get_sync_with_chain");
}

void Account::_process(double delta) {
	seconds_since_fetch += delta;
	if (seconds_since_fetch < min_fetch_interval) {
		return;
	}
	if (!is_valid()) {
		return;
	}

	Object *ptr = Engine::get_singleton()->get_singleton("account_fetcher");
	auto *account_fetcher = Object::cast_to<AccountFetcher>(ptr);
	if (seconds_since_fetch > max_fetch_interval) {
		// Check for duplicates.
		if (!account_fetcher->has_account(key)) {
			account_fetcher->add_account_to_fetch(this);
		}

		account_fetcher->fetch_all();
		account_fetcher->process(delta);
	} else {
		// Check for duplicates.
		if (!account_fetcher->has_account(key)) {
			account_fetcher->add_account_to_fetch(this);
		}
	}
}

bool Account::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;
	if (name == "data") {
		set_data(p_value);
		return true;
	}
	if (name == "key") {
		set_key(p_value);
		return true;
	}
	if (name == "min_fetch_interval") {
		set_min_fetch_interval(p_value);
		return true;
	}
	if (name == "max_fetch_interval") {
		set_max_fetch_interval(p_value);
		return true;
	}
	if (name == "lamports") {
		set_lamports(p_value);
		return true;
	}
	if (name == "executable") {
		set_executable(p_value);
		return true;
	}
	if (name == "sync_with_chain") {
		set_sync_with_chain(p_value);
		return true;
	}
	return false;
}

bool Account::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;
	if (name == "data") {
		r_ret = get_data();
		return true;
	}
	if (name == "key") {
		r_ret = get_key();
		return true;
	}
	if (name == "min_fetch_interval") {
		r_ret = get_min_fetch_interval();
		return true;
	}
	if (name == "max_fetch_interval") {
		r_ret = get_max_fetch_interval();
		return true;
	}
	if (name == "lamports") {
		r_ret = get_lamports();
		return true;
	}
	if (name == "executable") {
		r_ret = get_executable();
		return true;
	}
	if (name == "sync_with_chain") {
		r_ret = get_sync_with_chain();
		return true;
	}
	return false;
}

void Account::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_DEFAULT));
	p_list->push_back(PropertyInfo(Variant::BOOL, "sync_with_chain"));

	if (sync_with_chain) {
		p_list->push_back(PropertyInfo(Variant::FLOAT, "min_fetch_interval"));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "max_fetch_interval"));
		p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY | PROPERTY_USAGE_EDITOR));
		p_list->push_back(PropertyInfo(Variant::INT, "lamports", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY | PROPERTY_USAGE_EDITOR));
		p_list->push_back(PropertyInfo(Variant::BOOL, "executable", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_READ_ONLY | PROPERTY_USAGE_EDITOR));
	} else {
		p_list->push_back(PropertyInfo(Variant::FLOAT, "min_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		p_list->push_back(PropertyInfo(Variant::FLOAT, "max_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
		p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
		p_list->push_back(PropertyInfo(Variant::INT, "lamports", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
		p_list->push_back(PropertyInfo(Variant::BOOL, "executable", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT));
	}
}

bool Account::is_account(const Variant &other) {
	if (other.get_type() != Variant::OBJECT) {
		return false;
	}
	return (static_cast<Object *>(other))->is_class("Account");
}

bool Account::is_valid() const {
	return Pubkey::bytes_from_variant(key).size() == PUBKEY_LENGTH;
}

double Account::get_seconds_since_last_sync() const {
	return seconds_since_fetch;
}

void Account::synchronize() {
	seconds_since_fetch = 0.0;
}

void Account::set_sync_with_chain(const bool value) {
	sync_with_chain = value;
	notify_property_list_changed();
}

bool Account::get_sync_with_chain() const {
	return sync_with_chain;
}

void Account::set_data(const PackedByteArray &value) {
	data = value;
}

PackedByteArray Account::get_data() const {
	return data;
}

void Account::set_executable(bool value) {
	executable = value;
}

bool Account::get_executable() const {
	return executable;
}

void Account::set_lamports(uint64_t value) {
	lamports = value;
}

uint64_t Account::get_lamports() const {
	return lamports;
}

Variant Account::get_key() const {
	return key;
}

void Account::set_key(const Variant &value) {
	key = value;
}
void Account::set_min_fetch_interval(const double value) {
	min_fetch_interval = value;
}
double Account::get_min_fetch_interval() const {
	return min_fetch_interval;
}
void Account::set_max_fetch_interval(const double value) {
	max_fetch_interval = value;
}
double Account::get_max_fetch_interval() const {
	return max_fetch_interval;
}

void Account::init_with_onchain_data(const Dictionary &onchain_data) {
	if (!onchain_data.has_all(Array({ "data", "executable", "lamports", "owner" }))) {
		return;
	}

	if (onchain_data["data"].get_type() != Variant::ARRAY) {
		return;
	}

	const Array data_array = onchain_data["data"];
	data = SolanaUtils::bs64_decode(data_array[0]);
	owner = Pubkey::new_from_variant(onchain_data["owner"]);
	executable = onchain_data["executable"];
	lamports = onchain_data["lamports"];
	seconds_since_fetch = 0.0;
	emit_signal("synced_to_chain");
}

AccountFetcher::AccountFetcher() {
	set_callback(callable_mp(this, &AccountFetcher::fetch_callback));
	set_async_override(true);
}

void AccountFetcher::fetch_callback(const Dictionary &params) {
	const Variant accounts = SolanaUtils::get_nested_value(params, "result/value");
	if (accounts.get_type() == Variant::ARRAY) {
		const Array account_array = static_cast<Array>(accounts);
		const Array account_keys = fetch_accounts.keys();
		for (int64_t i = 0; i < account_array.size(); i++) {
			const Variant fetch_account = fetch_accounts[account_keys[i]];
			Object::cast_to<Account>(fetch_account)->init_with_onchain_data(account_array[i]);
		}
	}
	fetch_accounts.clear();
	pending_fetch = false;
	fetch_accounts = fetch_accounts_buffer;
	fetch_accounts_buffer.clear();
}

void AccountFetcher::_bind_methods() {
}

void AccountFetcher::_ready() {
}

void AccountFetcher::process(double delta) {
	const unsigned int current_frame = Engine::get_singleton()->get_process_frames();
	if (current_frame == last_processed_frame) {
		return;
	}
	last_processed_frame = current_frame;

	_process(delta);

	time_since_last_fetch += delta;
}

void AccountFetcher::fetch_all() {
	if (pending_fetch) {
		return;
	}
	get_multiple_accounts(fetch_accounts.keys());
	pending_fetch = true;
}

bool AccountFetcher::has_account(const Variant &account) {
	if (account.get_type() != Variant::STRING) {
		return fetch_accounts.keys().has(static_cast<String>(account));
	}

	if (account.get_type() != Variant::OBJECT) {
		return fetch_accounts.keys().has(Pubkey::string_from_variant(account));
	}

	ERR_FAIL_V_EDMSG_CUSTOM(false, "Account type is not an object");
}

void AccountFetcher::add_account_to_fetch(const Variant &account) {
	ERR_FAIL_COND_EDMSG_CUSTOM(account.get_type() != Variant::OBJECT, "Account is not an object.");
	const auto *account_object = static_cast<Object *>(account);
	ERR_FAIL_COND_EDMSG_CUSTOM(!account_object->is_class("Account"), "Object is not an account.");

	const String key = Pubkey::string_from_variant(account);
	ERR_FAIL_COND_EDMSG_CUSTOM(key.is_empty(), "Account key is Empty");

	if (pending_fetch) {
		fetch_accounts_buffer[key] = account;
	} else {
		fetch_accounts[key] = account;
	}
}

void AccountFetcher::remove_account_to_fetch(const Variant &account) {
	fetch_accounts.erase(account);
	fetch_accounts_buffer.erase(account);
}

} //namespace godot
