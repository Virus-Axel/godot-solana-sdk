#include "instructions/spl_token.hpp"
#include "instructions/mpl_token_metadata.hpp"

namespace godot {

const std::string TokenProgram::ID = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";

Variant TokenProgram::_initialize_mint(const Variant &token_program_pid, const Variant &mint_pubkey, const Variant &mint_authority, const Variant &freeze_authority, const uint32_t decimals) {
	Variant result = memnew(Instruction);
	PackedByteArray data;
	if (freeze_authority.get_type() == Variant::OBJECT) {
		data.resize(67);

		data[34] = 1;
		PackedByteArray mint_authority_bytes = Pubkey::bytes_from_variant(mint_authority);
		ERR_FAIL_COND_V_EDMSG(mint_authority_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid mint authority.");

		for (unsigned int i = 0; i < 32; i++) {
			data[35 + i] = mint_authority_bytes[i];
		}
	} else {
		data.resize(35);
		data[34] = 0;
	}

	const unsigned int DISCRIMINATOR = 20;
	data[0] = DISCRIMINATOR;
	data[1] = decimals;

	PackedByteArray mint_authority_bytes = Pubkey::bytes_from_variant(mint_authority);
	ERR_FAIL_COND_V_EDMSG(mint_authority_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid mint authority.");

	for (unsigned int i = 0; i < 32; i++) {
		data[2 + i] = mint_authority_bytes[i];
	}

	Object::cast_to<Instruction>(result)->set_program_id(token_program_pid);
	Object::cast_to<Instruction>(result)->set_data(data);

	Object::cast_to<Instruction>(result)->append_meta(*memnew(AccountMeta(mint_pubkey, false, true)));

	Variant rent = memnew(Pubkey);
	Object::cast_to<Pubkey>(rent)->from_string("SysvarRent111111111111111111111111111111111");
	Object::cast_to<Instruction>(result)->append_meta(*memnew(AccountMeta(rent, false, false)));

	return result;
}

Variant TokenProgram::_initialize_account(const Variant &token_program_pid, const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;

	data.resize(33);

	const unsigned int DISCRIMINATOR = 18;
	data[0] = DISCRIMINATOR;

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner_pubkey);
	ERR_FAIL_COND_V_EDMSG(owner_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");
	for (unsigned int i = 0; i < 32; i++) {
		data[1 + i] = owner_bytes[i];
	}

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(mint_pubkey, false, false)));

	return result;
}

Variant TokenProgram::_mint_to(const Variant &token_program_pid, const Variant &mint_pubkey, const Variant &account_pubkey, const Variant &owner_pubkey, const Variant &mint_authority, uint64_t amount) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;

	data.resize(9);
	const unsigned int DISCRIMINATOR = 7;
	data[0] = DISCRIMINATOR;
	data.encode_u64(1, amount);

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner_pubkey);

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(mint_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(owner_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(mint_authority, true, false)));

	return result;
}

Variant TokenProgram::_transfer_checked(const Variant &token_program_pid, const Variant &source_pubkey, const Variant &mint_pubkey, const Variant &destination_pubkey, const Variant &source_authority, uint64_t amount, uint32_t decimals) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;

	data.resize(10);
	const unsigned int DISCRIMINATOR = 12;
	data[0] = DISCRIMINATOR;
	data.encode_u64(1, amount);
	data[9] = decimals;

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(source_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(mint_pubkey, false, false)));
	result->append_meta(*memnew(AccountMeta(destination_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(source_authority, true, false)));

	return result;
}

Variant TokenProgram::_freeze_account(const Variant &token_program_pid, const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey, const Variant &freeze_authority) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;

	data.resize(1);
	const unsigned int DISCRIMINATOR = 10;
	data[0] = DISCRIMINATOR;

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew(AccountMeta(mint_pubkey, false, false)));
	result->append_meta(*memnew(AccountMeta(freeze_authority, true, false)));

	return result;
}

Variant TokenProgram::_set_authority(const Variant &token_program_pid, const Variant &mint_account, const Variant &current_authority, const Variant &new_authority, const uint8_t authority_type) {
	Instruction *result = memnew(Instruction);
	PackedByteArray data;

	data.resize(3);
	const unsigned int DISCRIMINATOR = 6;
	data[0] = DISCRIMINATOR;
	data[1] = authority_type;
	if (new_authority.get_type() != Variant::NIL) {
		data[2] = 1;
		data.append_array(Pubkey::bytes_from_variant(new_authority));
	} else {
		data[2] = 0;
	}

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew(AccountMeta(mint_account, false, true)));
	result->append_meta(*memnew(AccountMeta(current_authority, true, false)));

	return result;
}

void TokenProgram::_bind_methods() {
	ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram::initialize_mint);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram::initialize_account);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram::mint_to);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram::transfer_checked);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "owner_pubkey", "freeze_authority"), &TokenProgram::freeze_account);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("set_authority", "mint_account", "current_authority", "new_authority", "authority_type"), &TokenProgram::set_authority);

	ClassDB::bind_static_method("TokenProgram", D_METHOD("new_token_record_address", "omner", "mint"), &TokenProgram::new_token_record_address);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("get_pid"), &TokenProgram::get_pid);
}

// TODO(Virax): Data is unexplained. Fix this

Variant TokenProgram::new_token_record_address(const Variant &token, const Variant &mint) {
	Array seeds;

	seeds.append(String("metadata").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(MplTokenMetadata::get_pid()));
	seeds.append(Pubkey::bytes_from_variant(mint));
	seeds.append(String("token_record").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(token));

	return Pubkey::new_pda_bytes(seeds, MplTokenMetadata::get_pid());
}

Variant TokenProgram::new_delegate_record_address(const Variant &update_authority, const Variant &mint, const Variant &delegate_address, const MetaDataDelegateRole role) {
	Array seeds;
	seeds.append(String("metadata").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(MplTokenMetadata::get_pid()));
	seeds.append(Pubkey::bytes_from_variant(mint));

	// TODO(Virax): Change based on delegate role.
	seeds.append(String("collection_delegate").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(update_authority));
	seeds.append(Pubkey::bytes_from_variant(delegate_address));

	return Pubkey::new_pda_bytes(seeds, MplTokenMetadata::get_pid());
}

Variant TokenProgram::initialize_mint(const Variant &mint_pubkey, const Variant &mint_authority, const Variant &freeze_authority, const uint32_t decimals) {
	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	return _initialize_mint(new_pid, mint_pubkey, mint_authority, freeze_authority, decimals);
}

Variant TokenProgram::initialize_account(const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey) {
	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	return _initialize_account(new_pid, account_pubkey, mint_pubkey, owner_pubkey);
}

Variant TokenProgram::mint_to(const Variant &mint_pubkey, const Variant &account_pubkey, const Variant &owner_pubkey, const Variant &mint_authority, uint64_t amount) {
	const Variant new_pid = Pubkey::new_from_string(String(ID.c_str()));
	return _mint_to(new_pid, mint_pubkey, account_pubkey, owner_pubkey, mint_authority, amount);
}

Variant TokenProgram::transfer_checked(const Variant &source_pubkey, const Variant &mint_pubkey, const Variant &destination_pubkey, const Variant &source_authority, uint64_t amount, uint32_t decimals) {
	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	return _transfer_checked(new_pid, source_pubkey, mint_pubkey, destination_pubkey, source_authority, amount, decimals);
}

Variant TokenProgram::freeze_account(const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey, const Variant &freeze_authority) {
	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	return _freeze_account(new_pid, account_pubkey, mint_pubkey, owner_pubkey, freeze_authority);
}

Variant TokenProgram::set_authority(const Variant &mint_account, const Variant &current_authority, const Variant &new_authority, const uint8_t authority_type) {
	const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
	return _set_authority(new_pid, mint_account, current_authority, new_authority, authority_type);
}

Variant TokenProgram::get_pid() {
	return Pubkey::new_from_string(ID.c_str());
}

} //namespace godot