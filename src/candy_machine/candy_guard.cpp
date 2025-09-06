#include "candy_machine/candy_guard.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "candy_machine/candy_guard_access_list.hpp"
#include "candy_machine/candy_machine.hpp"
#include "instruction.hpp"
#include "instructions/associated_token_account.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "instructions/spl_token.hpp"
#include "instructions/system_program.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string MplCandyGuard::PID = "Guard1JwRhJkVH6XZhzoYxeBVQe872VH6QggF4BWmS9g";

PackedByteArray MplCandyGuard::wrap_discriminator() {
	return { 178, 40, 10, 189, 228, 129, 186, 140 }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

PackedByteArray MplCandyGuard::serialize_label(const String &label) {
	const int MAX_LABEL_SIZE = 6;

	PackedByteArray result;

	if (label == "default") {
		result.append(0);
		return result;
	}

	const int32_t BASE_DATA_SIZE = 5;

	result.resize(BASE_DATA_SIZE);
	result[0] = 1;
	result.encode_u32(1, MAX_LABEL_SIZE);

	PackedByteArray fixed_size_data = label.to_ascii_buffer();

	fixed_size_data.resize(MAX_LABEL_SIZE);
	result.append_array(fixed_size_data);

	return result;
}

void MplCandyGuard::_bind_methods() {
	ClassDB::bind_static_method("MplCandyGuard", D_METHOD("initialize", "owner", "authority", "payer", "candy_guard_acl"), &MplCandyGuard::initialize);
	ClassDB::bind_static_method("MplCandyGuard", D_METHOD("mint", "candy_machine_id", "candy_guard_id", "payer", "receiver", "mint", "nft_mint_authority", "collection_mint", "collection_update_authority", "candy_guard_acl", "label"), &MplCandyGuard::mint);
	ClassDB::bind_static_method("MplCandyGuard", D_METHOD("wrap", "candy_guard_id", "guard_authority", "candy_machine_id", "candy_machine_authority"), &MplCandyGuard::wrap);
	ClassDB::bind_static_method("MplCandyGuard", D_METHOD("new_associated_candy_guard_key", "candy_machine_key"), &MplCandyGuard::new_associated_candy_guard_key);

	ClassDB::bind_static_method("MplCandyGuard", D_METHOD("get_pid"), &MplCandyGuard::get_pid);
}

Variant MplCandyGuard::new_limit_counter_pda(const uint8_t mint_guard_id, const Variant &payer, const Variant &candy_machine_key, const Variant &candy_guard_key) {
	Array seeds;
	seeds.append(String("mint_limit").to_ascii_buffer());
	PackedByteArray id_array;
	id_array.append(mint_guard_id);
	seeds.append(id_array);
	seeds.append(Pubkey::bytes_from_variant(payer));
	//seeds.append(Pubkey::bytes_from_variant(get_pid()));
	//seeds.append(Pubkey::bytes_from_variant(MplCandyMachine::get_pid()));
	seeds.append(Pubkey::bytes_from_variant(candy_guard_key));
	seeds.append(Pubkey::bytes_from_variant(candy_machine_key));

	return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant MplCandyGuard::initialize(const Variant &owner, const Variant &candy_guard_authority, const Variant &payer, const Variant &candy_guard_acl) {
	Instruction *result = memnew_custom(Instruction);

	PackedByteArray data = MplCandyMachine::initialize_discriminator();
	data.append_array(Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->serialize());

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	const Variant CANDY_MACHINE_PDA = new_associated_candy_guard_key(owner);
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_PDA, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(owner, true, false)));

	result->append_meta(*memnew_custom(AccountMeta(candy_guard_authority, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(payer, true, false)));

	const Variant SYSTEM_PID = SystemProgram::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PID, false, false)));

	return result;
}

Variant MplCandyGuard::wrap(const Variant &candy_guard_id, const Variant &guard_authority, const Variant &candy_machine_id, const Variant &candy_machine_authority) {
	Instruction *result = memnew_custom(Instruction);

	const PackedByteArray data = wrap_discriminator();

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));

	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(candy_guard_id, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(guard_authority, true, false)));
	result->append_meta(*memnew_custom(AccountMeta(candy_machine_id, false, true)));

	const Variant CANDY_MACHINE_ID = MplCandyMachine::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_ID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(candy_machine_authority, true, false)));

	return result;
}

Variant MplCandyGuard::mint(
		const Variant &candy_machine_id,
		const Variant &guard_account_id,
		const Variant &payer,
		const Variant &receiver,
		const Variant &mint,
		const Variant &nft_mint_authority,
		const Variant &collection_mint,
		const Variant &collection_update_authority,
		const Variant &candy_guard_acl,
		const String &label) {
	Instruction *result = memnew_custom(Instruction);

	PackedByteArray data = MplCandyMachine::mint2_discriminator();

	data.append_array(Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->get_group(label).serialize_mint_settings());
	data.append_array(serialize_label(label));

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	const Variant candy_machine_creator = MplCandyMachine::new_candy_machine_authority_pda(candy_machine_id);

	const Variant associated_token_account = Pubkey::new_associated_token_address(payer, mint, TokenProgram::get_pid());

	result->set_program_id(new_pid);
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(guard_account_id, false, false)));

	const Variant CANDY_MACHINE_ID = MplCandyMachine::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_ID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(candy_machine_id, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(candy_machine_creator, false, true)));

	result->append_meta(*memnew_custom(AccountMeta(payer, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(receiver, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(mint, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(nft_mint_authority, true, false)));

	const Variant MINT_METADATA = MplTokenMetadata::new_associated_metadata_pubkey(mint);
	const Variant MINT_MASTER_EDITION = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(mint);
	result->append_meta(*memnew_custom(AccountMeta(MINT_METADATA, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(MINT_MASTER_EDITION, false, true)));

	result->append_meta(*memnew_custom(AccountMeta(associated_token_account, false, true)));
	const Variant TOKEN_RECORD = TokenProgram::new_token_record_address(associated_token_account, mint);
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_RECORD, false, true)));

	const Variant DELEGATE_RECORD = TokenProgram::new_delegate_record_address(collection_update_authority, collection_mint, candy_machine_creator, TokenProgram::MetaDataDelegateRole::COLLECTION);
	result->append_meta(*memnew_custom(AccountMeta(DELEGATE_RECORD, false, false)));

	result->append_meta(*memnew_custom(AccountMeta(collection_mint, false, false)));

	const Variant COLLECTION_METADATA = MplTokenMetadata::new_associated_metadata_pubkey(collection_mint);
	const Variant COLLECTION_MASTER_EDITION = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint);
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_METADATA, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_MASTER_EDITION, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(collection_update_authority, false, false)));

	const Variant METADATA_PID = MplTokenMetadata::get_pid();
	const Variant TOKEN_PID = TokenProgram::get_pid();
	const Variant ASSOCIATED_TOKEN_PID = AssociatedTokenAccountProgram::get_pid();
	const Variant SYSTEM_PID = SystemProgram::get_pid();
	const Variant SYSVAR_INSTRUCTIONS = Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111");
	const Variant SYSVAR_SLOTHASHES = Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111");
	result->append_meta(*memnew_custom(AccountMeta(METADATA_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(ASSOCIATED_TOKEN_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_INSTRUCTIONS, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_SLOTHASHES, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(new_pid, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(new_pid, false, false)));

	TypedArray<AccountMeta> mint_arg_accounts = Object::cast_to<CandyGuardAccessList>(candy_guard_acl)->get_group(label).get_mint_arg_accounts(receiver, candy_machine_id, guard_account_id);
	for (unsigned int i = 0; i < mint_arg_accounts.size(); i++) {
		result->append_meta(*memnew_custom(AccountMeta(mint_arg_accounts[i])));
	}

	return result;
}

Variant MplCandyGuard::get_pid() {
	return Pubkey::new_from_string(PID.c_str());
}

Variant MplCandyGuard::new_associated_candy_guard_key(const Variant &candy_machine_key) {
	Array seeds;
	seeds.append(String("candy_guard").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(candy_machine_key));
	return Pubkey::new_pda_bytes(seeds, get_pid());
}

} //namespace godot