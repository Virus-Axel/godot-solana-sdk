#include "merged_account_metas.hpp"

#include <cstdint>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

namespace godot {

Variant MergedAccountMetas::preferred_signer(const Variant &left, const Variant &right) {
	Object *left_object = nullptr;
	Object *right_object = nullptr;

	const Variant left_pubkey = Pubkey::new_from_variant(left);
	if (left.get_type() == Variant::OBJECT) {
		left_object = static_cast<Object *>(left);
	} else {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(left_pubkey.get_type() != Variant::OBJECT, nullptr, "Invalid signer input type.");
		left_object = static_cast<Object *>(left_pubkey);
	}
	const Variant right_pubkey = Pubkey::new_from_variant(right);
	if (right.get_type() == Variant::OBJECT) {
		right_object = static_cast<Object *>(right);
	} else {
		ERR_FAIL_COND_V_EDMSG_CUSTOM(right_pubkey.get_type() != Variant::OBJECT, nullptr, "Invalid signer input type.");
		right_object = static_cast<Object *>(right_pubkey);
	}

	// Keypair is prefferred over WalletAdapter since they can instantly sign.
	if (Keypair::is_compatible_type(left)) {
		return left;
	}
	if (Keypair::is_compatible_type(right)) {
		return right;
	}
	if (left_object->is_class("WalletAdapter")) {
		return left;
	}
	if (right_object->is_class("WalletAdapter")) {
		return right;
	}

	return left;
}

void MergedAccountMetas::merge_at(const Variant &account_meta, int64_t index) {
	ERR_FAIL_COND_EDMSG_CUSTOM(account_meta.get_type() != Variant::OBJECT, "FAIL");
	ERR_FAIL_COND_EDMSG_CUSTOM(static_cast<Object *>(account_meta)->get_class() != "AccountMeta", "FAIL");
	auto *existing_meta = Object::cast_to<AccountMeta>(merged_metas[index]);
	const Variant left = existing_meta->get_key();
	const Variant right = Object::cast_to<AccountMeta>(account_meta)->get_key();
	const Variant selected_signer = preferred_signer(left, right);
	ERR_FAIL_COND_EDMSG_CUSTOM(selected_signer.get_type() == Variant::NIL, "Preferred signer could not be determined");

	existing_meta->set_key(selected_signer);
	existing_meta->set_is_signer(existing_meta->get_is_signer() || Object::cast_to<AccountMeta>(account_meta)->get_is_signer());
	existing_meta->set_writeable(existing_meta->get_writeable() || Object::cast_to<AccountMeta>(account_meta)->get_writeable());
}

void MergedAccountMetas::clear() {
	merged_metas.clear();
}

void MergedAccountMetas::add(const Variant &account_meta) {
	ERR_FAIL_COND_EDMSG_CUSTOM(account_meta.get_type() != Variant::OBJECT, "Provided account meta is not an object.");
	ERR_FAIL_COND_EDMSG_CUSTOM(static_cast<Object *>(account_meta)->get_class() != "AccountMeta", "Object is not an AccountMeta.");
	const auto *account_meta_ptr = Object::cast_to<AccountMeta>(account_meta);
	const int64_t index = find(*account_meta_ptr);

	// Merge attributes if address is already in list.
	if (index != -1) {
		merge_at(account_meta, index);
	} else {
		// Create a new entry.
		merged_metas.append(account_meta);
	}
}

void MergedAccountMetas::from_instructions(const TypedArray<Instruction> &instructions) {
	for (unsigned int i = 0; i < instructions.size(); i++) {
		auto *element = Object::cast_to<Instruction>(instructions[i]);

		const TypedArray<AccountMeta> account_metas = element->get_accounts();
		const AccountMeta *pid_meta = memnew_custom(AccountMeta(element->get_program_id(), false, false));
		add(pid_meta);

		for (unsigned int j = 0; j < account_metas.size(); j++) {
			auto *account_meta = Object::cast_to<AccountMeta>(account_metas[j]);
			add(account_meta);
		}
	}
}

void MergedAccountMetas::sort() {
	TypedArray<AccountMeta> writable_signers;
	TypedArray<AccountMeta> readonly_signers;
	TypedArray<AccountMeta> writable_nonsigners;
	TypedArray<AccountMeta> readonly_nonsigners;

	for (unsigned int i = 0; i < merged_metas.size(); i++) {
		auto *element = Object::cast_to<AccountMeta>(merged_metas[i]);
		const int value = (static_cast<int>(element->get_is_signer()) * 2) + (static_cast<int>(element->get_writeable()) * 1);

		switch (value) {
			case 3:
				writable_signers.append(element);
				break;
			case 2:
				readonly_signers.append(element);
				break;
			case 1:
				writable_nonsigners.append(element);
				break;
			case 0:
				readonly_nonsigners.append(element);
				break;
			default:
				ERR_FAIL_EDMSG("Unexpected Error.");
				break;
		}
	}
	TypedArray<AccountMeta> result;
	result.append_array(writable_signers);
	result.append_array(readonly_signers);
	result.append_array(writable_nonsigners);
	result.append_array(readonly_nonsigners);

	merged_metas = result;
}

bool MergedAccountMetas::is_key_at_index(const Variant &key, int64_t index) const {
	return (Pubkey::bytes_from_variant(merged_metas[index]) == Pubkey::bytes_from_variant(key));
}

bool MergedAccountMetas::is_empty() const {
	return merged_metas.size() == 0;
}

TypedArray<AccountMeta> MergedAccountMetas::get_list() const {
	return merged_metas;
}

Array MergedAccountMetas::get_signers() const {
	Array signers;
	for (unsigned int i = 0; i < merged_metas.size(); i++) {
		auto *meta = Object::cast_to<AccountMeta>(merged_metas[i]);
		if (meta->get_is_signer()) {
			signers.append(meta->get_key());
		}
	}

	return signers;
}

void MergedAccountMetas::supply_signers(const Array &signers) {
	for (unsigned int i = 0; i < signers.size(); i++) {
		const Variant signer_meta = AccountMeta::new_account_meta(signers[i], true, false);
		const int64_t index = find(*Object::cast_to<AccountMeta>(signer_meta));
		if (index != -1) {
			merge_at(signer_meta, index);
		} else {
			WARN_PRINT_ONCE_ED("A supplied signer is not relevant to this transaction");
		}
	}
}

int64_t MergedAccountMetas::find(const AccountMeta &account_meta) const {
	for (int64_t i = 0; i < merged_metas.size(); i++) {
		if (Pubkey::bytes_from_variant(merged_metas[i]) == Pubkey::bytes_from_variant(account_meta.get_key())) {
			return i;
		}
	}
	return -1;
}

} //namespace godot