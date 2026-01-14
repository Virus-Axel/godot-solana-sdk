#include "account.hpp"

#include <cstdint>

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "pubkey.hpp"
#include "solana_utils.hpp"
#include "transaction/transaction.hpp"

namespace godot {

void Account::_bind_methods() {
    ClassDB::add_signal("Account", MethodInfo("synced_to_chain"));
    ClassDB::add_signal("Account", MethodInfo("sync_failed"));
	ClassDB::add_signal("Account", MethodInfo("data_changed"));

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
	ClassDB::bind_method(D_METHOD("get_pre_simulate_transaction"), &Account::get_pre_simulate_transaction);
	ClassDB::bind_method(D_METHOD("set_pre_simulate_transaction", "value"), &Account::set_pre_simulate_transaction);

	ClassDB::add_property("Account", PropertyInfo(Variant::FLOAT, "min_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_min_fetch_interval", "get_min_fetch_interval");
	ClassDB::add_property("Account", PropertyInfo(Variant::FLOAT, "max_fetch_interval", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_max_fetch_interval", "get_max_fetch_interval");
	ClassDB::add_property("Account", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_key", "get_key");
	ClassDB::add_property("Account", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_data", "get_data");
	ClassDB::add_property("Account", PropertyInfo(Variant::INT, "lamports", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_lamports", "get_lamports");
	ClassDB::add_property("Account", PropertyInfo(Variant::BOOL, "executable", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_executable", "get_executable");
	ClassDB::add_property("Account", PropertyInfo(Variant::BOOL, "sync_with_chain", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_sync_with_chain", "get_sync_with_chain");
	ClassDB::add_property("Account", PropertyInfo(Variant::OBJECT, "pre_simulate_transaction", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_pre_simulate_transaction", "get_pre_simulate_transaction");
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

	Object *account_simulator_ptr = Engine::get_singleton()->get_singleton("account_simulator");
	auto *account_simulator = Object::cast_to<AccountSimulator>(account_simulator_ptr);
	if (seconds_since_fetch > max_fetch_interval) {
		if (has_pre_simulate_transaction()) {
			account_simulator->add_account_to_fetch(this);
			account_simulator->simulate_all(get_pre_simulate_transaction());
			account_simulator->manual_process(delta);
		} else {
			// Check for duplicates.
			if (!account_fetcher->has_account(key)) {
				account_fetcher->add_account_to_fetch(this);
			}
			account_fetcher->fetch_all();
			account_fetcher->manual_process(delta);
		}

	} else {
		if (has_pre_simulate_transaction()) {
			account_simulator->add_account_to_fetch(this);
		} else {
			if (!account_fetcher->has_account(key)) {
				account_fetcher->add_account_to_fetch(this);
			}
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
	if (name == "pre_simulate_transaction") {
		set_pre_simulate_transaction(p_value);
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
	if (name == "pre_simulate_transaction") {
		r_ret = get_pre_simulate_transaction();
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

	p_list->push_back(PropertyInfo(Variant::OBJECT, "pre_simulate_transaction", PROPERTY_HINT_NODE_TYPE, "Transaction", PROPERTY_USAGE_DEFAULT));
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

void Account::set_sync_with_chain(const bool sync_with_chain) {
	this->sync_with_chain = sync_with_chain;
	notify_property_list_changed();
}

bool Account::get_sync_with_chain() const {
	return sync_with_chain;
}

void Account::set_data(const PackedByteArray &data) {
	this->data = data;
}

PackedByteArray Account::get_data() const {
	return data;
}

void Account::set_executable(bool executable) {
	this->executable = executable;
}

bool Account::get_executable() const {
	return executable;
}

void Account::set_lamports(uint64_t lamports) {
	this->lamports = lamports;
}

uint64_t Account::get_lamports() const {
	return lamports;
}

Variant Account::get_key() const {
	return key;
}

void Account::set_key(const Variant &key) {
	this->key = key;
}
void Account::set_min_fetch_interval(const double min_fetch_interval) {
	this->min_fetch_interval = min_fetch_interval;
}
double Account::get_min_fetch_interval() const {
	return min_fetch_interval;
}
void Account::set_max_fetch_interval(const double max_fetch_interval) {
	this->max_fetch_interval = max_fetch_interval;
}
double Account::get_max_fetch_interval() const {
	return max_fetch_interval;
}
void Account::set_pre_simulate_transaction(const Variant &pre_simulate_transaction) {
	this->pre_simulate_transaction = pre_simulate_transaction;
}
Variant Account::get_pre_simulate_transaction() const {
	return pre_simulate_transaction;
}

bool Account::has_pre_simulate_transaction() const {
	if (pre_simulate_transaction.get_type() == Variant::OBJECT) {
		return static_cast<Object *>(pre_simulate_transaction)->is_class("Transaction");
	}

	return false;
}

void Account::init_with_onchain_data(const Dictionary &onchain_data) {
    seconds_since_fetch = 0.0;
	if (!onchain_data.has_all(Array({ "data", "executable", "lamports", "owner" }))) {
	    emit_signal("sync_failed");
		return;
	}

	if (onchain_data["data"].get_type() != Variant::ARRAY) {
	    emit_signal("sync_failed");
		return;
	}

	const Array data_array = onchain_data["data"];
	data = SolanaUtils::bs64_decode(data_array[0]);
	owner = Pubkey::new_from_variant(onchain_data["owner"]);
	executable = onchain_data["executable"];
	lamports = onchain_data["lamports"];
	emit_signal("synced_to_chain");
	if (encoded_data != String(data_array[0])) {
		emit_signal("data_changed");
	}
	encoded_data = data_array[0];
}

AccountFetcher::AccountFetcher() {
	set_callback(callable_mp(this, &AccountFetcher::fetch_callback));
	set_async_override(true);
}

void AccountFetcher::update_accounts(const Dictionary &account_dict, const Array &accounts_data) {
	const Array account_keys = account_dict.keys();
	for (int64_t i = 0; i < accounts_data.size(); i++) {
		const Variant &fetch_account = account_dict[account_keys[i]];
		Object::cast_to<Account>(fetch_account)->init_with_onchain_data(accounts_data[i]);
	}
}

void AccountFetcher::fetch_callback(const Dictionary &params) {
	const Variant rpc_value = SolanaUtils::get_nested_value(params, "result/value");
	if (rpc_value.get_type() == Variant::ARRAY) {
		update_accounts(fetch_accounts, rpc_value);
	} else {
		WARN_PRINT_ONCE_ED_CUSTOM(vformat("Unknwn response: ", params));
	}
	pending_fetch = false;
	fetch_accounts.clear();
	fetch_accounts = fetch_accounts_buffer;
	fetch_accounts_buffer.clear();
}

void AccountFetcher::_bind_methods() {
}

void AccountFetcher::_ready() {
}

void AccountFetcher::manual_process(double delta) {
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
	UtilityFunctions::print(fetch_accounts.keys());
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

AccountSimulator::AccountSimulator() {
	set_callback(callable_mp(this, &AccountSimulator::fetch_callback));
	set_async_override(true);
}

void AccountSimulator::update_accounts(const Dictionary &account_dict, const Array &accounts_data) {
	const Array account_keys = account_dict.keys();
	for (int64_t i = 0; i < accounts_data.size(); i++) {
		const Variant &fetch_account = account_dict[account_keys[i]];
		Object::cast_to<Account>(fetch_account)->init_with_onchain_data(accounts_data[i]);
	}
}

void AccountSimulator::add_sim_account(const String &hash, const Variant &account) {
	const String account_key = Pubkey::string_from_variant(account);
	if (sim_accounts.has(hash)) {
		static_cast<Dictionary>(sim_accounts[hash])[account_key] = account;
	} else {
		Dictionary account_dict;
		account_dict[account_key] = account;
		sim_accounts[hash] = account_dict;
	}
}

void AccountSimulator::fetch_callback(const Dictionary &params) {
	const Variant rpc_value = SolanaUtils::get_nested_value(params, "result/value");
	if (rpc_value.get_type() == Variant::DICTIONARY) {
		const Dictionary value_dict = rpc_value;
		if (value_dict.has("accounts")) {
			update_accounts(sim_accounts[pending_sim_key], value_dict["accounts"]);
		} else {
			WARN_PRINT_ONCE_ED_CUSTOM("RPC response did not contain any acconts.");
		}
	} else {
		WARN_PRINT_ONCE_ED_CUSTOM(vformat("Unknwn response: ", params));
	}

	pending_simulation = false;
	sim_accounts.erase(pending_sim_key);
}

void AccountSimulator::_bind_methods() {
}

void AccountSimulator::_ready() {
}

void AccountSimulator::manual_process(double delta) {
	const unsigned int current_frame = Engine::get_singleton()->get_process_frames();
	if (current_frame == last_processed_frame) {
		return;
	}
	last_processed_frame = current_frame;

	_process(delta);

	time_since_last_fetch += delta;
}

void AccountSimulator::simulate_all(const Variant &transaction) {
	if (pending_simulation) {
		return;
	}

	auto *transaction_ptr = Object::cast_to<Transaction>(static_cast<Object *>(transaction));
	const PackedByteArray serialized_message = transaction_ptr->serialize();
	const String encoded_transaction = SolanaUtils::bs64_encode(serialized_message);
	pending_sim_key = transaction_ptr->get_message_hash();
	const Dictionary accounts_dict = sim_accounts[pending_sim_key];

	simulate_transaction(encoded_transaction, false, true, accounts_dict.keys());
	pending_simulation = true;
}

void AccountSimulator::add_account_to_fetch(const Variant &account) {
	ERR_FAIL_COND_EDMSG_CUSTOM(account.get_type() != Variant::OBJECT, "Account is not an object.");
	const auto *account_object = static_cast<Object *>(account);
	ERR_FAIL_COND_EDMSG_CUSTOM(!account_object->is_class("Account"), "Object is not an account.");

	const String key = Pubkey::string_from_variant(account);
	ERR_FAIL_COND_EDMSG_CUSTOM(key.is_empty(), "Account key is Empty");

	const auto *account_ptr = Object::cast_to<Account>(account_object);
	Object *transaction_object = static_cast<Object *>(account_ptr->get_pre_simulate_transaction());
	const String transaction_hash = Object::cast_to<Transaction>(transaction_object)->get_message_hash();
	add_sim_account(transaction_hash, account);
}

} //namespace godot
