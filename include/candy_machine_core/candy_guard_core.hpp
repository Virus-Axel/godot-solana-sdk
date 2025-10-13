#ifndef CANDY_GUARD_CORE_CANDY_GUARD_CORE_HPP
#define CANDY_GUARD_CORE_CANDY_GUARD_CORE_HPP

#include "anchor/generic_anchor_resource.hpp"
#include "godot_cpp/variant/string_name.hpp"

namespace godot {

/**
 * @brief Base class for Candy Guard Core resources.
 *
 * This class will provides methods to bind Candy Guard Core node and resources from IDL.
 */
class CandyGuardCore : public GenericAnchorResource {
private:
	static std::string PID;
	static void bind_mint_methods(const StringName &class_name);
	static void bind_guard_methods();

	static Variant get_pid();

public:
	/**
	 * @brief Register Candy Guard Core classes from IDL.
	 */
	static void register_from_idl();

	/**
	 * @brief Get the extra account metas for certain Candy Guard instructions.
	 *
	 * @return Array List of extra account metas.
	 */
	Array get_extra_account_metas(const Variant &owner = nullptr);

	/**
	 * @brief Get the extra accounts for nft payment guard object.
	 * 
	 * @param owner Owner of the payment NFT.
	 * @param payment_mint Payment mint address.
	 * @param pnft Indicates if the payment is a programmable NFT.
	 * @param creator Creator of the payment NFT.
	 * @return Array List of extra account metas.
	 */
	Array get_extra_accounts_nft_payment(const Variant &owner, const Variant &payment_mint, bool pnft = false, const Variant &creator = nullptr);

	/**
	 * @brief Serializes core mint args for certain Candy Guard resource.
	 *
	 * @param group Candy Guard group to serialize.
	 * @return PackedByteArray Serialized bytes.
	 */
	static PackedByteArray serialize_core_mint_args_group(const Variant &group);

	/**
	 * @brief Serialize core mint args for certain Candy Guard instructions.
	 *
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize_core_mint_args();
};

} //namespace godot

#endif // CANDY_GUARD_CORE_CANDY_GUARD_CORE_HPP