#ifndef SOLANA_SDK_MPL_CANDY_MACHINE
#define SOLANA_SDK_MPL_CANDY_MACHINE

#include <string>

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Instruction builder for Candy Machine v3 program.
 */
class MplCandyMachine : public SolanaClient {
	GDCLASS_CUSTOM(MplCandyMachine, SolanaClient)
private:
	static const std::string PID;

protected:
	/**
	 * @brief @bindmethods{MplCandyMachine, Node}
	 */
	static void _bind_methods();

public:
	MplCandyMachine() = default;

	/**
	 * @brief Get the 8 byte discriminator of mint instruction.
	 *
	 * @return PackedByteArray discriminator.
	 */
	static PackedByteArray mint_discriminator();

	/**
	 * @brief Get the 8 byte discriminator of mintv2 instruction.
	 *
	 * @return PackedByteArray discriminator.
	 */
	static PackedByteArray mint2_discriminator();

	/**
	 * @brief Get the 8 byte discriminator of initialize instruction.
	 *
	 * @return PackedByteArray discriminator.
	 */
	static PackedByteArray initialize_discriminator();

	/**
	 * @brief Get the 8 byte discriminator of initializev2 instruction.
	 *
	 * @return PackedByteArray discriminator.
	 */
	static PackedByteArray initialize2_discriminator();

	/**
	 * @brief Get the 8 byte discriminator of add config lines instruction.
	 *
	 * @return PackedByteArray discriminator.
	 */
	static PackedByteArray add_config_lines_discriminator();

	/**
	 * @brief Create Instruction to initialize a candy machine account.
	 *
	 * Creates and initializes a new CandyMachine account that support multiple token standard.
	 * It requires that the CandyMachine account has been created with the expected size before
	 * executing this instruction. A Candy Machine created through this instruction will have
	 * its AccountVersion set to V2.
	 *
	 * @note This Instruction is called InitializeV2 in MplCandyMachine program.
	 *
	 * @param authority Payer of the transaction and authority of the candy machine. @signer @writable
	 * @param candy_machine_account Key to the candy machine account to initialize. @signer
	 * @param collection_mint Mint account of the collection.
	 * @param candy_machine_data CandyMachineData with candy machine arguments.
	 * @param pnft Specifies whether token standard is pNFT or not.
	 * @return Variant initialize Instruction.
	 */
	static Variant initialize(
			const Variant &authority,
			const Variant &candy_machine_account,
			const Variant &collection_mint,
			const Variant &candy_machine_data,
			bool pnft = false);

	/**
	 * @brief Create an instruction to mint an NFT or pNFT from a candy machine.
	 *
	 * @note Only the mint authority is allowed to mint from the candy machine.
	 * @note This Instruction is called MintV2 in MplCandyMachine program.
	 *
	 * @param payer Payer of the transaction as well as mint authority. @signer @writable
	 * @param receiver NFT token account owner.
	 * @param mint Mint account for the NFT. The account should be created before executing the instruction. @writable
	 * @param collection_mint Mint account of the collection.
	 * @param collection_update_authority Collection update authority key.
	 * @param candy_machine_key Candy machine key.
	 * @return Variant mint Instruction.
	 */
	static Variant mint(
			const Variant &payer,
			const Variant &receiver,
			const Variant &mint,
			const Variant &collection_mint,
			const Variant &collection_update_authority,
			const Variant &candy_machine_key);

	/**
	 * @brief Create an Instruction that adds config lines to a candy machine.
	 *
	 * This instruction adds config lines to the hidden data section of the account.
	 * It can only be used if the candy machine has config_line_settings.
	 *
	 * @param candy_machine_key Candy machine account key. @writable
	 * @param authority Authority of the candy machine. @signer
	 * @param config_lines ConfigLine to add.
	 * @param index Index where ConfigLine should be added.
	 * @return Variant Add config line Instruction.
	 */
	static Variant add_config_lines(
			const Variant &candy_machine_key,
			const Variant &authority,
			const Array &config_lines,
			unsigned int index);

	/**
	 * @brief Create a new candy machine authority PDA.
	 *
	 * @pda{"candy_machine", candy_machine_key}
	 *
	 * @param candy_machine_key Candy machine account key.
	 * @return Variant Candy machine authority PDA Pubkey.
	 */
	static Variant new_candy_machine_authority_pda(const Variant &candy_machine_key);

	/**
	 * @brief Fetch CandyMachineData from an account key.
	 *
	 * @param candy_machine_key Account key.
	 */
	void get_candy_machine_info(const Variant &candy_machine_key);

	/**
	 * @brief Called when CandyMachineData is fetched.
	 *
	 * @param params Account data.
	 */
	void fetch_account_callback(const Dictionary &params);

	/**
	 * @brief Get the MplCandyMachine program ID.
	 *
	 * @return Variant Program ID Pubkey.
	 */
	static Variant get_pid();
};

} //namespace godot

#endif