#include "instructions/spl_token.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "instruction.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string TokenProgram::PID = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";

const uint8_t TOKEN_MINT_SIZE = 82;
const uint8_t TOKEN_ACCOUNT_SIZE = 165;

Variant TokenProgram::_initialize_mint(const Variant &token_program_pid, const Variant &mint_pubkey, const Variant &mint_authority, const Variant &freeze_authority, const uint32_t decimals) {
	Variant result = memnew_custom(Instruction);
	PackedByteArray data;

	const int64_t BASE_DATA_LENGTH = 35;
	const int64_t OPTIONAL_AUTHORITY_LOCATION = 34;

	if (freeze_authority.get_type() == Variant::OBJECT) {
		data.resize(BASE_DATA_LENGTH + PUBKEY_LENGTH);

		data[OPTIONAL_AUTHORITY_LOCATION] = 1;
		PackedByteArray mint_authority_bytes = Pubkey::bytes_from_variant(mint_authority);
		ERR_FAIL_COND_V_EDMSG_CUSTOM(mint_authority_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid mint authority.");

		for (unsigned int i = 0; i < PUBKEY_LENGTH; i++) {
			data[BASE_DATA_LENGTH + i] = mint_authority_bytes[i];
		}
	} else {
		data.resize(BASE_DATA_LENGTH);
		data[OPTIONAL_AUTHORITY_LOCATION] = 0;
	}

	const unsigned int DISCRIMINATOR = 20;
	data[0] = DISCRIMINATOR;
	data[1] = decimals;

	PackedByteArray mint_authority_bytes = Pubkey::bytes_from_variant(mint_authority);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(mint_authority_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid mint authority.");

	for (unsigned int i = 0; i < PUBKEY_LENGTH; i++) {
		data[2 + i] = mint_authority_bytes[i];
	}

	Object::cast_to<Instruction>(result)->set_program_id(token_program_pid);
	Object::cast_to<Instruction>(result)->set_data(data);

	Object::cast_to<Instruction>(result)->append_meta(*memnew_custom(AccountMeta(mint_pubkey, false, true)));

	const Variant rent = memnew_custom(Pubkey);
	Object::cast_to<Pubkey>(rent)->from_string("SysvarRent111111111111111111111111111111111");
	Object::cast_to<Instruction>(result)->append_meta(*memnew_custom(AccountMeta(rent, false, false)));

	return result;
}

Variant TokenProgram::_initialize_account(const Variant &token_program_pid, const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;

	const int64_t DATA_LENGTH = 33;
	data.resize(DATA_LENGTH);

	const unsigned int DISCRIMINATOR = 18;
	data[0] = DISCRIMINATOR;

	PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner_pubkey);
	ERR_FAIL_COND_V_EDMSG_CUSTOM(owner_bytes.size() != PUBKEY_LENGTH, nullptr, "Invalid owner.");
	for (unsigned int i = 0; i < PUBKEY_LENGTH; i++) {
		data[1 + i] = owner_bytes[i];
	}

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(mint_pubkey, false, false)));

	return result;
}

Variant TokenProgram::_mint_to(const Variant &token_program_pid, const Variant &mint_pubkey, const Variant &account_pubkey, const Variant &owner_pubkey, const Variant &mint_authority, uint64_t amount) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;

	const int64_t DATA_LENGTH = 9;

	data.resize(DATA_LENGTH);
	const unsigned int DISCRIMINATOR = 7;
	data[0] = DISCRIMINATOR;
	data.encode_u64(1, static_cast<int64_t>(amount));

	const PackedByteArray owner_bytes = Pubkey::bytes_from_variant(owner_pubkey);

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(mint_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(owner_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(mint_authority, true, false)));

	return result;
}

Variant TokenProgram::_transfer_checked(const Variant &token_program_pid, const Variant &source_pubkey, const Variant &mint_pubkey, const Variant &destination_pubkey, const Variant &source_authority, uint64_t amount, uint32_t decimals) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;

	const int64_t DATA_LENGTH = 10;
	data.resize(DATA_LENGTH);
	const unsigned int DISCRIMINATOR = 12;
	data[0] = DISCRIMINATOR;

	const int64_t AMOUNT_LOCATION = 1;
	const int64_t DECIMALS_LOCATION = 9;

	data.encode_u64(AMOUNT_LOCATION, static_cast<int64_t>(amount));
	data[DECIMALS_LOCATION] = decimals;

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(source_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(mint_pubkey, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(destination_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(source_authority, true, false)));

	return result;
}

Variant TokenProgram::_freeze_account(const Variant &token_program_pid, const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &freeze_authority) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;

	data.resize(1);
	const unsigned int DISCRIMINATOR = 10;
	data[0] = DISCRIMINATOR;

	result->set_program_id(token_program_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(account_pubkey, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(mint_pubkey, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(freeze_authority, true, false)));

	return result;
}

Variant TokenProgram::_set_authority(const Variant &token_program_pid, const Variant &mint_account, const Variant &current_authority, const Variant &new_authority, const uint8_t authority_type) {
	Instruction *result = memnew_custom(Instruction);
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

	result->append_meta(*memnew_custom(AccountMeta(mint_account, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(current_authority, true, false)));

	return result;
}

void TokenProgram::_bind_methods() {
	ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram::initialize_mint);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram::initialize_account);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram::mint_to);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram::transfer_checked);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "freeze_authority"), &TokenProgram::freeze_account);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("set_authority", "mint_account", "current_authority", "new_authority", "authority_type"), &TokenProgram::set_authority);

	ClassDB::bind_static_method("TokenProgram", D_METHOD("new_token_record_address", "omner", "mint"), &TokenProgram::new_token_record_address);
	ClassDB::bind_static_method("TokenProgram", D_METHOD("get_pid"), &TokenProgram::get_pid);

	BIND_CONSTANT(TOKEN_MINT_SIZE);
	BIND_CONSTANT(TOKEN_ACCOUNT_SIZE);
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

	switch (role) {
		case MetaDataDelegateRole::AUTHORITY_ITEM:
			seeds.append(String("authority_item_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::COLLECTION:
			seeds.append(String("collection_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::USE:
			seeds.append(String("use_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::DATA:
			seeds.append(String("data_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::PROGRAMABLE_CONFIG:
			seeds.append(String("programmable_config_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::DATA_ITEM:
			seeds.append(String("data_item_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::COLLECTION_ITEM:
			seeds.append(String("collection_item_delegate").to_ascii_buffer());
			break;
		case MetaDataDelegateRole::PROGRAMABLE_CONFIG_ITEM:
			seeds.append(String("prog_config_item_delegate").to_ascii_buffer());
			break;
		default:
			ERR_FAIL_V_EDMSG_CUSTOM(nullptr, "Invalid delegate role.");
	}
	seeds.append(Pubkey::bytes_from_variant(update_authority));
	seeds.append(Pubkey::bytes_from_variant(delegate_address));

	return Pubkey::new_pda_bytes(seeds, MplTokenMetadata::get_pid());
}

Variant TokenProgram::initialize_mint(const Variant &mint_pubkey, const Variant &mint_authority, const Variant &freeze_authority, const uint32_t decimals) {
	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	return _initialize_mint(new_pid, mint_pubkey, mint_authority, freeze_authority, decimals);
}

Variant TokenProgram::initialize_account(const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &owner_pubkey) {
	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	return _initialize_account(new_pid, account_pubkey, mint_pubkey, owner_pubkey);
}

Variant TokenProgram::mint_to(const Variant &mint_pubkey, const Variant &account_pubkey, const Variant &owner_pubkey, const Variant &mint_authority, uint64_t amount) {
	const Variant new_pid = Pubkey::new_from_string(String(PID.c_str()));
	return _mint_to(new_pid, mint_pubkey, account_pubkey, owner_pubkey, mint_authority, amount);
}

Variant TokenProgram::transfer_checked(const Variant &source_pubkey, const Variant &mint_pubkey, const Variant &destination_pubkey, const Variant &source_authority, uint64_t amount, uint32_t decimals) {
	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	return _transfer_checked(new_pid, source_pubkey, mint_pubkey, destination_pubkey, source_authority, amount, decimals);
}

Variant TokenProgram::freeze_account(const Variant &account_pubkey, const Variant &mint_pubkey, const Variant &freeze_authority) {
	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	return _freeze_account(new_pid, account_pubkey, mint_pubkey, freeze_authority);
}

Variant TokenProgram::set_authority(const Variant &mint_account, const Variant &current_authority, const Variant &new_authority, const uint8_t authority_type) {
	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	return _set_authority(new_pid, mint_account, current_authority, new_authority, authority_type);
}

Variant TokenProgram::get_pid() {
	return Pubkey::new_from_string(PID.c_str());
}

} //namespace godot