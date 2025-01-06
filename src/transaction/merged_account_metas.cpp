#include "merged_account_metas.hpp"

#include "address_lookup_table.hpp"
#include "compute_budget.hpp"
#include "wallet_adapter.hpp"
#include <cstdint>
#include <godot_cpp/core/error_macros.hpp>
#include <keypair.hpp>
#include <pubkey.hpp>
#include <solana_utils.hpp>

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

void MergedAccountMetas::merge_at(int64_t index, const Variant &account_meta) {
	auto *existing_meta = Object::cast_to<AccountMeta>(merged_metas[index]);
	auto *new_meta = Object::cast_to<AccountMeta>(account_meta);
	existing_meta->set_key(preferred_signer(existing_meta->get_key(), new_meta->get_key()));
	existing_meta->set_is_signer(existing_meta->get_is_signer() || new_meta->get_is_signer());
	existing_meta->set_writeable(existing_meta->get_writeable() || new_meta->get_writeable());
}

void MergedAccountMetas::add(const Variant &account_meta) {
	const auto *account_meta_ptr = Object::cast_to<AccountMeta>(account_meta);
	const int index = find(*account_meta_ptr);

	// Merge attributes if address is already in list.
	if (index != -1) {
		merge_at(index, account_meta);
	} else {
		// Create a new entry.
		merged_metas.append(account_meta);
	}
}

void MergedAccountMetas::from_instructions(const TypedArray<Instruction> &instructions) {
	for (unsigned int i = 0; i < instructions.size(); i++) {
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
	TypedArray<AccountMeta> m1;
	TypedArray<AccountMeta> m2;
	TypedArray<AccountMeta> m3;
	TypedArray<AccountMeta> m4;

	for (unsigned int i = 0; i < merged_metas.size(); i++) {
		AccountMeta *element = Object::cast_to<AccountMeta>(merged_metas[i]);
		const int value = element->get_is_signer() * 2 + element->get_writeable() * 1;

		switch (value) {
			case 3:
				m1.append(element);
				break;
			case 2:
				m2.append(element);
				break;
			case 1:
				m3.append(element);
				break;
			case 0:
				m4.append(element);
				break;
			default:
				ERR_FAIL_EDMSG("Unexpected Error.");
				break;
		}
	}
	TypedArray<AccountMeta> result;
	result.append_array(m1);
	result.append_array(m2);
	result.append_array(m3);
	result.append_array(m4);

	merged_metas = result;
}

bool MergedAccountMetas::is_key_at_index(const Variant &key, uint64_t index) const {
	return (Pubkey::bytes_from_variant(merged_metas[index]) == Pubkey::bytes_from_variant(key));
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
		Variant signer_meta = AccountMeta::new_account_meta(signers[i], true, false);
		const int64_t index = find(*Object::cast_to<AccountMeta>(signer_meta));
		if (index != -1) {
			merge_at(index, signer_meta);
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