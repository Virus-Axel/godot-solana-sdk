#include "candy_machine/candy_machine.hpp"

#include <cstdint>
#include <string>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "candy_machine/candy_machine_data.hpp"
#include "candy_machine/config_line.hpp"
#include "instruction.hpp"
#include "instructions/associated_token_account.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "instructions/spl_token.hpp"
#include "instructions/system_program.hpp"
#include "meta_data/creator.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

const std::string MplCandyMachine::PID = "CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR";

PackedByteArray MplCandyMachine::mint_discriminator() {
	return { 51, 57, 225, 47, 182, 146, 137, 166 }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

PackedByteArray MplCandyMachine::mint2_discriminator() {
	return { 120, 121, 23, 146, 173, 110, 199, 205 }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

PackedByteArray MplCandyMachine::initialize_discriminator() {
	return { 175, 175, 109, 31, 13, 152, 155, 237 }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

PackedByteArray MplCandyMachine::initialize2_discriminator() {
	return { 67, 153, 175, 39, 218, 16, 38, 32 }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

PackedByteArray MplCandyMachine::add_config_lines_discriminator() {
	return { 0xdf, 0x32, 0xe0, 0xe3, 0x97, 0x08, 0x73, 0x6a }; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void MplCandyMachine::_bind_methods() {
	ClassDB::add_signal("MplCandyMachine", MethodInfo("account_fetched"));

	ClassDB::bind_static_method("MplCandyMachine", D_METHOD("initialize", "authority", "candy_machine_account", "collection_mint", "candy_machine_data", "pnft"), &MplCandyMachine::initialize, DEFVAL(false));
	ClassDB::bind_static_method("MplCandyMachine", D_METHOD("add_config_lines", "candy_machine_key", "authority", "config_lines", "index"), &MplCandyMachine::add_config_lines);
	ClassDB::bind_static_method("MplCandyMachine", D_METHOD("mint", "payer", "receiver", "mint", "collection_mint", "collection_update_authority", "candy_machine_key"), &MplCandyMachine::mint);

	ClassDB::bind_static_method("MplCandyMachine", D_METHOD("get_pid"), &MplCandyMachine::get_pid);
	ClassDB::bind_static_method("MplCandyMachine", D_METHOD("new_candy_machine_authority_pda"), &MplCandyMachine::new_candy_machine_authority_pda);

	ClassDB::bind_method(D_METHOD("fetch_account_callback", "params"), &MplCandyMachine::fetch_account_callback);
	ClassDB::bind_method(D_METHOD("get_candy_machine_info", "candy_machine_key"), &MplCandyMachine::get_candy_machine_info);
}

Variant MplCandyMachine::initialize(const Variant &authority, const Variant &candy_machine_account, const Variant &collection_mint, const Variant &candy_machine_data, bool pnft) {
	Instruction *result = memnew_custom(Instruction);

	PackedByteArray data = initialize2_discriminator();
	data.append_array(Object::cast_to<CandyMachineData>(candy_machine_data)->serialize());
	data.append(4LL * static_cast<int8_t>(pnft));

	result->set_program_id(get_pid());
	result->set_data(data);

	result->append_meta(*memnew_custom(AccountMeta(candy_machine_account, false, true)));

	const Variant CANDY_MACHINE_AUTHORITY = MplCandyMachine::new_candy_machine_authority_pda(candy_machine_account);

	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_AUTHORITY, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(authority, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(authority, true, true)));

	const Variant CANDY_MACHINE_PID = Pubkey::new_from_string("CndyV3LdqHUfDLmE5naZjVN8rBZz4tqhdefbAnjHG3JR");
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_PID, false, false)));

	const Variant COLLECTION_METADATA = MplTokenMetadata::new_associated_metadata_pubkey(collection_mint);
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_METADATA, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(collection_mint, false, false)));
	const Variant COLLECTION_MASTER_EDITION = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint);
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_MASTER_EDITION, false, false)));

	result->append_meta(*memnew_custom(AccountMeta(authority, true, true)));
	const Variant DELEGATE_RECORD = TokenProgram::new_delegate_record_address(authority, collection_mint, CANDY_MACHINE_AUTHORITY, godot::TokenProgram::MetaDataDelegateRole::COLLECTION);
	result->append_meta(*memnew_custom(AccountMeta(DELEGATE_RECORD, false, true)));

	const Variant METADATA_PID = MplTokenMetadata::get_pid();
	const Variant SYSTEM_PROGRAM_PID = SystemProgram::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(METADATA_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PROGRAM_PID, false, false)));

	const Variant SYSVAR_INSTRUCTION = Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111");
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_INSTRUCTION, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE_PID, false, false)));

	return result;
}

Variant MplCandyMachine::mint(
		const Variant &payer,
		const Variant &receiver,
		const Variant &mint,
		const Variant &collection_mint,
		const Variant &collection_update_authority,
		const Variant &candy_machine_key) {
	Instruction *result = memnew_custom(Instruction);

	const PackedByteArray data = mint2_discriminator();

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	const Variant candy_machine_creator = new_candy_machine_authority_pda(candy_machine_key);

	result->append_meta(*memnew_custom(AccountMeta(candy_machine_key, false, true)));

	Array seeds;
	seeds.append(String("candy_machine").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(candy_machine_key));

	const Variant CANDYMACHINE_PID = get_pid();
	const Variant CANDY_MACHINE = Pubkey::new_pda_bytes(seeds, CANDYMACHINE_PID);
	result->append_meta(*memnew_custom(AccountMeta(CANDY_MACHINE, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(payer, true, false)));
	result->append_meta(*memnew_custom(AccountMeta(payer, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(receiver, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(mint, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(payer, true, false)));
	const Variant MINT_METADATA = MplTokenMetadata::new_associated_metadata_pubkey(mint);
	result->append_meta(*memnew_custom(AccountMeta(MINT_METADATA, false, true)));
	const Variant MINT_MASTER_EDITION = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(mint);
	result->append_meta(*memnew_custom(AccountMeta(MINT_MASTER_EDITION, false, true)));

	const Variant associated_token_account = Pubkey::new_associated_token_address(receiver, mint, TokenProgram::get_pid());
	result->append_meta(*memnew_custom(AccountMeta(associated_token_account, false, true)));

	const Variant TOKEN_RECORD = TokenProgram::new_token_record_address(associated_token_account, mint);
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_RECORD, false, true)));

	const Variant TOKEN_DELEGATE = TokenProgram::new_delegate_record_address(collection_update_authority, collection_mint, candy_machine_creator, TokenProgram::MetaDataDelegateRole::COLLECTION);
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_DELEGATE, false, false)));

	result->append_meta(*memnew_custom(AccountMeta(collection_mint, false, false)));
	const Variant COLLECTION_METADATA = MplTokenMetadata::new_associated_metadata_pubkey(collection_mint);
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_METADATA, false, true)));
	const Variant COLLECTION_MASTER_EDITION = MplTokenMetadata::new_associated_metadata_pubkey_master_edition(collection_mint);
	result->append_meta(*memnew_custom(AccountMeta(COLLECTION_MASTER_EDITION, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(collection_update_authority, true, true)));

	const Variant METADATA_PID = MplTokenMetadata::get_pid();
	const Variant TOKEN_PID = TokenProgram::get_pid();
	const Variant ASSOCIATED_TOKEN_PID = AssociatedTokenAccountProgram::get_pid();
	const Variant SYSTEM_PROGRAM_PID = SystemProgram::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(METADATA_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(TOKEN_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(ASSOCIATED_TOKEN_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PROGRAM_PID, false, false)));
	const Variant SYSVAR_INSTRUCTIONS = Pubkey::new_from_string("Sysvar1nstructions1111111111111111111111111");
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_INSTRUCTIONS, false, false)));
	const Variant SYSVAR_SLOTHASHES = Pubkey::new_from_string("SysvarS1otHashes111111111111111111111111111");
	result->append_meta(*memnew_custom(AccountMeta(SYSVAR_SLOTHASHES, false, false)));

	result->append_meta(*memnew_custom(AccountMeta(CANDYMACHINE_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(CANDYMACHINE_PID, false, false)));

	return result;
}

Variant MplCandyMachine::add_config_lines(
		const Variant &candy_machine_key,
		const Variant &authority,
		const Array &config_lines,
		const unsigned int index) {
	Instruction *result = memnew_custom(Instruction);

	PackedByteArray data = add_config_lines_discriminator();
	PackedByteArray temp_array;
	temp_array.resize(4);
	temp_array.encode_u32(0, index);
	data.append_array(temp_array);
	temp_array.encode_u32(0, config_lines.size());
	data.append_array(temp_array);
	for (unsigned int i = 0; i < config_lines.size(); i++) {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(config_lines[i].get_type() != Variant::OBJECT, nullptr, "Element is not ConfigLine");
		Object *obj = config_lines[i];
		ERR_FAIL_COND_V_EDMSG_CUSTOM(obj->get_class() != "ConfigLine", nullptr, "Element is not ConfigLine");

		data.append_array(Object::cast_to<ConfigLine>(config_lines[i])->serialize());
	}

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	const Variant candy_machine_creator = new_candy_machine_authority_pda(candy_machine_key);

	result->append_meta(*memnew_custom(AccountMeta(candy_machine_key, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(authority, true, false)));

	return result;
}

Variant MplCandyMachine::new_candy_machine_authority_pda(const Variant &candy_machine_key) {
	Array seeds;
	seeds.append(String("candy_machine").to_ascii_buffer());
	seeds.append(Pubkey::bytes_from_variant(candy_machine_key));

	return Pubkey::new_pda_bytes(seeds, get_pid());
}

void MplCandyMachine::get_candy_machine_info(const Variant &candy_machine_key) {
	const Callable callback(this, "fetch_account_callback");
	connect("http_request_completed", callback, ConnectFlags::CONNECT_ONE_SHOT);
	get_account_info(Pubkey::string_from_variant(candy_machine_key));
}

void MplCandyMachine::fetch_account_callback(const Error error, const Dictionary &params) {
	if (error != Error::OK || !params.has("result")) {
		UtilityFunctions::print_verbose(VERBOSE_LOG_PREFIX, vformat("Fetching account failed with error code (%d) and rpc result (%s)", error, params));
		return;
	}

	Dictionary result_dict = params["result"];
	Variant value;
	if (!result_dict.has("value")) {
		return;
	}
	value = result_dict["value"];
	if (value.get_type() != Variant::DICTIONARY) {
		return;
	}

	Dictionary account = value;

	Array account_data_tuple = account["data"];
	const String encoded_data = account_data_tuple[0];

	PackedByteArray account_data = SolanaUtils::bs64_decode(encoded_data);
	CandyMachineData *res = memnew_custom(CandyMachineData);

	const int64_t BASE_DATA_LENGTH = 9;
	int64_t cursor = BASE_DATA_LENGTH;
	res->set_token_standard(account_data[cursor]);
	cursor++;
	PackedByteArray features;

	const uint8_t AMOUNT_FEATURES = 6;
	for (int i = 0; i < AMOUNT_FEATURES; i++) {
		features.append(account_data[cursor]);
		cursor++;
	}
	res->set_features(features);

	res->set_authority(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + PUBKEY_LENGTH)));
	cursor += PUBKEY_LENGTH;
	res->set_mint_authority(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + PUBKEY_LENGTH)));
	cursor += PUBKEY_LENGTH;
	res->set_collection_mint(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + PUBKEY_LENGTH)));
	cursor += PUBKEY_LENGTH;

	res->set_items_redeemed(account_data.decode_u64(cursor));
	cursor += sizeof(uint64_t);
	res->set_items_available(account_data.decode_u64(cursor));
	cursor += sizeof(uint64_t);

	const int64_t name_length = account_data.decode_u32(cursor);
	cursor += sizeof(uint32_t);
	res->set_symbol(account_data.slice(cursor, cursor + name_length).get_string_from_ascii());
	cursor += name_length;

	res->set_seller_fee_basis_points(account_data.decode_u16(cursor));
	cursor += sizeof(uint16_t);
	res->set_max_supply(account_data.decode_u64(cursor));
	cursor += sizeof(uint64_t);
	res->set_is_mutable(account_data[cursor] == 1);
	cursor++;
	const uint32_t creators_length = account_data.decode_u32(cursor);
	cursor += 4;
	Array read_creators;
	for (int i = 0; i < creators_length; i++) {
		MetaDataCreator *creator = memnew_custom(MetaDataCreator);
		creator->set_address(Pubkey::new_from_bytes(account_data.slice(cursor, cursor + PUBKEY_LENGTH)));
		cursor += PUBKEY_LENGTH;
		creator->set_verified(account_data[cursor] == 1);
		cursor++;
		creator->set_share(account_data[cursor]);
		cursor += 1;
		read_creators.append(creator);
	}

	res->set_creators(read_creators);

	if (account_data[cursor] != 1) {
		emit_signal("account_fetched", res);
	} else {
		cursor++;

		const int64_t prefix_length = account_data.decode_u32(cursor);

		const Variant config_line_setting = memnew_custom(ConfigLineSetting);
		auto *config_line_setting_ptr = Object::cast_to<ConfigLineSetting>(config_line_setting);

		cursor += 4;
		config_line_setting_ptr->set_prefix_name(account_data.slice(cursor, cursor + prefix_length).get_string_from_ascii());

		cursor += prefix_length;

		config_line_setting_ptr->set_name_length(account_data.decode_u32(cursor));
		cursor += 4;

		const int64_t prefix_uri_length = account_data.decode_u32(cursor);
		cursor += 4;
		config_line_setting_ptr->set_prefix_uri(account_data.slice(cursor, cursor + prefix_uri_length).get_string_from_ascii());
		cursor += prefix_uri_length;

		config_line_setting_ptr->set_uri_length(account_data.decode_u32(cursor));
		cursor += 4;

		config_line_setting_ptr->set_is_sequential(account_data[cursor] == 1);

		res->set_config_line_setting(config_line_setting);

		emit_signal("account_fetched", res);
	}
}

Variant MplCandyMachine::get_pid() {
	return Pubkey::new_from_string(PID.c_str());
}
} //namespace godot
