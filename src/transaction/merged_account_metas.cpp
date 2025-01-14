#include "merged_account_metas.hpp"

#include <cstdint>

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "instruction.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"

Variant MergedAccountMetas::preferred_signer(const Variant &left, const Variant &right) {
	const auto *left_object = static_cast<Object *>(left);
	const auto *right_object = static_cast<Object *>(right);

	// Keypair is prefferred over WalletAdapter since they can instantly sign.
	if (left_object->is_class("Keypair")) {
		return left;
	}
	if (right_object->is_class("Keypair")) {
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

void MergedAccountMetas::merge_at(const AccountMeta *account_meta, int64_t index) {
	auto *existing_meta = Object::cast_to<AccountMeta>(merged_metas[index]);
	existing_meta->set_key(preferred_signer(existing_meta->get_key(), account_meta->get_key()));
	existing_meta->set_is_signer(existing_meta->get_is_signer() || account_meta->get_is_signer());
	existing_meta->set_writeable(existing_meta->get_writeable() || account_meta->get_writeable());
}

void MergedAccountMetas::clear() {
	merged_metas.clear();
}

void MergedAccountMetas::add(const Variant &account_meta) {
	const auto *account_meta_ptr = Object::cast_to<AccountMeta>(account_meta);
	const int64_t index = find(*account_meta_ptr);

	// Merge attributes if address is already in list.
	if (index != -1) {
		merge_at(account_meta_ptr, index);
	} else {
		// Create a new entry.
		merged_metas.append(account_meta);
	}
}

void MergedAccountMetas::from_instructions(const TypedArray<Instruction> &instructions) {
	for (unsigned int i = 0; i < instructions.size(); i++) {
		ERR_FAIL_COND_EDMSG_CUSTOM(instructions[i].get_type() != Variant::OBJECT, "Trying to add an instruction with invalid type.");
		ERR_FAIL_COND_EDMSG_CUSTOM(Object::cast_to<Object>(instructions[i])->get_class() != "Instruction", "Trying to add an instruction with invalid type.");
		auto *element = Object::cast_to<Instruction>(instructions[i]);

		const TypedArray<AccountMeta> &account_metas = element->get_accounts();
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
			merge_at(Object::cast_to<AccountMeta>(signer_meta), index);
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