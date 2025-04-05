#ifndef SOLANA_SDK_MPL_TOKEN_PROGRAM
#define SOLANA_SDK_MPL_TOKEN_PROGRAM

#include <godot_cpp/classes/node.hpp>
#include <instruction.hpp>
#include <meta_data.hpp>
#include <solana_client.hpp>

namespace godot {

/**
 * @brief Instruction builder for Metaplex token metadata program.
 */
class MplTokenMetadata : public SolanaClient {
	GDCLASS(MplTokenMetadata, SolanaClient)
private:
	static const std::string ID;

	bool pending_fetch = false;

	void metadata_callback(const Dictionary &rpc_result);

protected:
    /**
     * @bindmethods{MplTokenMetadata, Node}
     */
	static void _bind_methods();

public:
	MplTokenMetadata();

    /**
     * @brief Create new associated metadata Pubkey.
     * 
     * @pda{"metadata", token_metadata_pid, mint}
     * 
     * @param mint Mint key.
     * @return Variant Associated metadata Pubkey.
     */
	static Variant new_associated_metadata_pubkey(const Variant &mint);

    /**
     * @brief Create new associated metadata pubkey master edition.
     * 
     * @pda{"metadata", token_metadata_pid, mint, "edition"}
     * 
     * @param mint Mint key.
     * @return Variant Associated metadata Pubkey master edition.
     */
	static Variant new_associated_metadata_pubkey_master_edition(const Variant &mint);

    /**
     * @brief Fetch the mint MetaData from a mint account.
     * 
     * @param mint Mint account.
     * @return Variant Status code of request.
     */
	Variant get_mint_metadata(const Variant &mint);

    /**
     * @brief Create Instruction that creates a metadata account.
     * 
     * @note This instruciton is called CreateMetadataAccountv3 in token metadata program.
     * 
     * @param mint Mint of token asset.
     * @param mint_authority Authority of mint. @signer
     * @param update_authority Update authority info.
     * @param meta_data CreateMetaDataArgs resource.
     * @param is_mutable Indicates whether metadata is mutable.
     * @return Variant Create metadata account Instruction
     */
	static Variant create_metadata_account(const Variant &mint, const Variant &mint_authority, const Variant &update_authority, const Variant &meta_data, bool is_mutable);

    /**
     * @brief Create instruction to update metadata account.
     * 
     * @param metadata_account Metadata account to update. @writable
     * @param update_authority Update authority key. @signer
     * @return Variant Update metadata account Instruction.
     */
	static Variant update_metadata_account(const Variant &metadata_account, const Variant &update_authority);

    /**
     * @brief Create instruction to create master edition account.
     * 
     * @note This instruction is called CreateMasterEditionv3 in the token metadata program.
     * @note Mint authority will be transferred in this instruction.
     * 
     * @param mint Metadata mint. @writable
     * @param update_authority Update authority. @signer
     * @param mint_authority Mint authority on the metadata's mint. @signer
     * @param payer Payer of transaction. @signer @writable
     * @param max_supply Max supply.
     * @return Variant Create master edition Instruction.
     */
	static Variant create_master_edition(const Variant &mint, const Variant &update_authority, const Variant &mint_authority, const Variant &payer, const Variant &max_supply);

    /**
     * @brief Get the MplTokenMetadata program ID.
     * 
     * @return Variant Program ID.
     */
	static Variant get_pid();
};

} //namespace godot

#endif