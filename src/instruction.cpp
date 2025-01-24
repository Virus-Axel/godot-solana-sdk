#include "instruction.hpp"
#include "keypair.hpp"
#include "solana_utils.hpp"

#include "utils.hpp"

#include <algorithm>
#include <vector>

#include "instructions/compute_budget.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <message.hpp>

namespace godot {

using internal::gdextension_interface_print_warning;

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

Instruction::Instruction() {
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
	accounts.append(memnew(AccountMeta(meta)));
}

PackedByteArray Instruction::serialize() {
	PackedByteArray result;

	if (program_id.has_method("get_bytes")) {
		result.append_array(Object::cast_to<Pubkey>(program_id)->to_bytes());
	}
	result.append_array(data);
	for (unsigned int i = 0; i < accounts.size(); i++) {
		const Pubkey key = accounts[i];

		result.append_array(key.to_bytes());
	}

	return result;
}

Instruction::~Instruction() {
}

CompiledInstruction::CompiledInstruction() {
	accounts = PackedByteArray();
	data = PackedByteArray();
}

int CompiledInstruction::create_from_bytes(const PackedByteArray &bytes) {
	const unsigned int MINIMUM_COMPILED_INSTRUCTION_SIZE = 3;
	ERR_FAIL_COND_V_EDMSG(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE, 0, "Invalid compiled instruction.");

	int cursor = 0;
	program_id_index = bytes[cursor++];

	const unsigned int accounts_len = SolanaUtils::short_u16_decode(bytes, &cursor);

	ERR_FAIL_COND_V_EDMSG(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE + accounts_len, 0, "Invalid compiled instruction.");
	accounts = bytes.slice(cursor, cursor + accounts_len);
	cursor += accounts_len;

	const unsigned int data_len = SolanaUtils::short_u16_decode(bytes, &cursor);

	ERR_FAIL_COND_V_EDMSG(bytes.size() < MINIMUM_COMPILED_INSTRUCTION_SIZE + accounts_len + data_len, 0, "Invalid compiled instruction.");
	data = bytes.slice(cursor, cursor + data_len);

	return cursor + data_len;
}

void CompiledInstruction::compile(const Instruction *instruction, const MergedAccountMetas &merged_metas) {
	const TypedArray<AccountMeta> &account_metas = instruction->get_accounts();

	data = instruction->get_data();
	AccountMeta *pid_meta = memnew(AccountMeta(instruction->get_program_id(), false, false));
	program_id_index = merged_metas.find(*pid_meta);

	memfree(pid_meta);

	for (unsigned int j = 0; j < account_metas.size(); j++) {
		AccountMeta *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
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

PackedByteArray CompiledInstruction::serialize() {
	PackedByteArray result;

	result.append(program_id_index);
	result.append_array(SolanaUtils::short_u16_encode(accounts.size()));

	for (unsigned int i = 0; i < accounts.size(); i++) {
		result.append(accounts[i]);
	}

	result.append_array(SolanaUtils::short_u16_encode(data.size()));
	for (unsigned int i = 0; i < data.size(); i++) {
		result.append(data[i]);
	}

	return result;
}

void CompiledInstruction::set_data(const PackedByteArray &data) {
	this->data = data;
}

PackedByteArray CompiledInstruction::get_data() const {
	return data;
}

CompiledInstruction::~CompiledInstruction() {
}

} //namespace godot