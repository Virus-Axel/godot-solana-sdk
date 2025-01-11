#include "message.hpp"

#include <cstdint>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "address_lookup_table.hpp"
#include "compiled_instruction.hpp"
#include "instruction.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

const unsigned char V0_INDICATOR = 128;

void Message::compile_instruction(const Variant &instruction) {
	auto *element = Object::cast_to<Instruction>(instruction);
	const TypedArray<AccountMeta> &account_metas = element->get_accounts();

	CompiledInstruction *compiled_instruction = memnew_custom(CompiledInstruction);

	compiled_instruction->data = element->get_data();
	const Variant pid_meta = memnew_custom(AccountMeta(element->get_program_id(), false, false));
	compiled_instruction->program_id_index = meta_list.find(*Object::cast_to<AccountMeta>(pid_meta));

	for (unsigned int j = 0; j < account_metas.size(); j++) {
		auto *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
		compiled_instruction->accounts.push_back(meta_list.find(*account_meta));
	}
	compiled_instructions.push_back(compiled_instruction);
}

void Message::recalculate_headers() {
	num_required_signatures = 0;
	num_readonly_signed_accounts = 0;
	num_readonly_unsigned_accounts = 0;
	account_keys.clear();
	for (unsigned int i = 0; i < meta_list.get_list().size(); i++) {
		auto *account_meta = Object::cast_to<AccountMeta>(meta_list.get_list()[i]);
		if (account_meta->get_is_signer()) {
			num_required_signatures++;
			if (!account_meta->get_writeable()) {
				num_readonly_signed_accounts++;
			}
		} else if (!account_meta->get_writeable()) {
			num_readonly_unsigned_accounts++;
		}

		account_keys.push_back(account_meta->get_pubkey());
	}
}

void Message::read_accounts_from_bytes(const PackedByteArray &bytes, uint8_t num_accounts, bool is_signer, bool is_writable, int64_t &cursor) {
	for (uint8_t i = 0; i < num_accounts; i++) {
		account_keys.append(Pubkey::new_from_bytes(bytes.slice(cursor, cursor + static_cast<int64_t>(PUBKEY_LENGTH))));
		const Variant new_meta = AccountMeta::new_account_meta(account_keys.back(), is_signer, is_writable);
		meta_list.add(new_meta);
		cursor += static_cast<int64_t>(PUBKEY_LENGTH);
	}
}

void Message::compile_instructions(const TypedArray<Instruction> &instructions) {
	compiled_instructions.clear();
	for (unsigned int i = 0; i < instructions.size(); i++) {
		CompiledInstruction *compiled_instruction = memnew_custom(CompiledInstruction);
		compiled_instruction->compile(Object::cast_to<Instruction>(instructions[i]), meta_list);
		compiled_instructions.push_back(compiled_instruction);
	}
}

void Message::supply_signers(const Array &signers) {
	meta_list.supply_signers(signers);
}

void Message::create(const MergedAccountMetas &merged_meta_list, Variant &payer) {
	meta_list = merged_meta_list;

	const Variant payer_meta = AccountMeta::new_account_meta(payer, true, true);

	meta_list.add(payer_meta);
	meta_list.sort();

	payer_index = meta_list.find(*(Object::cast_to<AccountMeta>(payer_meta)));

	recalculate_headers();
}

void Message::create(const PackedByteArray &bytes) {
	int64_t cursor = 0;
	compiled_instructions.clear();
	account_keys.clear();
	meta_list.clear();

	// blockhash + number of accounts + compiled instruction size
	const unsigned int minimum_remaining_size = BLOCKHASH_LENGTH + 1 + 1;
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() < minimum_remaining_size, "Invalid accounts size");

	num_required_signatures = bytes[cursor++];

	is_versioned_transaction = false;

	if (num_required_signatures >= V0_INDICATOR) {
		is_versioned_transaction = true;
		num_required_signatures = bytes[cursor++];
		num_readonly_signed_accounts = bytes[cursor++];
	} else {
		num_readonly_signed_accounts = bytes[cursor++];
	}

	num_readonly_unsigned_accounts = bytes[cursor++];

	const uint8_t account_size = bytes[cursor++];
	const uint8_t writable_signed_accounts = num_required_signatures - num_readonly_signed_accounts;
	const uint8_t writable_unsigned_accounts = account_size - writable_signed_accounts - num_readonly_signed_accounts - num_readonly_unsigned_accounts;

	// Check that account sizes add up.
	ERR_FAIL_COND_EDMSG_CUSTOM(bytes.size() < minimum_remaining_size + account_size * 32, "Invalid accounts size");
	ERR_FAIL_COND_EDMSG_CUSTOM(account_size != (writable_unsigned_accounts + writable_signed_accounts + num_readonly_signed_accounts + num_readonly_unsigned_accounts), "Invalid account sizes.");

	read_accounts_from_bytes(bytes, writable_signed_accounts, true, true, cursor);
	read_accounts_from_bytes(bytes, num_readonly_signed_accounts, true, false, cursor);
	read_accounts_from_bytes(bytes, writable_unsigned_accounts, false, true, cursor);
	read_accounts_from_bytes(bytes, num_readonly_unsigned_accounts, false, false, cursor);

	recalculate_headers();

	latest_blockhash = Pubkey::string_from_variant(bytes.slice(cursor, cursor + static_cast<int64_t>(PUBKEY_LENGTH)));
	cursor += static_cast<int64_t>(PUBKEY_LENGTH);
	const uint8_t compiled_instructions_size = bytes[cursor++];

	for (int i = 0; i < compiled_instructions_size; i++) {
		CompiledInstruction *new_instruction = memnew_custom(CompiledInstruction);
		const int consumed_bytes = new_instruction->create_from_bytes(bytes.slice(cursor));
		ERR_FAIL_COND_CUSTOM(consumed_bytes == 0);

		compiled_instructions.append(new_instruction);
		cursor += static_cast<int64_t>(consumed_bytes);
	}

	if (is_versioned_transaction) {
		const unsigned int amount_of_lookup_tables = bytes[cursor++];
		for (unsigned int i = 0; i < amount_of_lookup_tables; i++) {
			unsigned int consumed_bytes = 0;
			const Variant lookup_table = memnew_custom(AddressLookupTable(bytes.slice(cursor), consumed_bytes));
			address_lookup_tables.append(lookup_table);

			cursor += static_cast<int64_t>(consumed_bytes);
		}
	}
}

void Message::set_latest_blockhash(const String &blockhash) {
	latest_blockhash = blockhash;
}

PackedByteArray Message::serialize() {
	PackedByteArray result;

	result.append(num_required_signatures);
	result.append(num_readonly_signed_accounts);
	result.append(num_readonly_unsigned_accounts);

	result.append(account_keys.size());
	for (unsigned int i = 0; i < account_keys.size(); i++) {
		result.append_array(Pubkey::bytes_from_variant(account_keys[i]));
	}

	result.append_array(serialize_blockhash());
	const PackedByteArray temp = serialize_blockhash();

	result.append(compiled_instructions.size());

	for (unsigned int i = 0; i < compiled_instructions.size(); i++) {
		auto *inst = Object::cast_to<CompiledInstruction>(compiled_instructions[i]);
		result.append_array(inst->serialize());
	}

	// TODO(Virax): The lookup table is set if the transaction is imported as a V0.
	// Support to specify lookup table from editor is to be implemented.
	if (is_versioned()) {
		// Append Versioned flag.
		result.insert(0, V0_INDICATOR);
		result.append_array(serialize_lookup_tables());
	}

	return result;
}

Array Message::get_signers() {
	return meta_list.get_signers();
}

void Message::set_address_lookup_tables(const Array &address_lookup_tables) {
	this->address_lookup_tables = address_lookup_tables;
}

Array Message::get_address_lookup_tables() {
	return address_lookup_tables;
}

bool Message::is_versioned() {
	if (is_versioned_transaction) {
		return true;
	}
	if (address_lookup_tables.is_empty()) {
		return false;
	}
	for (unsigned int i = 0; i < address_lookup_tables.size(); i++) {
		if (address_lookup_tables[i].get_type() != Variant::OBJECT) {
			WARN_PRINT_ED("Transaction lookup table type is unknown.");
			return false;
		}
		if (!(static_cast<Object *>(address_lookup_tables[i]))->is_class("AddressLookupTable")) {
			WARN_PRINT_ED("Transaction lookup table type is unknown.");
			return false;
		}
	}

	return true;
}

bool Message::is_valid() const {
	return !(meta_list.is_empty() || compiled_instructions.is_empty() || account_keys.is_empty());
}

PackedByteArray Message::serialize_blockhash() {
	if (latest_blockhash.is_empty()) {
		PackedByteArray result;
		result.resize(static_cast<int64_t>(BLOCKHASH_LENGTH));
		return result;
	}

	return SolanaUtils::bs58_decode(latest_blockhash);
}

PackedByteArray Message::serialize_lookup_tables() {
	PackedByteArray result;
	result.append(static_cast<unsigned char>(address_lookup_tables.size()));

	for (unsigned int i = 0; i < address_lookup_tables.size(); i++) {
		result.append_array(Object::cast_to<AddressLookupTable>(address_lookup_tables[i])->serialize());
	}

	return result;
}

TypedArray<CompiledInstruction> Message::get_compiled_instructions() const {
	return compiled_instructions;
}

int Message::get_amount_signers() const {
	return num_required_signatures;
}
