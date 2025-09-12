#include "system_program.hpp"

#include <cstdint>
#include <string>

#include "account_meta.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "instruction.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string SystemProgram::PID = "11111111111111111111111111111111";

void SystemProgram::_bind_methods() {
	ClassDB::bind_static_method("SystemProgram", D_METHOD("transfer", "sender_pubkey", "reciever_pubkey", "lamports"), &SystemProgram::transfer);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("create_account", "from_keypair", "to_keypair", "lamports", "space", "owner"), &SystemProgram::create_account);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("create_account_with_seed", "from_keypair", "base_keypair", "seed", "lamports", "space", "owner"), &SystemProgram::create_account_with_seed);

	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_pid"), &SystemProgram::get_pid);

	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_clock_pubkey"), &SystemProgram::get_sysvar_clock_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_epoch_schedule_pubkey"), &SystemProgram::get_sysvar_epoch_schedule_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_fees_pubkey"), &SystemProgram::get_sysvar_fees_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_instructions_pubkey"), &SystemProgram::get_sysvar_instructions_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_recent_blockhashes_pubkey"), &SystemProgram::get_sysvar_recent_blockhashes_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_rent_pubkey"), &SystemProgram::get_sysvar_rent_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_slot_hashes_pubkey"), &SystemProgram::get_sysvar_slot_hashes_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_slot_history_pubkey"), &SystemProgram::get_sysvar_slot_history_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_stake_history_pubkey"), &SystemProgram::get_sysvar_stake_history_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_sysvar_epoch_rewards_pubkey"), &SystemProgram::get_sysvar_epoch_rewards_pubkey);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_last_restart_slot_pubkey"), &SystemProgram::get_last_restart_slot_pubkey);
}

Variant SystemProgram::get_sysvar_clock_pubkey() {
	return Pubkey::new_from_string("SysvarC1ock11111111111111111111111111111111");
}

Variant SystemProgram::get_sysvar_epoch_schedule_pubkey() {
	return Pubkey::new_from_string("SysvarEpochSchedu1e111111111111111111111111");
}

Variant SystemProgram::get_sysvar_fees_pubkey() {
	return Pubkey::new_from_string("SysvarFees111111111111111111111111111111111");
}

Variant SystemProgram::get_sysvar_instructions_pubkey() {
	return Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111");
}

Variant SystemProgram::get_sysvar_recent_blockhashes_pubkey() {
	return Pubkey::new_from_string("SysvarRecentB1ockHashes11111111111111111111");
}

Variant SystemProgram::get_sysvar_rent_pubkey() {
	return Pubkey::new_from_string("SysvarRent111111111111111111111111111111111");
}

Variant SystemProgram::get_sysvar_slot_hashes_pubkey() {
	return Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111");
}

Variant SystemProgram::get_sysvar_slot_history_pubkey() {
	return Pubkey::new_from_string("SysvarS1otHistory11111111111111111111111111");
}

Variant SystemProgram::get_sysvar_stake_history_pubkey() {
	return Pubkey::new_from_string("SysvarStakeHistory1111111111111111111111111");
}

Variant SystemProgram::get_sysvar_epoch_rewards_pubkey() {
	return Pubkey::new_from_string("SysvarEpochRewards1111111111111111111111111");
}

Variant SystemProgram::get_last_restart_slot_pubkey() {
	return Pubkey::new_from_string("SysvarLastRestartS1ot1111111111111111111111");
}

Variant SystemProgram::create_account(const Variant &from_keypair, const Variant &to_keypair, const uint64_t lamports, const uint64_t space, const Variant &owner) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(CREATE_ACCOUNT_DATA_SIZE);

	const int64_t LAMPORTS_LOCATION = 4;
	const int64_t SPACE_LOCATION = 12;
	const int64_t OWNER_LOCATION = 20;

	data.encode_u32(0, MethodNumber::CREATE_ACCOUNT);
	data.encode_u64(LAMPORTS_LOCATION, static_cast<int64_t>(lamports));
	data.encode_u64(SPACE_LOCATION, static_cast<int64_t>(space));

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner);
	for (unsigned int i = 0; i < owner_bytes.size(); i++) {
		data[OWNER_LOCATION + i] = owner_bytes[i];
	}

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(from_keypair, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(to_keypair, true, true)));

	return result;
}

Variant SystemProgram::create_account_with_seed(const Variant &from_keypair, const Variant &base_keypair, const String &seed, const uint64_t lamports, const uint64_t space, const Variant &owner) {
	Variant result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(CREATE_ACCOUNT_WITH_SEED_DATA_SIZE + seed.length());

	data.encode_u32(0, MethodNumber::CREATE_ACCOUNT_WITH_SEED);

	const int64_t SEED_LENGTH_LOCATION = 36;
	const int64_t SEED_LOCATION = 44;

	data.encode_u64(SEED_LENGTH_LOCATION, seed.length());
	for (unsigned int i = 0; i < seed.length(); i++) {
		data[SEED_LOCATION + i] = seed.to_ascii_buffer()[i];
	}

	const int64_t LAMPORTS_LOCATION = 44 + seed.length();
	const int64_t SPACE_LOCATION = 52 + seed.length();
	data.encode_u64(LAMPORTS_LOCATION, static_cast<int64_t>(lamports));
	data.encode_u64(SPACE_LOCATION, static_cast<int64_t>(space));

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner);
	PackedByteArray base_bytes = Pubkey::bytes_from_variant(base_keypair);

	ERR_FAIL_COND_V_EDMSG_CUSTOM(owner_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");
	ERR_FAIL_COND_V_EDMSG_CUSTOM(base_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");

	for (unsigned int i = 0; i < PUBKEY_LENGTH; i++) {
		const int64_t OWNER_LOCATION = 60 + seed.length();
		const int64_t BASE_LOCATION = 4;
		data[OWNER_LOCATION + i] = owner_bytes[i];
		data[BASE_LOCATION + i] = base_bytes[i];
	}

	const Variant created_account_key = Pubkey::new_from_seed(base_keypair, seed, owner);

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	Object::cast_to<Instruction>(result)->set_program_id(new_pid);
	Object::cast_to<Instruction>(result)->set_data(data);

	Object::cast_to<Instruction>(result)->append_meta(*memnew_custom(AccountMeta(from_keypair, true, true)));
	Object::cast_to<Instruction>(result)->append_meta(*memnew_custom(AccountMeta(created_account_key, false, true)));
	Object::cast_to<Instruction>(result)->append_meta(*memnew_custom(AccountMeta(base_keypair, true, false)));

	return result;
}

Variant SystemProgram::transfer(const Variant &sender, const Variant &reciever, const uint64_t lamports) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(TRANSFER_DATA_SIZE);

	data[0] = 2;
	data.encode_u64(4, static_cast<int64_t>(lamports));

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(sender, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(reciever, false, true)));

	return result;
}

Variant SystemProgram::get_pid() {
	return memnew_custom(Pubkey(String(PID.c_str())));
}

} //namespace godot