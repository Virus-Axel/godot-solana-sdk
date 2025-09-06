#include "associated_token_account.hpp"

#include <string>

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "account_meta.hpp"
#include "instruction.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"
#include "system_program.hpp"

namespace godot {

const std::string AssociatedTokenAccountProgram::PID = "ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL";

void AssociatedTokenAccountProgram::_bind_methods() {
	ClassDB::bind_static_method("AssociatedTokenAccountProgram", D_METHOD("create_associated_token_account", "payer", "wallet_address", "mint_address", "token_program_id"), &AssociatedTokenAccountProgram::create_associated_token_account);
	ClassDB::bind_static_method("AssociatedTokenAccountProgram", D_METHOD("get_pid"), &AssociatedTokenAccountProgram::get_pid);
}

Variant AssociatedTokenAccountProgram::create_associated_token_account(const Variant &payer, const Variant &wallet_address, const Variant &mint_address, const Variant &token_program_id) {
	Instruction *result = memnew_custom(Instruction);
	PackedByteArray data;
	data.resize(1);

	data[0] = 0;

	const Variant new_pid = memnew_custom(Pubkey(String(PID.c_str())));
	result->set_program_id(new_pid);
	result->set_data(data);

	const Variant ata = Pubkey::new_associated_token_address(wallet_address, mint_address, token_program_id);

	result->append_meta(*memnew_custom(AccountMeta(payer, true, true)));
	result->append_meta(*memnew_custom(AccountMeta(ata, false, true)));
	result->append_meta(*memnew_custom(AccountMeta(wallet_address, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(mint_address, false, false)));
	const Variant SYSTEM_PID = SystemProgram::get_pid();
	result->append_meta(*memnew_custom(AccountMeta(SYSTEM_PID, false, false)));
	result->append_meta(*memnew_custom(AccountMeta(token_program_id, false, false)));

	return result;
}

Variant AssociatedTokenAccountProgram::get_pid() {
	return Pubkey::new_from_string(PID.c_str());
}

} //namespace godot