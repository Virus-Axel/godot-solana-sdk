#include "candy_machine_core/candy_guard_core.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/method_bind.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "anchor/generated/candy_guard.hpp"
#include "anchor/generic_anchor_node.hpp"
#include "anchor/generic_anchor_resource.hpp"
#include "anchor/idl_utils.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"
#include "spl_token.hpp"
#include "instructions/mpl_token_metadata.hpp"
#include "anchor/anchor_program.hpp"
#include "instructions/associated_token_account.hpp"

namespace godot {

std::string CandyGuardCore::PID;

void CandyGuardCore::bind_mint_methods(const StringName &class_name) {
	MethodBind *serialize_core_mint_bind = create_method_bind(&CandyGuardCore::serialize_core_mint_args);
	bind_resource_method(class_name, D_METHOD("serialize_core_mint_args"), serialize_core_mint_bind);

	MethodBind *get_extra_account_metas_bind = create_method_bind(&CandyGuardCore::get_extra_account_metas);
	bind_resource_method(class_name, D_METHOD("get_extra_account_metas", "owner"), get_extra_account_metas_bind);
}

void CandyGuardCore::bind_guard_methods(){
	MethodBind *nft_payment_bind = create_method_bind(&CandyGuardCore::get_extra_accounts_nft_payment);
	bind_resource_method("NftPayment", D_METHOD("get_extra_account_metas", "owner", "payment_mint", "pnft"), nft_payment_bind);
}

Variant CandyGuardCore::get_pid(){
	return Pubkey::new_from_string(String(CandyGuardCore::PID.c_str()));
}

void CandyGuardCore::register_from_idl() {
	const Dictionary content = JSON::parse_string(String(candy_guard_idl.c_str()));
	GenericAnchorNode::bind_anchor_node<GenericAnchorNode, CandyGuardCore>(content);
	const Variant idl_address = AnchorProgram::get_address_from_idl(content);

	ERR_FAIL_COND_CUSTOM(idl_address.get_type() != Variant::OBJECT);
	PID = Object::cast_to<Pubkey>(idl_address)->to_string().ascii();

	bind_mint_methods("CandyGuardData");
	bind_mint_methods("GuardSet");
	bind_guard_methods();
}

Array CandyGuardCore::get_extra_account_metas(const Variant &owner) {
	TypedArray<AccountMeta> result;

	if (is_property_enabled("enable_thirdPartySigner")) {
		result.append(memnew_custom(AccountMeta(properties["thirdPartySigner"].value.get("signerKey"), true, true)));
	}
	if (is_property_enabled("enable_gatekeeper")) {
		result.append(memnew_custom(AccountMeta(properties["gatekeeper"].value.get("gatekeeperNetwork"), false, false)));
	}
	if (is_property_enabled("enable_nftPayment")) {
		const Variant sender_ata = Pubkey::new_associated_token_address(owner, properties["nftPayment"].value.get("mint"), TokenProgram::get_pid());
		result.append(memnew_custom(AccountMeta(properties["nftPayment"].value.get("destination"), false, true)));
	}
	if (is_property_enabled("enable_nftGate")) {
		result.append(memnew_custom(AccountMeta(properties["nftGate"].value.get("requiredCollection"), false, false)));
	}
	if (is_property_enabled("enable_nftBurn")) {
		result.append(memnew_custom(AccountMeta(properties["nftBurn"].value.get("requiredCollection"), false, true)));
	}
	if (is_property_enabled("enable_solPayment")) {
		result.append(memnew_custom(AccountMeta(properties["solPayment"].value.get("destination"), false, true)));
	}
	if (is_property_enabled("enable_nftMintLimit")) {
		// TODO(VIRAX): Find the correct key here
		//const Variant limit_authority = MplCandyGuard::new_limit_counter_pda(static_cast<uint8_t>(limit_id), payer, machine_key, guard_key);
		result.append(memnew_custom(AccountMeta(properties["nftMintLimit"].value.get("requiredCollection"), false, true)));
	}
	if (is_property_enabled("enable_tokenPayment")) {
		const Variant sender_ata = Pubkey::new_associated_token_address(owner, properties["tokenPayment"].value.get("mint"), TokenProgram::get_pid());
		result.append(memnew_custom(AccountMeta(sender_ata, false, true)));
		result.append(memnew_custom(AccountMeta(properties["tokenPayment"].value.get("destinationAta"), false, true)));
		result.append(AccountMeta::new_account_meta(TokenProgram::get_pid(), false, false));
	}
	if (is_property_enabled("enable_tokenGate")) {
		// TODO(VirAx): Different from old guard. Verify this
		result.append(memnew_custom(AccountMeta(properties["tokenGate"].value.get("mint"), false, true)));
	}
	if (is_property_enabled("enable_tokenBurn")) {
		const Variant sender_ata = Pubkey::new_associated_token_address(owner, properties["tokenBurn"].value.get("mint"), TokenProgram::get_pid());
		result.append(memnew_custom(AccountMeta(sender_ata, false, true)));
		result.append(memnew_custom(AccountMeta(properties["tokenBurn"].value.get("mint"), false, true)));
		result.append(AccountMeta::new_account_meta(TokenProgram::get_pid(), false, false));
	}
	if (is_property_enabled("enable_freezeSolPayment")) {
		result.append(memnew_custom(AccountMeta(properties["freezeSolPayment"].value.get("destination"), false, true)));
	}
	if (is_property_enabled("enable_freezeTokenPayment")) {
		result.append(memnew_custom(AccountMeta(properties["freezeTokenPayment"].value.get("mint"), false, false)));
		result.append(memnew_custom(AccountMeta(properties["freezeTokenPayment"].value.get("destinationAta"), false, true)));
	}
	if (is_property_enabled("enable_programGate")) {
		for (unsigned int i = 0; i < static_cast<Array>(properties["programGate"].value.get("additional")).size(); i++) {
			result.append(memnew_custom(AccountMeta(static_cast<Array>(properties["programGate"].value.get("additional"))[i], false, false)));
		}
	}

	return result;
}

Array CandyGuardCore::get_extra_accounts_nft_payment(const Variant &owner, const Variant &payment_mint, bool pnft, const Variant &creator) {
	Array result;
	const Variant payment_token_account = Pubkey::new_associated_token_address(owner, payment_mint, TokenProgram::get_pid());
	result.append(memnew(AccountMeta(payment_token_account, false, true)));
	result.append(AccountMeta::new_account_meta(MplTokenMetadata::new_associated_metadata_pubkey(payment_mint), false, true));
	result.append(AccountMeta::new_account_meta(payment_mint, false, false));
	result.append(AccountMeta::new_account_meta(properties["destination"].value, false, false));
	//result.append(AccountMeta::new_account_meta(Pubkey::new_associated_token_address(properties["destination"].value, payment_mint, TokenProgram::get_pid()), false, false));
	const Variant destination_ata = Pubkey::new_associated_token_address(properties["destination"].value, payment_mint, TokenProgram::get_pid());
	result.append(AccountMeta::new_account_meta(destination_ata, false, true));
	result.append(AccountMeta::new_account_meta(AssociatedTokenAccountProgram::get_pid(), false, false));
	result.append(AccountMeta::new_account_meta(TokenProgram::get_pid(), false, false));
	result.append(AccountMeta::new_account_meta(MplTokenMetadata::get_pid(), false, false));

	if (pnft) {
		TokenProgram::new_token_record_address(payment_mint, owner);
		result.append(AccountMeta::new_account_meta(MplTokenMetadata::new_associated_metadata_pubkey_master_edition(payment_mint), false, false));
		result.append(AccountMeta::new_account_meta(TokenProgram::new_token_record_address(payment_token_account, payment_mint), false, true));
		result.append(AccountMeta::new_account_meta(TokenProgram::new_token_record_address(destination_ata, payment_mint), false, true));
		result.append(AccountMeta::new_account_meta(Pubkey::new_from_string("auth9SigNpDKz4sJJ1DfCTuZrZNSAgh9sFD3rboVmgg"), false, false));
		const Variant rule_set_account = Pubkey::new_pda_bytes(Array::make(String("rule_set").to_ascii_buffer(), Pubkey::bytes_from_variant(creator)), MplTokenMetadata::get_pid());
		result.append(AccountMeta::new_account_meta(rule_set_account, false, false));
	}
	return result;
}

PackedByteArray CandyGuardCore::serialize_core_mint_args_group(const Variant &group) {
	PackedByteArray result;
	ERR_FAIL_COND_V_EDMSG_CUSTOM(group.get_type() != Variant::OBJECT, PackedByteArray(), "Group is not an object.");
	ERR_FAIL_COND_V_EDMSG_CUSTOM(!static_cast<Object *>(group)->is_class("Group"), PackedByteArray(), "Group is not a Group resource.");

	auto *class_ptr = Object::cast_to<CandyGuardCore>(group);

	PackedByteArray fix_size_label = String(class_ptr->properties["label"].value).to_ascii_buffer();

	const int MAX_LABEL_SIZE = 6;
	fix_size_label.resize(MAX_LABEL_SIZE);

	result.append_array(fix_size_label);
	result.append_array(Object::cast_to<CandyGuardCore>(class_ptr->properties["guards"].value)->serialize_core_mint_args());

	return result;
}

// TODO(VirAx): Refactor and reuse code.
PackedByteArray CandyGuardCore::serialize_core_mint_args() { // NOLINT(readability-function-cognitive-complexity)
	PackedByteArray result;

	if (get_local_name() == "CandyGuardData") {
		result.append_array(Object::cast_to<CandyGuardCore>(properties["default"].value)->serialize_core_mint_args());

		PackedByteArray group_size_stream;
		group_size_stream.resize(4);

		if (properties["enable_groups"].value) {
			const Array &groups = properties["groups"].value;
			const uint32_t group_size = groups.size();
			group_size_stream.encode_u32(0, group_size);
			result.append_array(group_size_stream);

			for (unsigned int i = 0; i < groups.size(); i++) {
				const Variant group = groups[i];
				result.append_array(serialize_core_mint_args_group(group));
			}
		} else {
			result.append_array(group_size_stream);
		}

		return result;
	}

	uint8_t counter = 0;
	uint64_t enabled_map = 0;

	result.resize(sizeof(enabled_map));

	for (const auto &property : properties) {
		if (property.first == StringName("enable_groups")) {
			continue;
		}
		if (property.first.begins_with("enable_")) {
			std::vector<StringName> guard_names = property_order[get_local_name()];
			if (property.second.value) {
				uint8_t index = 0;
				for (size_t i = 0; i < guard_names.size() / 2; i++) {
					if (guard_names[i * 2U] == property.first) {
						index = i;
						break;
					}
				}
				enabled_map += (1ULL << index);
			}
			++counter;
		} else if (property.second.enabled) {
			result.append_array(IdlUtils::serialize_variant(property.second.value));
		}
	}

	result.encode_u64(0, static_cast<int64_t>(enabled_map));

	return result;
}

} //namespace godot