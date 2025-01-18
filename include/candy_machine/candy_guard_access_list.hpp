#ifndef SOLANA_SDK_MPL_CANDY_GUARD_ACCESS_LIST
#define SOLANA_SDK_MPL_CANDY_GUARD_ACCESS_LIST

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

class CandyGuardAccessList : public Resource {
	GDCLASS(CandyGuardAccessList, Resource)
private:
	String label = "default";
	bool top_level = true;

	bool enable_address_gate = false;
	Variant address_gate_reciever;

	bool enable_allocation = false;
	uint32_t allocation_id = 0;
	uint32_t allocation_size = 0;

	bool enable_allow_list = false;
	PackedByteArray allowed_merkle_root;

	bool enable_bot_tax = false;
	uint64_t bot_tax_lamports = 0;
	bool validate_last_instruction = false;

	bool enable_end_date = false;
	int64_t end_date = 0;

	bool enable_freeze_sol_payment = false;
	uint64_t freeze_amount_lamports = 0;
	Variant freeze_sol_destination;

	bool enable_freeze_token_payment = false;
	uint64_t amount = 0;
	Variant mint;
	Variant freeze_token_destination_ata;

	bool enable_gatekeeper = false;
	Variant gatekeeper_network;
	bool expire_on_use = false;

	bool enable_mint_limit = false;
	uint32_t limit_id = 0;
	uint32_t mint_limit = 0;

	bool enable_nft_burn = false;
	Variant required_nft_burn_collection;

	bool enable_nft_gate = false;
	Variant required_nft_gate_collection;

	bool enable_nft_payment = false;
	Variant required_nft_payment_collection;
	Variant nft_payment_destination;

	bool enable_program_gate = false;
	Array program_gate_addresses;

	bool enable_redeem_account = false;
	uint64_t max_redeem_amount = 0;

	bool enable_sol_payment = false;
	uint64_t sol_payment_lamports = 0;
	Variant sol_payment_destination;

	bool enable_start_date = false;
	int64_t start_date = 0;

	bool enable_third_party_signer = false;
	Variant signer_key;

	bool enable_token_burn = false;
	uint64_t token_burn_amount = 0;
	Variant token_burn_mint;

	bool enable_token_gate = false;
	uint64_t token_gate_amount = 0;
	Variant token_gate_mint;

	bool enable_token_payment = false;
	uint64_t token_payment_amount = 0;
	Variant token_payment_mint;
	Variant token_payment_destination;

	TypedArray<CandyGuardAccessList> groups;

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
	static void _bind_methods();

public:
	PackedByteArray serialize() const;
	PackedByteArray serialize_mint_settings() const;
	PackedByteArray serialize_groups() const;
	PackedByteArray serialize_guard_settings() const;

	CandyGuardAccessList &get_group(const String &label);
	TypedArray<AccountMeta> get_mint_arg_accounts(const Variant &payer, const Variant &machine_key, const Variant &guard_key);
};

} //namespace godot

#endif