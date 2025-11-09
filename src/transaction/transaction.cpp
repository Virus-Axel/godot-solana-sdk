#include "transaction.hpp"

#include <cstdint>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "account_meta.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"
#include "solana_client.hpp"
#include <instructions/compute_budget.hpp>
#include <merged_account_metas.hpp>
#include <message.hpp>
#include <solana_utils.hpp>
#include <wallet_adapter.hpp>

namespace godot {

void Transaction::_bind_methods() {
	ClassDB::add_signal("Transaction", MethodInfo("processed"));
	ClassDB::add_signal("Transaction", MethodInfo("confirmed"));
	ClassDB::add_signal("Transaction", MethodInfo("finalized"));

	ClassDB::add_signal("Transaction", MethodInfo("fully_signed"));
	ClassDB::add_signal("Transaction", MethodInfo("send_ready"));
	ClassDB::add_signal("Transaction", MethodInfo("signing_failed", PropertyInfo(Variant::INT, "signer_index")));
	ClassDB::add_signal("Transaction", MethodInfo("transaction_response_received", PropertyInfo(Variant::DICTIONARY, "result")));
	ClassDB::add_signal("Transaction", MethodInfo("blockhash_updated", PropertyInfo(Variant::DICTIONARY, "result")));
	ClassDB::add_signal("Transaction", MethodInfo("blockhash_update_failed", PropertyInfo(Variant::DICTIONARY, "result")));
	ClassDB::add_signal("Transaction", MethodInfo("signer_state_changed"));
	ClassDB::add_signal("Transaction", MethodInfo("transaction_simulation_failed", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::DICTIONARY, "data")));

	ClassDB::bind_static_method("Transaction", D_METHOD("new_from_bytes", "bytes"), &Transaction::new_from_bytes);

	ClassDB::bind_method(D_METHOD("get_instructions"), &Transaction::get_instructions);
	ClassDB::bind_method(D_METHOD("set_instructions", "p_value"), &Transaction::set_instructions);
	ClassDB::bind_method(D_METHOD("get_payer"), &Transaction::get_payer);
	ClassDB::bind_method(D_METHOD("set_payer", "p_value"), &Transaction::set_payer);
	ClassDB::bind_method(D_METHOD("get_signers"), &Transaction::get_signers);
	ClassDB::bind_method(D_METHOD("set_signers", "p_value"), &Transaction::set_signers);
	ClassDB::bind_method(D_METHOD("get_unit_limit"), &Transaction::get_unit_limit);
	ClassDB::bind_method(D_METHOD("set_unit_limit", "value"), &Transaction::set_unit_limit);
	ClassDB::bind_method(D_METHOD("get_unit_price"), &Transaction::get_unit_price);
	ClassDB::bind_method(D_METHOD("set_unit_price", "value"), &Transaction::set_unit_price);

	ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);

	ClassDB::bind_method(D_METHOD("add_instruction", "instruction"), &Transaction::add_instruction);
	ClassDB::bind_method(D_METHOD("update_latest_blockhash", "custom_hash"), &Transaction::update_latest_blockhash, DEFVAL(""));

	ClassDB::bind_method(D_METHOD("sign"), &Transaction::sign);
	ClassDB::bind_method(D_METHOD("sign_and_send"), &Transaction::sign_and_send);
	ClassDB::bind_method(D_METHOD("create_message"), &Transaction::create_message);
	ClassDB::bind_method(D_METHOD("send"), &Transaction::send);
	ClassDB::bind_method(D_METHOD("partially_sign", "latest_blockhash"), &Transaction::partially_sign);

	BIND_CONSTANT(DEFAULT_UNIT_LIMIT);
	BIND_CONSTANT(DEFAULT_UNIT_PRICE);
	BIND_CONSTANT(SIGNATURE_LENGTH);
}

void Transaction::_signer_signed(const PackedByteArray &signature, const int32_t index) {
	auto *controller = Object::cast_to<WalletAdapter>(signers[index]);
	controller->disconnect("signing_failed", callable_mp(this, &Transaction::_signer_failed));

	if (controller->did_transaction_change()) {
		const PackedByteArray new_bytes = WalletAdapter::get_modified_transaction();
		const Array temp_signers = signers;
		from_bytes(new_bytes);
		signers = temp_signers;
		UtilityFunctions::print("Browser wallet altered transaction.");
	}

	signatures[index] = signature;
	ready_signature_amount++;

	emit_signal("signer_state_changed");
	check_fully_signed();
}

void Transaction::_signer_failed(const int32_t index) {
	ERR_FAIL_COND_EDMSG_CUSTOM(index >= signers.size(), "Signer index out of bounds.");
	ERR_FAIL_COND_EDMSG_CUSTOM(!WalletAdapter::is_wallet_adapter(signers[index]), "Signer is not a WalletAdapter.");

	auto *controller = Object::cast_to<WalletAdapter>(signers[index]);
	controller->disconnect("message_signed", callable_mp(this, &Transaction::_signer_signed));

	emit_signal("signing_failed", index);
}

bool Transaction::is_phantom_payer() const {
	return payer.has_method("get_connected_key");
}

bool Transaction::has_valid_payer() const {
	// TODO(Virax): Check for signing class as well.
	return (payer.get_type() == Variant::OBJECT);
}

void Transaction::create_message() {
	if (instructions.is_empty() || (payer.get_type() == Variant::NIL)) {
		signatures.clear();
		return;
	}
	for (unsigned int i = 0; i < instructions.size(); i++) {
		if (instructions[i].get_type() != Variant::OBJECT) {
			signatures.clear();
			return;
		}
		if (!Object::cast_to<Instruction>(instructions[i])->is_serializable()) {
			signatures.clear();
			return;
		}
	}

	Array instruction_list = instructions.duplicate();

	MergedAccountMetas merged_metas;
	if (append_budget_instructions) {
		instruction_list.insert(0, ComputeBudget::set_compute_unit_limit(unit_limit));
		instruction_list.insert(1, ComputeBudget::set_compute_unit_price(unit_price));
	}
	if (has_valid_payer()) {
		const Variant payer_meta = AccountMeta::new_account_meta(payer, true, true);
		merged_metas.add(payer_meta);
	}

	merged_metas.from_instructions(instruction_list);
	message.create(merged_metas, payer);
	message.compile_instructions(instruction_list);

	message.set_latest_blockhash(latest_blockhash_string);
	message.set_address_lookup_tables(address_lookup_tables);

	const int amount_of_signers = message.get_amount_signers();

	if (signers.size() == amount_of_signers) {
		return;
	}

	ready_signature_amount = 0;
	signatures.resize(amount_of_signers);
	for (unsigned int i = 0; i < signatures.size(); i++) {
		PackedByteArray temp;
		temp.resize(SIGNATURE_LENGTH);
		signatures[i] = temp;
	}
}

void Transaction::check_fully_signed() {
	if (ready_signature_amount == signatures.size()) {
		emit_signal("fully_signed");
	}
}

void Transaction::reset_state() {
	ready_signature_amount = 0;
	signatures.clear();
	signers.clear();
	result_signature = "";
}

void Transaction::sign_at_index(const uint32_t index) {
	if (signers[index].get_type() != Variant::Type::OBJECT) {
		Array params;
		params.append(Variant::get_type_name(signers[index].get_type()));
		WARN_PRINT_ONCE_ED(String("Signer is not an object. It is a {0}").format(params));
	} else if (Keypair::is_keypair(signers[index])) {
		auto *signer_keypair = Object::cast_to<Keypair>(signers[index]);

		const PackedByteArray signature = signer_keypair->sign_message(serialize_message());
		signatures[index] = signature;
		ready_signature_amount++;
		emit_signal("signer_state_changed");
		check_fully_signed();
	} else if (Keypair::is_compatible_type(signers[index])) {
		auto signer_keypair = Keypair::new_from_variant(signers[index]);
		ERR_FAIL_COND_EDMSG_CUSTOM(signer_keypair.get_type() != Variant::OBJECT, "Signer is not a Keypair or compatible type.");

		const PackedByteArray signature = Object::cast_to<Keypair>(signer_keypair)->sign_message(serialize_message());
		signatures[index] = signature;
		ready_signature_amount++;
		emit_signal("signer_state_changed");
		check_fully_signed();
	} else if (signers[index].has_method("sign_message")) {
		auto *controller = Object::cast_to<WalletAdapter>(signers[index]);

		controller->connect("message_signed", callable_mp(this, &Transaction::_signer_signed).bindv(Array::make(index)), CONNECT_ONE_SHOT);
		controller->connect("signing_failed", callable_mp(this, &Transaction::_signer_failed).bindv(Array::make(index)), CONNECT_ONE_SHOT);
		controller->sign_message(serialize(), index);
	} else {
		ERR_PRINT_ONCE_ED("Unknown signer type.");
	}
}

void Transaction::copy_connection_state() {
	processed_connections = get_signal_connection_list("processed").size();
	confirmed_connections = get_signal_connection_list("confirmed").size();
	finalized_connections = get_signal_connection_list("finalized").size();
}

void Transaction::subscribe_to_signature(ConfirmationLevel confirmation_level) {
	switch (confirmation_level) {
		case CONFIRMED:
			signature_subscribe(result_signature, callable_mp(this, &Transaction::_emit_confirmed_callback), "confirmed");
			break;

		case PROCESSED:
			signature_subscribe(result_signature, callable_mp(this, &Transaction::_emit_processed_callback), "processed");
			break;

		case FINALIZED:
			signature_subscribe(result_signature, callable_mp(this, &Transaction::_emit_finalized_callback), "finalized");
			break;

		default:
			ERR_FAIL_EDMSG("Internal Error. Unknown confirmation level.");
			break;
	}

	active_subscriptions++;
}

void Transaction::subscribe_to_signature() {
	if (processed_connections != 0U) {
		subscribe_to_signature(PROCESSED);
	}
	if (confirmed_connections != 0U) {
		subscribe_to_signature(CONFIRMED);
	}
	if (finalized_connections != 0U) {
		subscribe_to_signature(FINALIZED);
	}
}

void Transaction::_emit_processed_callback(const Dictionary &params) {
	(void)params; // Unused.
	active_subscriptions--;
	emit_signal("processed");
}

void Transaction::_emit_confirmed_callback(const Dictionary &params) {
	(void)params; // Unused.
	active_subscriptions--;
	emit_signal("confirmed");
}

void Transaction::_emit_finalized_callback(const Dictionary &params) {
	(void)params; // Unused.
	active_subscriptions--;
	emit_signal("finalized");
}

void Transaction::check_transaction_simulation(const Dictionary &params) {
	const String simulation_failed_indicator = "Transaction simulation failed:";
	if (!params.has("error")) {
		return;
	}
	const Dictionary error = params["error"];
	if (!error.has("message")) {
		return;
	}
	const String message = error["message"];
	if (!message.begins_with(simulation_failed_indicator)) {
		return;
	}

	emit_signal("transaction_simulation_failed", message.replace(simulation_failed_indicator, "").lstrip(" "), error["data"]);
}

bool Transaction::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;
	if (name == "instructions") {
		set_instructions(p_value);
		return true;
	}
	if (name == "payer") {
		set_payer(p_value);
		return true;
	}
	if (name == "signers") {
		set_signers(p_value);
		return true;
	}
	if (name == "external_payer") {
		set_external_payer(p_value);
		return true;
	}
	if (name == "unit_limit") {
		unit_limit = p_value;
		return true;
	}
	if (name == "unit_price") {
		unit_price = p_value;
		return true;
	}
	if (name == "skip_preflight") {
		skip_preflight = p_value;
		return true;
	}
	if (name == "skip_preflight") {
		skip_preflight = p_value;
		return true;
	}
	if (name == "append_budget_instructions") {
		append_budget_instructions = p_value;
		return true;
	}
	return false;
}

bool Transaction::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;
	if (name == "instructions") {
		r_ret = instructions;
		return true;
	}
	if (name == "payer") {
		r_ret = payer;
		return true;
	}
	if (name == "signers") {
		r_ret = signers;
		return true;
	}
	if (name == "external_payer") {
		r_ret = external_payer;
		return true;
	}
	if (name == "unit_limit") {
		r_ret = unit_limit;
		return true;
	}
	if (name == "unit_price") {
		r_ret = unit_price;
		return true;
	}
	if (name == "skip_preflight") {
		r_ret = skip_preflight;
		return true;
	}
	if (name == "skip_preflight") {
		r_ret = skip_preflight;
		return true;
	}
	if (name == "append_budget_instructions") {
		r_ret = append_budget_instructions;
		return true;
	}
	return false;
}

bool Transaction::are_all_bytes_zeroes(const PackedByteArray &bytes) {
	for (const unsigned char byte : bytes) {
		if (byte != 0) {
			return false;
		}
	}
	return true;
}

void Transaction::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, "skip_preflight"));
	if (append_budget_instructions) {
		p_list->push_back(PropertyInfo(Variant::INT, "unit_limit"));
		p_list->push_back(PropertyInfo(Variant::INT, "unit_price"));
	}
	p_list->push_back(PropertyInfo(Variant::BOOL, "external_payer", PROPERTY_HINT_NONE, "false"));
	if (!external_payer) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,JSON"));
	} else {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_NODE_TYPE, "WalletAdapter"));
	}
	p_list->push_back(PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Instruction")));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "address_lookup_tables", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Instruction")));
}

void Transaction::from_bytes(const PackedByteArray &bytes) {
	signatures.clear();
	instructions.clear();
	const unsigned int MINIMUM_MESSAGE_SIZE = 32 + 4 + 1;
	const unsigned int MINIMUM_TRANSACTION_SIZE = MINIMUM_MESSAGE_SIZE + 1;
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() < MINIMUM_TRANSACTION_SIZE, "Invalid transaction size");

	int cursor = 0;

	const unsigned int signer_size = bytes[cursor++];
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() < MINIMUM_MESSAGE_SIZE + 1 + signer_size * SIGNATURE_LENGTH, "Invalid message size.");
	for (unsigned int i = 0; i < signer_size; i++) {
		const PackedByteArray signature_bytes = bytes.slice(cursor, cursor + SIGNATURE_LENGTH);
		if (!are_all_bytes_zeroes(signature_bytes)) {
			ready_signature_amount += 1;
		}
		signatures.append(bytes.slice(cursor, cursor + SIGNATURE_LENGTH));

		cursor += SIGNATURE_LENGTH;
	}

	message.create(bytes.slice(cursor));
	address_lookup_tables = message.get_address_lookup_tables();
	signers.resize(signer_size);
	deserialized = true;

	check_fully_signed();
}

Variant Transaction::new_from_bytes(const PackedByteArray &bytes) {
	Transaction *result = memnew_custom(Transaction);
	result->from_bytes(bytes);

	return result;
}

Transaction::Transaction() :
		subscribe_client(memnew_custom(SolanaClient)), blockhash_client(memnew_custom(SolanaClient)), send_client(memnew_custom(SolanaClient)) {
	// Override because we call process functions ourselves.
	set_async_override(true);
}

void Transaction::_ready() {
}

void Transaction::_process(double delta) {
	SolanaClient::_process(delta);

	// Detect new connections after transaction is performed.
	if (!result_signature.is_empty()) {
		if ((get_signal_connection_list("processed").size() != 0U) && (processed_connections == 0U)) {
			processed_connections = get_signal_connection_list("processed").size();
			subscribe_to_signature(PROCESSED);
		}
		if ((get_signal_connection_list("confirmed").size() != 0U) && (confirmed_connections == 0U)) {
			confirmed_connections = get_signal_connection_list("confirmed").size();
			subscribe_to_signature(CONFIRMED);
		}
		if ((get_signal_connection_list("finalized").size() != 0U) && (finalized_connections == 0U)) {
			finalized_connections = get_signal_connection_list("finalized").size();
			subscribe_to_signature(FINALIZED);
		}
	}
}

void Transaction::set_instructions(const Array &p_value) {
	instructions = p_value;
	reset_state();
	create_message();
}

Array Transaction::get_instructions() {
	return instructions;
}

void Transaction::set_payer(const Variant &p_value) {
	if (deserialized) {
		WARN_PRINT_ED_CUSTOM("Cannot set payer on deserialized transaction");
		return;
	}
	payer = p_value;
	reset_state();
	create_message();
}

Variant Transaction::get_payer() {
	return payer;
}

void Transaction::set_external_payer(bool p_value) {
	external_payer = p_value;
	notify_property_list_changed();
}

bool Transaction::get_external_payer() const {
	return external_payer;
}

void Transaction::update_latest_blockhash(const String &custom_hash) {
	//ERR_FAIL_COND_EDMSG(!is_inside_tree(), "Transaction node must be added to scene tree.");

	if (custom_hash.is_empty()) {
		pending_blockhash = true;
		connect("http_response_received", callable_mp(this, &Transaction::blockhash_callback), CONNECT_ONE_SHOT);
		get_latest_blockhash();
	} else {
		latest_blockhash_string = custom_hash;
		message.set_latest_blockhash(custom_hash);
	}
}

void Transaction::add_instruction(const Variant &instruction) {
	instructions.append(instruction);
	reset_state();
	create_message();
}

void Transaction::set_signers(const Array &p_value) {
	message.supply_signers(p_value);
	signers = p_value;
}

Array Transaction::get_signers() {
	return signers;
}

void Transaction::set_unit_limit(const uint32_t value) {
	unit_limit = value;
	create_message();
}

uint32_t Transaction::get_unit_limit() const {
	return unit_limit;
}

void Transaction::set_unit_price(const uint32_t value) {
	unit_price = value;
	create_message();
}

uint32_t Transaction::get_unit_price() const {
	return unit_price;
}

PackedByteArray Transaction::serialize() {
	PackedByteArray serialized_bytes;

	serialized_bytes.append_array(serialize_signers());
	serialized_bytes.append_array(serialize_message());

	return serialized_bytes;
}

PackedByteArray Transaction::serialize_message() {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!message.is_valid(), PackedByteArray(), "Invalid message.");
	message.set_address_lookup_tables(address_lookup_tables);
	return message.serialize();
}

PackedByteArray Transaction::serialize_signers() {
	PackedByteArray serialized_bytes;
	serialized_bytes.append(signatures.size());
	for (unsigned int i = 0; i < signatures.size(); i++) {
		serialized_bytes.append_array(signatures[i]);
	}
	return serialized_bytes;
}

Variant Transaction::sign_and_send() {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(message.get_signers().size() != signatures.size(), Error::ERR_UNCONFIGURED, "Transaction does not have enough signers.");
	connect("fully_signed", Callable(this, "send"), CONNECT_ONE_SHOT);

	sign();

	return OK;
}

void Transaction::send_callback(Dictionary params) {
	pending_send = false;

	if (params.has("result")) {
		result_signature = params["result"];
		copy_connection_state();
		subscribe_to_signature();
	} else {
		check_transaction_simulation(params);
	}
	emit_signal("transaction_response_received", params);
}

void Transaction::blockhash_callback(Dictionary params) {
	pending_blockhash = false;
	if (params.has("result")) {
		const Dictionary blockhash_result = params["result"];
		const Dictionary blockhash_value = blockhash_result["value"];
		latest_blockhash_string = blockhash_value["blockhash"];
		if (message.is_valid()) {
			message.set_latest_blockhash(latest_blockhash_string);
		}
		emit_signal("blockhash_updated", params);
		const Array connected_signals = get_signal_connection_list("send_ready");
		emit_signal("send_ready");
	} else {
		emit_signal("blockhash_update_failed", params);
	}
}

void Transaction::send() {
	//ERR_FAIL_COND_EDMSG(!is_inside_tree(), "Transaction node must be added to scene tree.");

	ERR_FAIL_COND_EDMSG_CUSTOM(message.get_signers().size() != signatures.size(), "Transaction does not have enough signers.");

	const PackedByteArray serialized_bytes = serialize();

	const Callable pending_blockhash_callback(this, "send");
	if (pending_blockhash) {
		const Callable pending_blockhash_callback(this, "send");
		connect("fully_signed", pending_blockhash_callback, CONNECT_ONE_SHOT);
		return;
	}

	pending_send = true;
	connect("http_response_received", callable_mp(this, &Transaction::send_callback), CONNECT_ONE_SHOT);
	send_transaction(SolanaUtils::bs64_encode(serialized_bytes), UINT64_MAX, skip_preflight);
}

Error Transaction::sign() {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!message.is_valid(), Error::ERR_INVALID_DATA, "Invalid message.");

	const Callable pending_blockhash_callback(this, "sign");
	if (pending_blockhash) {
		connect("send_ready", pending_blockhash_callback, CONNECT_ONE_SHOT);
		return ERR_UNAVAILABLE;
	}

	const PackedByteArray msg = serialize();

	signers = message.get_signers();

	for (unsigned int i = 0; i < signers.size(); i++) {
		sign_at_index(i);
	}

	check_fully_signed();

	return OK;
}

Error Transaction::partially_sign(const Array &array) {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!message.is_valid(), Error::ERR_INVALID_DATA, "Invalid message.");

	const Callable pending_blockhash_callback(this, "partially_sign");
	if (pending_blockhash) {
		connect("send_ready", pending_blockhash_callback.bind(array), CONNECT_ONE_SHOT);
		return ERR_UNAVAILABLE;
	}

	signers = message.get_signers();

	for (unsigned int i = 0; i < array.size(); i++) {
		for (unsigned int j = 0; j < signers.size(); j++) {
			if (Pubkey::bytes_from_variant(signers[j]) == Pubkey::bytes_from_variant(array[i])) {
				sign_at_index(j);
			}
		}
	}

	check_fully_signed();

	return OK;
}

void Transaction::set_address_lookup_tables(const Array &address_lookup_tables) {
	this->address_lookup_tables = address_lookup_tables;
}

Array Transaction::get_address_lookup_tables() {
	return address_lookup_tables;
}

} //namespace godot