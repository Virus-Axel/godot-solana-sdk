#include "system_program.hpp"

#include "instruction.hpp"
#include "keypair.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string SystemProgram::ID = "11111111111111111111111111111111";

void SystemProgram::_bind_methods() {
	ClassDB::bind_static_method("SystemProgram", D_METHOD("transfer", "sender_pubkey", "reciever_pubkey", "lamports"), &SystemProgram::transfer);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("create_account", "from_keypair", "to_keypair", "lamports", "space", "owner"), &SystemProgram::create_account);
	ClassDB::bind_static_method("SystemProgram", D_METHOD("create_account_with_seed", "from_keypair", "base_keypair", "seed", "lamports", "space", "owner"), &SystemProgram::create_account_with_seed);

	ClassDB::bind_static_method("SystemProgram", D_METHOD("get_pid"), &SystemProgram::get_pid);
}

Variant SystemProgram::create_account(const Variant &from_keypair, const Variant &to_keypair, const uint64_t lamports, const uint64_t space, const Variant &owner) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;
	data.resize(CREATE_ACCOUNT_DATA_SIZE);

	data.encode_u32(0, MethodNumber::CREATE_ACCOUNT);
	data.encode_u64(4, lamports);
	data.encode_u64(12, space);

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner);
	for (unsigned int i = 0; i < owner_bytes.size(); i++) {
		data[20 + i] = owner_bytes[i];
	}

	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(from_keypair, true, true)));
	result->append_meta(*memnew(AccountMeta(to_keypair, true, true)));

	return result;
}

Variant SystemProgram::create_account_with_seed(const Variant &from_keypair, const Variant &base_keypair, const String &seed, const uint64_t lamports, const uint64_t space, const Variant &owner) {
	Variant result = memnew(Instruction);
	PackedByteArray data;
	data.resize(CREATE_ACCOUNT_WITH_SEED_DATA_SIZE + seed.length());

	data.encode_u32(0, MethodNumber::CREATE_ACCOUNT_WITH_SEED);
	data.encode_u64(36, seed.length());
	for (unsigned int i = 0; i < seed.length(); i++) {
		data[44 + i] = seed.to_ascii_buffer()[i];
	}

	data.encode_u64(44 + seed.length(), lamports);
	data.encode_u64(52 + seed.length(), space);

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner);
	PackedByteArray base_bytes = Pubkey::bytes_from_variant(base_keypair);

	ERR_FAIL_COND_V_EDMSG_CUSTOM(owner_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");
	ERR_FAIL_COND_V_EDMSG_CUSTOM(base_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");

	for (unsigned int i = 0; i < 32; i++) {
		data[60 + seed.length() + i] = owner_bytes[i];
		data[4 + i] = base_bytes[i];
	}

	Variant created_account_key = Pubkey::new_from_seed(base_keypair, seed, owner);

	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	Object::cast_to<Instruction>(result)->set_program_id(new_pid);
	Object::cast_to<Instruction>(result)->set_data(data);

	Object::cast_to<Instruction>(result)->append_meta(*memnew(AccountMeta(from_keypair, true, true)));
	Object::cast_to<Instruction>(result)->append_meta(*memnew(AccountMeta(created_account_key, false, true)));
	Object::cast_to<Instruction>(result)->append_meta(*memnew(AccountMeta(base_keypair, true, false)));

	return result;
}

Variant SystemProgram::transfer(const Variant &sender, const Variant &reciever, const uint64_t lamports) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;
	data.resize(TRANSFER_DATA_SIZE);

	data[0] = 2;
	data.encode_u64(4, lamports);

	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(sender, true, true)));
	result->append_meta(*memnew(AccountMeta(reciever, false, true)));

	return result;
}

Variant SystemProgram::get_pid() {
	return memnew(Pubkey(String(ID.c_str())));
}

} //namespace godot