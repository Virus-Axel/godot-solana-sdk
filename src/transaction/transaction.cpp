#include "transaction.hpp"

#include <atomic>
#include <cstdint>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/hashing_context.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/object_id.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/variant/callable_method_pointer.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "account_meta.hpp"
#include "instruction.hpp"
#include "isigner.hpp"
#include "keypair.hpp"
#include "local_keypair_signer.hpp"
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

	ClassDB::bind_method(D_METHOD("_isigner_signed", "request_id", "sigs"), &Transaction::_isigner_signed);
	ClassDB::bind_method(D_METHOD("_isigner_failed", "request_id", "error_code", "message"), &Transaction::_isigner_failed);

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
	removed_path_taken_ = false;
}

// Story 1-3 ISigner refactor consolidated three signer pathways (native ISigner,
// raw-Keypair compat path with deprecation+v1.2-removal branch, and raw-WalletAdapter
// compat path) into a single dispatch. Threshold 25, measured 26 — splitting would
// obscure the path-symmetry that the unit tests rely on. Future work (Story 5-6)
// may refactor the compat branches into helpers.
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void Transaction::sign_at_index(const uint32_t index) {
	if (signers[index].get_type() != Variant::Type::OBJECT) {
		Array params;
		params.append(Variant::get_type_name(signers[index].get_type()));
		WARN_PRINT_ONCE_ED(String("Signer is not an object. It is a {0}").format(params));
		return;
	}

	// PRIMARY PATH (AC-2): native ISigner. Both godot_solana_sdk::ISigner
	// implementations (LocalKeypairSigner, WalletAdapterSigner, MobileWalletAdapter from
	//) flow through this branch.
	auto *isigner_native = Object::cast_to<godot_solana_sdk::ISigner>(signers[index]);
	if (isigner_native != nullptr) {
		const String request_id = allocate_isigner_request_id(static_cast<int32_t>(index));

		const Callable on_signed(this, "_isigner_signed");
		const Callable on_failed(this, "_isigner_failed");
		// fix (Q2=(a) source fix, 2026-04-30): `ISigner` declares a
		// zero-arg `is_connected()` that shadows the inherited
		// `Object::is_connected(StringName, Callable) const` 2-arg overload
		// (C++ name-hiding). Cast through `Object *` so overload resolution
		// finds the 2-arg version. See docs/triage/.md.
		if (!static_cast<Object *>(isigner_native)->is_connected("sign_completed", on_signed)) {
			isigner_native->connect("sign_completed", on_signed);
		}
		if (!static_cast<Object *>(isigner_native)->is_connected("sign_failed", on_failed)) {
			isigner_native->connect("sign_failed", on_failed);
		}
		// Track for clean disconnect in NOTIFICATION_PREDELETE (see _notification +
		// disconnect_all_isigner_signers below). Defensive lifecycle hygiene — not tied
		// to a tracked concern.
		// fix (Q2=(a) source fix, 2026-04-30): godot-cpp tightened
		// `ObjectID` to no longer accept implicit `uint64_t` rvalue (the type
		// returned by `get_instance_id()`). Wrap with explicit `ObjectID(...)`
		// constructor.
		isigner_connected_signer_ids_.insert(ObjectID(isigner_native->get_instance_id()));

		// payload is serialize_message — the message portion that ed25519 signs.
		// MWA's sign_transactions API expects the FULL serialized tx (with sig slots);
		// future MobileWalletAdapter must reconcile this convention. For
		// LocalKeypairSigner / WalletAdapterSigner v1.1, message-only is correct.
		const PackedByteArray msg = serialize_message();
		PackedInt32Array lengths;
		lengths.push_back(msg.size());

		// Semantic: this is a Solana transaction signing operation, not arbitrary
		// message signing. Calling sign_transactions (not sign_messages) so future
		// MWA-backed signers route to the correct wallet UX (review-and-approve, not
		// "sign arbitrary blob"). For LocalKeypairSigner the two methods are identical.
		isigner_native->sign_transactions(msg, lengths, request_id);
		return;
	}

	// COMPAT PATH (AC-3, AC-6): raw Keypair → wrap in Ref<LocalKeypairSigner> → dispatch
	// via the same ISigner pipeline. The local Ref keeps the wrapper alive through the
	// SYNCHRONOUS sign_completed emit (Concern 4 resolution: no self-pin needed). The
	// deprecation push_warning is planted by Task 5 here.
	if (Keypair::is_keypair(signers[index]) || Keypair::is_compatible_type(signers[index])) {
#ifdef MWA_ISIGNER_REMOVE_V1_2
		// AC-5 (post-v1.2 removal): the raw-Keypair compat path is gone. Surface a
		// loud ERR_PRINT pointing at the migration doc, mark the request, and short-
		// circuit out. The outer dispatch loop in sign() / partially_sign() detects
		// removed_path_taken_ and aborts on the FIRST offender (Option 2 from the
		// adversarial review): no half-signed transactions, no per-signer ERR_PRINT
		// spam if the array contains multiple raw Keypairs.
		// NOTE: This branch must stay semantically synchronized with the #else
		// deprecation branch below — they handle the same signer flavor; only the
		// version-gated UX differs (warn-and-wrap vs hard-fail).
		ERR_PRINT("Transaction.sign(Keypair) was removed in v1.2; use an ISigner. See docs/migrations/isigner.md.");
		removed_path_taken_ = true;
		return;
#else
		// AC-3 deprecation warning. Fires exactly once per process the first time
		// any Transaction signs via the raw-Keypair compat path. Removed in v1.2
		// (the #ifdef branch above replaces this when MWA_ISIGNER_REMOVE_V1_2 is set).
		static std::atomic<bool> deprecation_warned{false};
		if (!deprecation_warned.exchange(true)) {
			UtilityFunctions::push_warning(
					"Transaction.sign(Keypair) is deprecated; use ISigner. "
					"See docs/migrations/isigner.md. Removed in v1.2.");
		}

		const Variant kp_variant = Keypair::is_keypair(signers[index])
				? signers[index]
				: Keypair::new_from_variant(signers[index]);
		ERR_FAIL_COND_EDMSG_CUSTOM(kp_variant.get_type() != Variant::OBJECT, "Signer is not a Keypair or compatible type.");
		const Ref<Keypair> kp_ref = kp_variant;

		Ref<godot_solana_sdk::LocalKeypairSigner> wrapper;
		wrapper.instantiate();
		wrapper->set_keypair(kp_ref);

		const String request_id = allocate_isigner_request_id(static_cast<int32_t>(index));

		const Callable on_signed(this, "_isigner_signed");
		const Callable on_failed(this, "_isigner_failed");
		wrapper->connect("sign_completed", on_signed);
		wrapper->connect("sign_failed", on_failed);
		// NOTE: wrapper is stack-local; connections die with it on function return.
		// Do NOT track in isigner_connected_signer_ids_ — that's for long-lived signers.

		const PackedByteArray msg = serialize_message();
		PackedInt32Array lengths;
		lengths.push_back(msg.size());

		// Synchronous emit (per AC-6): handler runs INSIDE this call frame, sets
		// signatures[index] before sign_transactions returns. wrapper Ref stays alive
		// through emit chain by virtue of being a stack-local Ref.
		wrapper->sign_transactions(msg, lengths, request_id);
		return;
#endif // MWA_ISIGNER_REMOVE_V1_2
	}

	// COMPAT PATH (v1.1 unchanged per Concern 5): raw WalletAdapter — keep existing
	// async branch with one-shot per-call connections. Deprecation+removal of this
	// path is deferred to a future story.
	if (signers[index].has_method("sign_message")) {
		auto *controller = Object::cast_to<WalletAdapter>(signers[index]);

		controller->connect("message_signed", callable_mp(this, &Transaction::_signer_signed).bindv(Array::make(index)), CONNECT_ONE_SHOT);
		controller->connect("signing_failed", callable_mp(this, &Transaction::_signer_failed).bindv(Array::make(index)), CONNECT_ONE_SHOT);
		controller->sign_message(serialize(), index);
		return;
	}

	ERR_PRINT_ONCE_ED("Unknown signer type.");
}

String Transaction::allocate_isigner_request_id(const int32_t index) {
	const String request_id = String("tx-isigner-req-") + String::num_uint64(static_cast<int64_t>(next_isigner_request_id_++));
	isigner_request_id_to_index_[request_id] = index;
	return request_id;
}

void Transaction::_isigner_signed(const String &request_id, const Array &sigs) {
	if (!isigner_request_id_to_index_.has(request_id)) {
		// Unknown request — likely from another Transaction sharing this signer; ignore.
		return;
	}
	const int32_t index = isigner_request_id_to_index_[request_id];
	isigner_request_id_to_index_.erase(request_id);

	// Code-review MED 6: unify malformed-reply handling. Both empty-array and
	// wrong-size paths now emit signing_failed(index) so the caller's fully_signed
	// signal can never deadlock waiting on a misbehaving ISigner. Previously the
	// wrong-size path used ERR_FAIL_COND_MSG which returned silently and left
	// signatures[index] as a zero-byte placeholder.
	if (sigs.size() != 1) {
		UtilityFunctions::push_error(
				"[Transaction] Signer ", index, " emitted sign_completed with ", sigs.size(),
				" signatures; expected exactly 1 (request_id=", request_id, ").");
		emit_signal("signing_failed", index);
		return;
	}

	signatures[index] = sigs[0];
	ready_signature_amount++;
	emit_signal("signer_state_changed");
	check_fully_signed();
}

void Transaction::_isigner_failed(const String &request_id, const String &error_code, const String &message) {
	if (!isigner_request_id_to_index_.has(request_id)) {
		return;
	}
	const int32_t index = isigner_request_id_to_index_[request_id];
	isigner_request_id_to_index_.erase(request_id);

	// Surface the structured error to the editor / logs. The signing_failed signal
	// itself only carries an int index for backward compatibility; the structured
	// (error_code, message) pair would require a new signal — out of scope for v1.1.
	UtilityFunctions::push_error(
			"[Transaction] Signer ", index, " failed: ", error_code, " — ", message);

	emit_signal("signing_failed", index);
}

void Transaction::_notification(const int p_what) {
	if (p_what == NOTIFICATION_PREDELETE) {
		disconnect_all_isigner_signers();
	}
}

void Transaction::disconnect_all_isigner_signers() {
	const Callable on_signed(this, "_isigner_signed");
	const Callable on_failed(this, "_isigner_failed");
	// NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn) — analyzer
	// traces a hypothetical null-keys path inside godot-cpp's HashSet::Iterator
	// after the loop end has been reached; the analyzer's path is unreachable in
	// practice (operator!= guards against past-the-end deref). Suppress at the
	// site where the false-positive surfaces.
	for (const ObjectID &signer_id : isigner_connected_signer_ids_) {
		Object *obj = ObjectDB::get_instance(signer_id);
		if (obj == nullptr) {
			continue; // signer already destroyed; connections went with it
		}
		if (obj->is_connected("sign_completed", on_signed)) {
			obj->disconnect("sign_completed", on_signed);
		}
		if (obj->is_connected("sign_failed", on_failed)) {
			obj->disconnect("sign_failed", on_failed);
		}
	}
	isigner_connected_signer_ids_.clear();
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
	if (deserialized) {
		WARN_PRINT_ED_CUSTOM("Instructions cannot be decompiled from deserialized transaction");
	}
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
		connect("http_request_completed", callable_mp(this, &Transaction::blockhash_callback), CONNECT_ONE_SHOT);
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
	// Code-review HIGH 1: disconnect previously-connected ISigner instances before
	// swapping the signer set. Without this, late callbacks from the OLD signer set
	// (shared across Transactions, in-flight async resolves) fire into this Transaction
	// with stale request_ids and silently no-op or stomp signatures from a different pass.
	disconnect_all_isigner_signers();

	message.supply_signers(p_value);
	signers = p_value;

	// Code-review HIGH 2: clear stale signing state and re-resize signatures to match
	// the new signer count. Symmetric with set_instructions / set_payer / add_instruction
	// which trigger this via reset_state() + create_message(). We can't call reset_state()
	// directly here because it clears `signers` which we just assigned; inline the safe
	// subset and then run create_message() to resize signatures.
	ready_signature_amount = 0;
	signatures.clear();
	result_signature = "";
	removed_path_taken_ = false;
	create_message();
	// `create_message` early-exits without resizing signatures when `instructions` is
	// empty — the deserialized-then-set_signers flow (`new_from_bytes` + `set_signers`)
	// hits this. Resize inline so signatures.size() always matches signers.size() after
	// set_signers, regardless of which create_message branch ran.
	if (signatures.size() != signers.size()) {
		signatures.resize(signers.size());
		for (int i = 0; i < signatures.size(); i++) {
			PackedByteArray temp;
			temp.resize(SIGNATURE_LENGTH);
			signatures[i] = temp;
		}
	}
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

void Transaction::send_callback(const Error error, Dictionary params) {
	pending_send = false;

	if (params.has("result") && error == Error::OK) {
		result_signature = params["result"];
		copy_connection_state();
		subscribe_to_signature();
	} else {
		check_transaction_simulation(params);
	}
	emit_signal("transaction_response_received", params);
}

void Transaction::blockhash_callback(const Error error, Dictionary params) {
	pending_blockhash = false;
	if (params.has("result") && error == Error::OK) {
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
	connect("http_request_completed", callable_mp(this, &Transaction::send_callback), CONNECT_ONE_SHOT);
	send_transaction(SolanaUtils::bs64_encode(serialized_bytes), UINT64_MAX, skip_preflight);
}

Error Transaction::sign() {
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!message.is_valid(), Error::ERR_INVALID_DATA, "Invalid message.");

	const Callable pending_blockhash_callback(this, "sign");
	if (pending_blockhash) {
		connect("send_ready", pending_blockhash_callback, CONNECT_ONE_SHOT);
		return ERR_UNAVAILABLE;
	}

	// Code-review MED 5: removed dead `const PackedByteArray msg = serialize();` —
	// `msg` was never read; serialize() is expensive (walks signatures + serialize_message);
	// leftover from a prior refactor. Each sign_at_index serializes its own message.

	signers = message.get_signers();

	removed_path_taken_ = false;
	for (unsigned int i = 0; i < signers.size(); i++) {
		sign_at_index(i);
		if (removed_path_taken_) {
			// Abort the dispatch loop on first offender (per Task 7 review Option 2).
			// Leaves signatures[0..i-1] populated only if those slots were dispatched
			// to non-removed signers BEFORE the offender. Caller treats
			// ERR_METHOD_NOT_FOUND as "fully migrate before retrying."
			break;
		}
	}

	if (removed_path_taken_) {
		// Task 7 (AC-5): MWA_ISIGNER_REMOVE_V1_2 build encountered a raw
		// Keypair in signers. Surface to the caller as ERR_METHOD_NOT_FOUND.
		return Error::ERR_METHOD_NOT_FOUND;
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

	// Code-review MED 4: snapshot for rollback if the v1.2 abort-on-first-offender
	// fires. Without this, a partial sign attempt that hits a removed Keypair leaves
	// `signers` overwritten via `signers = message.get_signers()` and any prior valid
	// partial-sigs stranded in `signatures`. The caller getting ERR_METHOD_NOT_FOUND
	// has no clean retry path. Contract: on ERR_METHOD_NOT_FOUND, the Transaction is
	// unmutated relative to its pre-call state.
	const Array signers_snapshot = signers.duplicate();
	const TypedArray<PackedByteArray> signatures_snapshot = signatures.duplicate();
	const uint32_t ready_signature_amount_snapshot = ready_signature_amount;

	signers = message.get_signers();

	removed_path_taken_ = false;
	for (unsigned int i = 0; i < array.size(); i++) {
		if (removed_path_taken_) {
			break; // outer-loop abort on first offender (Option 2)
		}
		for (unsigned int j = 0; j < signers.size(); j++) {
			if (Pubkey::bytes_from_variant(signers[j]) == Pubkey::bytes_from_variant(array[i])) {
				sign_at_index(j);
				if (removed_path_taken_) {
					break; // inner-loop abort on first offender
				}
			}
		}
	}

	if (removed_path_taken_) {
		// MED 4: restore pre-call state — Transaction is unmutated on the error path.
		signers = signers_snapshot;
		signatures = signatures_snapshot;
		ready_signature_amount = ready_signature_amount_snapshot;
		// Task 7 (AC-5): same v1.2 removal semantics as sign.
		return Error::ERR_METHOD_NOT_FOUND;
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

String Transaction::get_message_hash() {
	HashingContext *context = memnew_custom(HashingContext());
	context->start(HashingContext::HashType::HASH_MD5);

	const PackedByteArray serialized_message = serialize_message();
	context->update(serialized_message);
	const String result_hash = context->finish().hex_encode();

	memfree(context);

	return result_hash;
}

} //namespace godot
