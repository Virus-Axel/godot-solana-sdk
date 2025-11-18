#include "instruction.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "account_meta.hpp"
#include "compiled_instruction.hpp"
#include "compute_budget.hpp"
#include "merged_account_metas.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

bool Instruction::is_serializable() {
	if (program_id.get_type() != Variant::OBJECT) {
		return false;
	}
	for (unsigned int i = 0; i < accounts.size(); i++) {
		if (accounts[i].get_type() != Variant::OBJECT) {
			return false;
		}
	}
	return true;
}

void Instruction::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_program_id"), &Instruction::get_program_id);
	ClassDB::bind_method(D_METHOD("set_program_id", "p_value"), &Instruction::set_program_id);
	ClassDB::bind_method(D_METHOD("get_data"), &Instruction::get_data);
	ClassDB::bind_method(D_METHOD("set_data", "p_value"), &Instruction::set_data);
	ClassDB::bind_method(D_METHOD("get_accounts"), &Instruction::get_accounts);
	ClassDB::bind_method(D_METHOD("set_accounts", "p_value"), &Instruction::set_accounts);
	ClassDB::bind_method(D_METHOD("serialize"), &Instruction::serialize);

	ClassDB::add_property("Instruction", PropertyInfo(Variant::OBJECT, "program_id", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_DEFAULT), "set_program_id", "get_program_id");
	ClassDB::add_property("Instruction", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");
	ClassDB::add_property("Instruction", PropertyInfo(Variant::ARRAY, "accounts", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("AccountMeta")), "set_accounts", "get_accounts");
}

void Instruction::set_program_id(const Variant &p_value) {
	program_id = p_value;
}

Variant Instruction::get_program_id() const {
	return program_id;
}

void Instruction::set_data(const PackedByteArray &p_value) {
	data = p_value;
}
PackedByteArray Instruction::get_data() const {
	return data;
}

void Instruction::set_accounts(const TypedArray<AccountMeta> &p_value) {
	accounts = p_value;
}

TypedArray<AccountMeta> Instruction::get_accounts() const {
	return accounts;
}

void Instruction::append_meta(const AccountMeta &meta) {
	accounts.append(memnew_custom(AccountMeta(meta)));
}

PackedByteArray Instruction::serialize() {
	PackedByteArray result;

	const PackedByteArray pid_bytes = Pubkey::bytes_from_variant(program_id);
	if (!pid_bytes.is_empty()) {
		result.append_array(pid_bytes);
	} else {
		WARN_PRINT_ED_CUSTOM("Program ID is not set or invalid in instruction.");
	}

	result.resize(static_cast<int64_t>(result.size() + sizeof(uint64_t)));
	result.encode_u64(static_cast<int64_t>(result.size() - sizeof(uint64_t)), accounts.size());

	for (unsigned int i = 0; i < accounts.size(); i++) {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(!AccountMeta::is_account_meta(accounts[i]), PackedByteArray(), "Element at index " + itos(i) + " is not of type AccountMeta.");
		const AccountMeta *meta = Object::cast_to<AccountMeta>(accounts[i]);
		result.append_array(Pubkey::bytes_from_variant(accounts[i]));
		result.append(meta->get_is_signer() ? 1 : 0);
		result.append(meta->get_writeable() ? 1 : 0);
	}
	result.append_array(data);

	return result;
}

CompiledInstruction::CompiledInstruction() {
	accounts = PackedByteArray();
	data = PackedByteArray();
}

uint16_t CompiledInstruction::create_from_bytes(const PackedByteArray &bytes) {
	const uint16_t MAX_TRANSACTION_SIZE = 1232;
	ERR_FAIL_COND_V_CUSTOM(bytes.size() > MAX_TRANSACTION_SIZE, 0);

	const unsigned int MINIMUM_COMPILED_INSTRUCTION_SIZE = 3;
	ERR_FAIL_COND_V_EDMSG_CUSTOM(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE, 0, "Invalid compiled instruction.");

	uint32_t cursor = 0;
	const uint8_t compiled_program_id_index = bytes[cursor++];

	const unsigned int accounts_len = SolanaUtils::short_u16_decode(bytes, &cursor);

	ERR_FAIL_COND_V_EDMSG_CUSTOM(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE + accounts_len, 0, "Invalid compiled instruction.");
	const PackedByteArray compiled_accounts = bytes.slice(cursor, cursor + accounts_len);
	cursor += accounts_len;

	const unsigned int data_len = SolanaUtils::short_u16_decode(bytes, &cursor);

	ERR_FAIL_COND_V_EDMSG_CUSTOM(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE + accounts_len + data_len, 0, "Invalid compiled instruction.");
	ERR_FAIL_COND_V_CUSTOM(cursor + data_len > MAX_TRANSACTION_SIZE, 0);
	// Check data_len and account_len separately as they can cause overflow.
	ERR_FAIL_COND_V_CUSTOM(data_len > MAX_TRANSACTION_SIZE, 0);
	ERR_FAIL_COND_V_CUSTOM(accounts_len > MAX_TRANSACTION_SIZE, 0);

	this->data = bytes.slice(cursor, cursor + data_len);
	this->accounts = compiled_accounts;
	this->program_id_index = compiled_program_id_index;

	return cursor + data_len;
}

void CompiledInstruction::compile(const Instruction *instruction, const MergedAccountMetas &merged_metas) {
	const TypedArray<AccountMeta> &account_metas = instruction->get_accounts();

	data = instruction->get_data();
	AccountMeta *pid_meta = memnew_custom(AccountMeta(instruction->get_program_id(), false, false));
	program_id_index = merged_metas.find(*pid_meta);

	memfree(pid_meta);

	for (unsigned int j = 0; j < account_metas.size(); j++) {
		auto *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
		accounts.push_back(merged_metas.find(*account_meta));
	}
}

void CompiledInstruction::_bind_methods() {
}

bool CompiledInstruction::is_compute_limit(const MergedAccountMetas &merged_metas) const {
	if (data.size() != ComputeBudget::COMPUTE_UNIT_LIMIT_DATA_SIZE) {
		return false;
	}
	if (data[0] != ComputeBudget::UNIT_LIMIT_DISCRIMINATOR) {
		return false;
	}
	return (merged_metas.is_key_at_index(ComputeBudget::get_pid(), program_id_index));
}

bool CompiledInstruction::is_compute_price(const MergedAccountMetas &merged_metas) const {
	if (data.size() != ComputeBudget::COMPUTE_UNIT_PRICE_DATA_SIZE) {
		return false;
	}
	if (data[0] != ComputeBudget::UNIT_PRICE_DISCRIMINATOR) {
		return false;
	}
	return (merged_metas.is_key_at_index(ComputeBudget::get_pid(), program_id_index));
}

PackedByteArray CompiledInstruction::serialize() const {
	PackedByteArray result;

	result.append(program_id_index);
	result.append_array(SolanaUtils::short_u16_encode(accounts.size()));
	result.append_array(accounts);
	result.append_array(SolanaUtils::short_u16_encode(data.size()));
	result.append_array(data);

	return result;
}

void CompiledInstruction::set_data(const PackedByteArray &data) {
	this->data = data;
}

PackedByteArray CompiledInstruction::get_data() const {
	return data;
}

} //namespace godot