#ifndef SOLANA_SDK_MPL_CANDY_GUARD
#define SOLANA_SDK_MPL_CANDY_GUARD

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

/**
 * @brief Instruction builder for Candy Guard v3 program.
 */
class MplCandyGuard : public Node {
	GDCLASS(MplCandyGuard, Node)
private:
	static const std::string ID;
	static PackedByteArray wrap_discriminator();
	static PackedByteArray serialize_label(const String &label);

protected:
	/**
	 * @brief @bindmethods{MplCandyGuard, Node}
	 */
	static void _bind_methods();

public:

	/**
	 * @brief Create an instruction to initialize a CandyGuard account.
	 * 
	 * @param owner Owner of candy guard program. @signer
	 * @param candy_guard_authority Authority of candy guard account.
	 * @param payer Payer of the transaction. @signer
	 * @param candy_guard_acl CandyGuardAcl specifying the guards to use.
	 * @return Variant Initialize Instruction.
	 */
	static Variant initialize(const Variant &owner, const Variant &candy_guard_authority, const Variant &payer, const Variant &candy_guard_acl);
	
	/**
	 * @brief Create wrap instruction to add a CandyGuard to a CandyMachine.
	 * 
	 * When a CandyGuard is added to a CandyMachine, minting is only allowed through MplCandyGuard program.
	 * 
	 * @param candy_guard_id Candy Guard key. Can be retrieved using new_associated_candy_guard_key.
	 * @param guard_authority Authority of candy guard account. @signer
	 * @param candy_machine_id Candy Machine key to add guard to. @writable
	 * @param candy_machine_authority Authority of the Candy Machine. @signer
	 * @return Variant Wrap instruction.
	 */
	static Variant wrap(const Variant &candy_guard_id, const Variant &guard_authority, const Variant &candy_machine_id, const Variant &candy_machine_authority);
	
	/**
	 * @brief Create instruction to mint from a candy machine wrapped by a candy guard.
	 * 
	 * Can mint both NFT and pNft. Only when the transaction is succesfully validated
	 * by the MplCandyGuard program, it is forwarded to the Candy Machine.
	 * 
	 * @note In Metaplex program this instruction is referred to as mintV2.
	 * 
	 * @param candy_machine_id Candy machine account key. @writable
	 * @param guard_account_id Candy guard key. Can be retrieved using new_associated_candy_guard_key.
	 * @param payer Payer of the transaction. @signer @writable
	 * @param receiver Minter (owner) of the NFT. @signer @writable
	 * @param mint Mint account of the NFT. Must be created before executing this instruction. @writable
	 * @param nft_mint_authority Mint authority of the NFT.
	 * @param collection_mint Mint account of the collection.
	 * @param collection_update_authority Update authority of the collection.
	 * @param candy_guard_acl CandyGuardAcl with mint parameters.
	 * @param label Gourd group label to mint with.
	 * @return Variant Mint Instruction.
	 */
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

	/**
	 * @brief Get the MplCandyGuard Program ID.
	 * 
	 * @return Variant Program ID Pubkey.
	 */
	static Variant get_pid();

	/**
	 * @brief Create a new associated candy guard key,
	 * 
	 * @pda{"candy_guard", base}
	 * 
	 * @param candy_machine_key Base (owner) of the candy guard account.
	 * @return Variant PDA Pubkey.
	 */
	static Variant new_associated_candy_guard_key(const Variant &candy_machine_key);

	/**
	 * @brief Create a new mint limit PDA Pubkey.
	 * 
	 * @pda{"mint_limit", mint_guard_id, payer, candy_machine_key, candy_guard_key}
	 * 
	 * @param mint_guard_id Mint guard account key.
	 * @param payer Payer key.
	 * @param candy_machine_key Candy machine key.
	 * @param candy_guard_key Candy guard key.
	 * @return Variant Mint counter account PDA Pubkey. 
	 */
	static Variant new_limit_counter_pda(const uint8_t mint_guard_id, const Variant &payer, const Variant &candy_machine_key, const Variant &candy_guard_key);
};

} //namespace godot

#endif