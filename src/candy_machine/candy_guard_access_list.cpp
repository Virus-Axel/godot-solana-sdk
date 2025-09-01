#include "candy_machine/candy_guard_access_list.hpp"

#include "godot_cpp/variant/utility_functions.hpp"

#include "candy_machine/candy_guard.hpp"
#include "instruction.hpp"
#include "instructions/associated_token_account.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "instructions/spl_token.hpp"
#include "instructions/system_program.hpp"
#include "solana_utils.hpp"

namespace godot {
void CandyGuardAccessList::_bind_methods() {
}

String CandyGuardAccessList::get_label() const {
	return label;
}

PackedByteArray CandyGuardAccessList::serialize() const {
	PackedByteArray result;
	PackedByteArray result_data;
	result_data.append_array(serialize_guard_settings());
	result_data.append_array(serialize_groups());

	result.resize(4);
	result.encode_u32(0, result_data.size());
	result.append_array(result_data);

	return result;
}

PackedByteArray CandyGuardAccessList::serialize_groups() const {
	PackedByteArray result;

	result.resize(4);
	result.encode_u32(0, groups.size());

	for (unsigned int i = 0; i < groups.size(); i++) {
		CandyGuardAccessList *group = Object::cast_to<CandyGuardAccessList>(groups[i]);
		PackedByteArray fix_size_label = group->get_label().to_ascii_buffer();

		const int MAX_LABEL_SIZE = 6;
		fix_size_label.resize(MAX_LABEL_SIZE);

		result.append_array(fix_size_label);
		result.append_array(group->serialize_guard_settings());
	}

	return result;
}

PackedByteArray CandyGuardAccessList::serialize_guard_settings() const {
	PackedByteArray result;
	result.resize(8);
	uint64_t enabled_guards = 0;

	if (enable_bot_tax) {
		enabled_guards += 1;
		PackedByteArray bot_tax_bytes;
		bot_tax_bytes.resize(9);
		bot_tax_bytes.encode_u64(0, bot_tax_lamports);
		bot_tax_bytes[8] = (int)validate_last_instruction;
		result.append_array(bot_tax_bytes);
	}
	if (enable_sol_payment) {
		enabled_guards += 2;
		PackedByteArray sol_payment_bytes;
		sol_payment_bytes.resize(8);
		sol_payment_bytes.encode_u64(0, sol_payment_lamports);
		result.append_array(sol_payment_bytes);
		result.append_array(Pubkey::bytes_from_variant(sol_payment_destination));
	}
	if (enable_token_payment) {
		enabled_guards += 4;
		PackedByteArray token_payment_bytes;
		token_payment_bytes.resize(8);
		token_payment_bytes.encode_u64(0, token_payment_amount);
		result.append_array(token_payment_bytes);
		result.append_array(Pubkey::bytes_from_variant(token_payment_mint));
		result.append_array(Pubkey::bytes_from_variant(token_payment_destination));
	}
	if (enable_start_date) {
		enabled_guards += 8;
		PackedByteArray start_date_bytes;
		start_date_bytes.resize(8);
		start_date_bytes.encode_u64(0, start_date);
		result.append_array(start_date_bytes);
	}
	if (enable_third_party_signer) {
		enabled_guards += 16;
		result.append_array(Pubkey::bytes_from_variant(signer_key));
	}
	if (enable_token_gate) {
		enabled_guards += 32;
		PackedByteArray token_gate_bytes;
		token_gate_bytes.resize(8);
		token_gate_bytes.encode_u64(0, token_gate_amount);
		result.append_array(token_gate_bytes);
		result.append_array(Pubkey::bytes_from_variant(token_gate_mint));
	}
	if (enable_gatekeeper) {
		enabled_guards += 64;
		result.append_array(Pubkey::bytes_from_variant(gatekeeper_network));
		result.append((int)expire_on_use);
	}
	if (enable_end_date) {
		enabled_guards += 128;
		PackedByteArray end_date_bytes;
		end_date_bytes.resize(8);
		end_date_bytes.encode_s64(0, token_gate_amount);
		result.append_array(end_date_bytes);
	}
	if (enable_allow_list) {
		enabled_guards += 256;
		result.append_array(allowed_merkle_root);
	}
	if (enable_mint_limit) {
		enabled_guards += 512;
		PackedByteArray mint_limit_bytes;
		mint_limit_bytes.resize(3);
		mint_limit_bytes[0] = limit_id;
		mint_limit_bytes.encode_u16(1, mint_limit);
		result.append_array(mint_limit_bytes);
	}
	if (enable_nft_payment) {
		enabled_guards += 1024;
		result.append_array(Pubkey::bytes_from_variant(required_nft_payment_collection));
		result.append_array(Pubkey::bytes_from_variant(nft_payment_destination));
	}
	if (enable_redeem_account) {
		enabled_guards += 2048;
		PackedByteArray redeem_bytes;
		redeem_bytes.resize(8);
		redeem_bytes.encode_u64(0, max_redeem_amount);
		result.append_array(redeem_bytes);
	}
	if (enable_address_gate) {
		enabled_guards += 4096;
		result.append_array(Pubkey::bytes_from_variant(address_gate_reciever));
	}
	if (enable_nft_gate) {
		enabled_guards += 8192;
		result.append_array(Pubkey::bytes_from_variant(required_nft_gate_collection));
	}
	if (enable_nft_burn) {
		enabled_guards += 16384;
		result.append_array(Pubkey::bytes_from_variant(required_nft_burn_collection));
	}
	if (enable_token_burn) {
		enabled_guards += 32768;
		PackedByteArray token_burn_bytes;
		token_burn_bytes.resize(8);
		token_burn_bytes.encode_u64(0, max_redeem_amount);
		result.append_array(token_burn_bytes);
		result.append_array(Pubkey::bytes_from_variant(token_burn_mint));
	}
	if (enable_freeze_sol_payment) {
		enabled_guards += 65536;
		PackedByteArray freeze_sol_bytes;
		freeze_sol_bytes.resize(8);
		freeze_sol_bytes.encode_u64(0, freeze_amount_lamports);
		result.append_array(freeze_sol_bytes);
		result.append_array(Pubkey::bytes_from_variant(freeze_sol_destination));
	}
	if (enable_freeze_token_payment) {
		enabled_guards += 65536;
		PackedByteArray freeze_token_bytes;
		freeze_token_bytes.resize(8);
		freeze_token_bytes.encode_u64(0, amount);
		result.append_array(freeze_token_bytes);
		result.append_array(Pubkey::bytes_from_variant(mint));
		result.append_array(Pubkey::bytes_from_variant(freeze_token_destination_ata));
	}
	if (enable_program_gate) {
		enabled_guards += 131072;
		PackedByteArray array_length_bytes;
		array_length_bytes.resize(8);
		array_length_bytes.encode_u64(0, amount);
		result.append_array(array_length_bytes);
		for (unsigned int i = 0; i < program_gate_addresses.size(); i++) {
			result.append_array(Pubkey::bytes_from_variant(program_gate_addresses[i]));
		}
	}
	if (enable_allocation) {
		enabled_guards += 262144;
		PackedByteArray allocation_bytes;
		allocation_bytes.resize(5);
		allocation_bytes.encode_u8(0, allocation_id);
		allocation_bytes.encode_u32(1, allocation_size);
		result.append_array(allocation_bytes);
	}

	result.encode_u64(0, enabled_guards);

	return result;
}

CandyGuardAccessList &CandyGuardAccessList::get_group(const String &label) {
	if (label == "default")
		return *this;

	for (unsigned int i = 0; i < groups.size(); i++) {
		if (groups[i].get_type() != Variant::OBJECT) {
			continue;
		} else if (Object::cast_to<CandyGuardAccessList>(groups[i])->label == label) {
			return *Object::cast_to<CandyGuardAccessList>(groups[i]);
		}
	}
	internal::gdextension_interface_print_warning("Nonexisting group", "get_group", __FILE__, __LINE__, true);

	return *this;
}

TypedArray<AccountMeta> CandyGuardAccessList::get_mint_arg_accounts(const Variant &payer, const Variant &machine_key, const Variant &guard_key) {
	TypedArray<AccountMeta> result;

	if (enable_third_party_signer) {
		result.append(memnew(AccountMeta(signer_key, true, true)));
	}
	if (enable_gatekeeper) {
		result.append(memnew(AccountMeta(gatekeeper_network, false, false)));
	}
	if (enable_nft_payment) {
		result.append(memnew(AccountMeta(nft_payment_destination, false, true)));
	}
	if (enable_nft_gate) {
		result.append(memnew(AccountMeta(required_nft_gate_collection, false, false)));
	}
	if (enable_nft_burn) {
		result.append(memnew(AccountMeta(required_nft_burn_collection, false, true)));
	}
	if (enable_sol_payment) {
		result.append(memnew(AccountMeta(sol_payment_destination, false, true)));
	}
	if (enable_mint_limit) {
		const Variant limit_authority = MplCandyGuard::new_limit_counter_pda(static_cast<uint8_t>(limit_id), payer, machine_key, guard_key);
		result.append(memnew(AccountMeta(limit_authority, false, true)));
	}
	if (enable_token_payment) {
		const Variant token_payment_ata = Pubkey::new_associated_token_address(payer, token_payment_mint, TokenProgram::get_pid());
		result.append(memnew(AccountMeta(token_payment_ata, false, true)));
		result.append(memnew(AccountMeta(token_payment_destination, false, true)));
	}
	if (enable_token_gate) {
		const Variant token_gate_ata = Pubkey::new_associated_token_address(payer, token_gate_mint, TokenProgram::get_pid());
		result.append(memnew(AccountMeta(token_gate_ata, false, true)));
	}
	if (enable_token_burn) {
		const Variant token_burn_ata = Pubkey::new_associated_token_address(payer, token_burn_mint, TokenProgram::get_pid());
		result.append(memnew(AccountMeta(token_burn_ata, false, true)));
		result.append(memnew(AccountMeta(token_burn_mint, false, true)));
	}
	if (enable_freeze_sol_payment) {
		result.append(memnew(AccountMeta(freeze_sol_destination, false, true)));
	}
	if (enable_freeze_token_payment) {
		result.append(memnew(AccountMeta(mint, false, false)));
		result.append(memnew(AccountMeta(freeze_token_destination_ata, false, true)));
	}
	if (enable_program_gate) {
		for (unsigned int i = 0; i < program_gate_addresses.size(); i++) {
			result.append(memnew(AccountMeta(program_gate_addresses, false, false)));
		}
	}

	return result;
}

PackedByteArray CandyGuardAccessList::serialize_mint_settings() const {
	PackedByteArray result;
	result.resize(4);

	if (enable_gatekeeper) {
		result.append((int)expire_on_use);
	}
	if (enable_allow_list) {
		result.append_array(allowed_merkle_root);
	}
	if (enable_mint_limit) {
		result.append(limit_id);
	}
	if (enable_sol_payment) {
		result.append_array(Pubkey::bytes_from_variant(sol_payment_destination));
	}

	// insert mint setting size
	result.encode_u32(0, result.size() - 4);

	return result;
}

bool CandyGuardAccessList::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;

	if (name == "enable_address_gate") {
		notify_property_list_changed();
		enable_address_gate = p_value;
		return true;
	} else if (name == "address_gate_reciever") {
		address_gate_reciever = p_value;
		return true;
	} else if (name == "enable_allocation") {
		notify_property_list_changed();
		enable_allocation = p_value;
		return true;
	} else if (name == "allocation_id") {
		allocation_id = p_value;
		return true;
	} else if (name == "allocation_size") {
		allocation_size = p_value;
		return true;
	} else if (name == "enable_allow_list") {
		notify_property_list_changed();
		enable_allow_list = p_value;
		return true;
	} else if (name == "allowed_merkle_root") {
		allowed_merkle_root = p_value;
		return true;
	} else if (name == "enable_bot_tax") {
		notify_property_list_changed();
		enable_bot_tax = p_value;
		return true;
	} else if (name == "bot_tax_lamports") {
		bot_tax_lamports = p_value;
		return true;
	} else if (name == "validate_last_instruction") {
		validate_last_instruction = p_value;
		return true;
	} else if (name == "enable_end_date") {
		notify_property_list_changed();
		enable_end_date = p_value;
		return true;
	} else if (name == "end_date") {
		end_date = p_value;
		return true;
	} else if (name == "enable_freeze_sol_payment") {
		notify_property_list_changed();
		enable_freeze_sol_payment = p_value;
		return true;
	} else if (name == "freeze_amount_lamports") {
		freeze_amount_lamports = p_value;
		return true;
	} else if (name == "freeze_sol_destination") {
		freeze_sol_destination = p_value;
		return true;
	} else if (name == "enable_freeze_token_payment") {
		notify_property_list_changed();
		enable_freeze_token_payment = p_value;
		return true;
	} else if (name == "amount") {
		amount = p_value;
		return true;
	} else if (name == "mint") {
		mint = p_value;
		return true;
	} else if (name == "freeze_token_destination_ata") {
		freeze_token_destination_ata = p_value;
		return true;
	} else if (name == "enable_gatekeeper") {
		notify_property_list_changed();
		enable_gatekeeper = p_value;
		return true;
	} else if (name == "gatekeeper_network") {
		gatekeeper_network = p_value;
		return true;
	} else if (name == "expire_on_use") {
		expire_on_use = p_value;
		return true;
	} else if (name == "enable_mint_limit") {
		notify_property_list_changed();
		enable_mint_limit = p_value;
		return true;
	} else if (name == "limit_id") {
		limit_id = p_value;
		return true;
	} else if (name == "mint_limit") {
		mint_limit = p_value;
		return true;
	} else if (name == "enable_nft_burn") {
		notify_property_list_changed();
		enable_nft_burn = p_value;
		return true;
	} else if (name == "required_nft_burn_collection") {
		required_nft_burn_collection = p_value;
		return true;
	} else if (name == "enable_nft_gate") {
		notify_property_list_changed();
		enable_nft_gate = p_value;
		return true;
	} else if (name == "required_nft_gate_collection") {
		required_nft_gate_collection = p_value;
		return true;
	} else if (name == "enable_nft_payment") {
		notify_property_list_changed();
		enable_nft_payment = p_value;
		return true;
	} else if (name == "required_nft_payment_collection") {
		required_nft_payment_collection = p_value;
		return true;
	} else if (name == "nft_payment_destination") {
		nft_payment_destination = p_value;
		return true;
	} else if (name == "enable_program_gate") {
		notify_property_list_changed();
		enable_program_gate = p_value;
		return true;
	} else if (name == "program_gate_addresses") {
		program_gate_addresses = p_value;
		return true;
	} else if (name == "enable_redeem_account") {
		notify_property_list_changed();
		enable_redeem_account = p_value;
		return true;
	} else if (name == "max_redeem_amount") {
		max_redeem_amount = p_value;
		return true;
	} else if (name == "enable_sol_payment") {
		notify_property_list_changed();
		enable_sol_payment = p_value;
		return true;
	} else if (name == "sol_payment_lamports") {
		sol_payment_lamports = p_value;
		return true;
	} else if (name == "sol_payment_destination") {
		sol_payment_destination = p_value;
		return true;
	} else if (name == "enable_start_date") {
		notify_property_list_changed();
		enable_start_date = p_value;
		return true;
	} else if (name == "start_date") {
		start_date = p_value;
		return true;
	} else if (name == "enable_third_party_signer") {
		notify_property_list_changed();
		enable_third_party_signer = p_value;
		return true;
	} else if (name == "signer_key") {
		signer_key = p_value;
		return true;
	} else if (name == "enable_token_burn") {
		notify_property_list_changed();
		enable_token_burn = p_value;
		return true;
	} else if (name == "token_burn_amount") {
		token_burn_amount = p_value;
		return true;
	} else if (name == "token_burn_mint") {
		token_burn_mint = p_value;
		return true;
	} else if (name == "enable_token_gate") {
		notify_property_list_changed();
		enable_token_gate = p_value;
		return true;
	} else if (name == "token_gate_amount") {
		token_gate_amount = p_value;
		return true;
	} else if (name == "token_gate_mint") {
		token_gate_mint = p_value;
		return true;
	} else if (name == "enable_token_payment") {
		notify_property_list_changed();
		enable_token_payment = p_value;
		return true;
	} else if (name == "token_payment_amount") {
		token_payment_amount = p_value;
		return true;
	} else if (name == "token_payment_mint") {
		token_payment_mint = p_value;
		return true;
	} else if (name == "token_payment_destination") {
		token_payment_destination = p_value;
		return true;
	} else if (name == "groups") {
		groups = (TypedArray<CandyGuardAccessList>)p_value;
		for (unsigned int i = 0; i < groups.size(); i++) {
			if (groups[i].get_type() == Variant::OBJECT) {
				if (Object::cast_to<CandyGuardAccessList>(groups[i])->top_level) {
					Object::cast_to<CandyGuardAccessList>(groups[i])->top_level = false;
					Object::cast_to<CandyGuardAccessList>(groups[i])->label = "";
				}
			}
		}
		return true;
	} else if (name == "label") {
		label = p_value;
		return true;
	} else if (name == "top_level") {
		top_level = p_value;
		return true;
	}
	return false;
}

bool CandyGuardAccessList::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "enable_address_gate") {
		r_ret = enable_address_gate;
		return true;
	} else if (name == "address_gate_reciever") {
		r_ret = address_gate_reciever;
		return true;
	} else if (name == "enable_allocation") {
		r_ret = enable_allocation;
		return true;
	} else if (name == "allocation_id") {
		r_ret = allocation_id;
		return true;
	} else if (name == "allocation_size") {
		r_ret = allocation_size;
		return true;
	} else if (name == "enable_allow_list") {
		r_ret = enable_allow_list;
		return true;
	} else if (name == "allowed_merkle_root") {
		r_ret = allowed_merkle_root;
		return true;
	} else if (name == "enable_bot_tax") {
		r_ret = enable_bot_tax;
		return true;
	} else if (name == "bot_tax_lamports") {
		r_ret = bot_tax_lamports;
		return true;
	} else if (name == "validate_last_instruction") {
		r_ret = validate_last_instruction;
		return true;
	} else if (name == "enable_end_date") {
		r_ret = enable_end_date;
		return true;
	} else if (name == "end_date") {
		r_ret = end_date;
		return true;
	} else if (name == "enable_freeze_sol_payment") {
		r_ret = enable_freeze_sol_payment;
		return true;
	} else if (name == "freeze_amount_lamports") {
		r_ret = freeze_amount_lamports;
		return true;
	} else if (name == "freeze_sol_destination") {
		r_ret = freeze_sol_destination;
		return true;
	} else if (name == "enable_freeze_token_payment") {
		r_ret = enable_freeze_token_payment;
		return true;
	} else if (name == "amount") {
		r_ret = amount;
		return true;
	} else if (name == "mint") {
		r_ret = mint;
		return true;
	} else if (name == "freeze_token_destination_ata") {
		r_ret = freeze_token_destination_ata;
		return true;
	} else if (name == "enable_gatekeeper") {
		r_ret = enable_gatekeeper;
		return true;
	} else if (name == "gatekeeper_network") {
		r_ret = gatekeeper_network;
		return true;
	} else if (name == "expire_on_use") {
		r_ret = expire_on_use;
		return true;
	} else if (name == "enable_mint_limit") {
		r_ret = enable_mint_limit;
		return true;
	} else if (name == "limit_id") {
		r_ret = limit_id;
		return true;
	} else if (name == "mint_limit") {
		r_ret = mint_limit;
		return true;
	} else if (name == "enable_nft_burn") {
		r_ret = enable_nft_burn;
		return true;
	} else if (name == "required_nft_burn_collection") {
		r_ret = required_nft_burn_collection;
		return true;
	} else if (name == "enable_nft_gate") {
		r_ret = enable_nft_gate;
		return true;
	} else if (name == "required_nft_gate_collection") {
		r_ret = required_nft_gate_collection;
		return true;
	} else if (name == "enable_nft_payment") {
		r_ret = enable_nft_payment;
		return true;
	} else if (name == "required_nft_payment_collection") {
		r_ret = required_nft_payment_collection;
		return true;
	} else if (name == "nft_payment_destination") {
		r_ret = nft_payment_destination;
		return true;
	} else if (name == "enable_program_gate") {
		r_ret = enable_program_gate;
		return true;
	} else if (name == "program_gate_addresses") {
		r_ret = program_gate_addresses;
		return true;
	} else if (name == "enable_redeem_account") {
		r_ret = enable_redeem_account;
		return true;
	} else if (name == "max_redeem_amount") {
		r_ret = max_redeem_amount;
		return true;
	} else if (name == "enable_sol_payment") {
		r_ret = enable_sol_payment;
		return true;
	} else if (name == "sol_payment_lamports") {
		r_ret = sol_payment_lamports;
		return true;
	} else if (name == "sol_payment_destination") {
		r_ret = sol_payment_destination;
		return true;
	} else if (name == "enable_start_date") {
		r_ret = enable_start_date;
		return true;
	} else if (name == "start_date") {
		r_ret = start_date;
		return true;
	} else if (name == "enable_third_party_signer") {
		r_ret = enable_third_party_signer;
		return true;
	} else if (name == "signer_key") {
		r_ret = signer_key;
		return true;
	} else if (name == "enable_token_burn") {
		r_ret = enable_token_burn;
		return true;
	} else if (name == "token_burn_amount") {
		r_ret = token_burn_amount;
		return true;
	} else if (name == "token_burn_mint") {
		r_ret = token_burn_mint;
		return true;
	} else if (name == "enable_token_gate") {
		r_ret = enable_token_gate;
		return true;
	} else if (name == "token_gate_amount") {
		r_ret = token_gate_amount;
		return true;
	} else if (name == "token_gate_mint") {
		r_ret = token_gate_mint;
		return true;
	} else if (name == "enable_token_payment") {
		r_ret = enable_token_payment;
		return true;
	} else if (name == "token_payment_amount") {
		r_ret = token_payment_amount;
		return true;
	} else if (name == "token_payment_mint") {
		r_ret = token_payment_mint;
		return true;
	} else if (name == "token_payment_destination") {
		r_ret = token_payment_destination;
		return true;
	} else if (name == "groups") {
		r_ret = groups;
		return true;
	} else if (name == "label") {
		r_ret = label;
		return true;
	} else if (name == "top_level") {
		r_ret = top_level;
		return true;
	}
	return false;
}

void CandyGuardAccessList::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::BOOL, "top_level", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
	if (top_level) {
		p_list->push_back(PropertyInfo(Variant::STRING, "label", PROPERTY_HINT_TYPE_STRING, "default", PROPERTY_USAGE_READ_ONLY + PROPERTY_USAGE_DEFAULT));
	} else {
		p_list->push_back(PropertyInfo(Variant::STRING, "label"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_address_gate"));
	if (enable_address_gate) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "address_gate_reciever", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_allocation"));
	if (enable_allocation) {
		p_list->push_back(PropertyInfo(Variant::INT, "allocation_id", PROPERTY_HINT_RANGE, "0,255"));
		p_list->push_back(PropertyInfo(Variant::INT, "allocation_size", PROPERTY_HINT_RANGE, "0,65535"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_allow_list"));
	if (enable_allow_list) {
		p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "allowed_merkle_root"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_bot_tax"));
	if (enable_bot_tax) {
		p_list->push_back(PropertyInfo(Variant::INT, "bot_tax_lamports"));
		p_list->push_back(PropertyInfo(Variant::BOOL, "validate_last_instruction"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_end_date"));
	if (enable_end_date) {
		p_list->push_back(PropertyInfo(Variant::INT, "end_date"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_freeze_sol_payment"));
	if (enable_freeze_sol_payment) {
		p_list->push_back(PropertyInfo(Variant::INT, "freeze_amount_lamports"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "freeze_sol_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_freeze_token_payment"));
	if (enable_freeze_token_payment) {
		p_list->push_back(PropertyInfo(Variant::INT, "amount"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "freeze_token_destination_ata", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_gatekeeper"));
	if (enable_gatekeeper) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "gatekeeper_network", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
		p_list->push_back(PropertyInfo(Variant::BOOL, "expire_on_use"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_mint_limit"));
	if (enable_mint_limit) {
		p_list->push_back(PropertyInfo(Variant::INT, "limit_id", PROPERTY_HINT_RANGE, "0,255"));
		p_list->push_back(PropertyInfo(Variant::INT, "mint_limit", PROPERTY_HINT_RANGE, "0,65535"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_burn"));
	if (enable_nft_burn) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_burn_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_gate"));
	if (enable_nft_gate) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_gate_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_nft_payment"));
	if (enable_nft_payment) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "required_nft_payment_collection", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "nft_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_program_gate"));
	if (enable_program_gate) {
		p_list->push_back(PropertyInfo(Variant::ARRAY, "program_gate_addresses"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_redeem_account"));
	if (enable_redeem_account) {
		p_list->push_back(PropertyInfo(Variant::INT, "max_redeem_amount"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_sol_payment"));
	if (enable_sol_payment) {
		p_list->push_back(PropertyInfo(Variant::INT, "sol_payment_lamports"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "sol_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_start_date"));
	if (enable_start_date) {
		p_list->push_back(PropertyInfo(Variant::INT, "start_date"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_third_party_signer"));
	if (enable_third_party_signer) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "signer_key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_burn"));
	if (enable_token_burn) {
		p_list->push_back(PropertyInfo(Variant::INT, "token_burn_amount"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "token_burn_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_gate"));
	if (enable_token_gate) {
		p_list->push_back(PropertyInfo(Variant::INT, "token_gate_amount"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "token_gate_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}

	p_list->push_back(PropertyInfo(Variant::BOOL, "enable_token_payment"));
	if (enable_token_payment) {
		p_list->push_back(PropertyInfo(Variant::INT, "token_payment_amount"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "token_payment_mint", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
		p_list->push_back(PropertyInfo(Variant::OBJECT, "token_payment_destination", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
	}
	if (top_level) {
		p_list->push_back(PropertyInfo(Variant::ARRAY, "groups", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("CandyGuardAccessList")));
	} else {
		p_list->push_back(PropertyInfo(Variant::ARRAY, "groups", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("CandyGuardAccessList"), PROPERTY_USAGE_STORAGE));
	}
}
} //namespace godot