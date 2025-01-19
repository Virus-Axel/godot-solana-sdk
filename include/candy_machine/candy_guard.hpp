#ifndef SOLANA_SDK_MPL_CANDY_GUARD
#define SOLANA_SDK_MPL_CANDY_GUARD

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

class MplCandyGuard : public Node {
	GDCLASS(MplCandyGuard, Node)
private:
	static PackedByteArray wrap_discriminator();
	static PackedByteArray serialize_label(const String &label);

protected:
	static void _bind_methods();

public:
	static const std::string ID;

	static Variant initialize(const Variant &owner, const Variant &candy_guard_authority, const Variant &payer, const Variant &candy_guard_acl);
	static Variant wrap(const Variant &candy_guard_id, const Variant &guard_authority, const Variant &candy_machine_id, const Variant &candy_machine_authority);
	static Variant mint(
			const Variant &candy_machine_id,
			const Variant &guard_account_id,
			const Variant &payer,
			const Variant &receiver,
			const Variant &mint,
			const Variant &nft_mint_authority,
			const Variant &collection_mint,
			const Variant &collection_update_authority,
			const Variant &candy_guard_acl,
			const String &label);

	static Variant get_pid();
	static Variant new_associated_candy_guard_key(const Variant &candy_machine_key);
	static Variant new_limit_counter_pda(const uint8_t mint_guard_id, const Variant &payer, const Variant &candy_machine_key, const Variant &candy_guard_key);
};

} //namespace godot

#endif