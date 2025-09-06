#ifndef SOLANA_SDK_MESSAGE_HPP
#define SOLANA_SDK_MESSAGE_HPP

#include <cstdint>

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "compiled_instruction.hpp"
#include "instruction.hpp"
#include "merged_account_metas.hpp"
#include "pubkey.hpp"

namespace godot {

const int64_t BLOCKHASH_LENGTH = 32;

/**
 * @brief A helper class for transaction messages
 *
 * This class handles the transaction message operations. The class will manage
 * the message state and is responsible of creating a serialization of message
 * bytes.
 */
class Message {
private:
	uint8_t payer_index = 0;
	uint8_t num_required_signatures = 0;
	uint8_t num_readonly_signed_accounts = 0;
	uint8_t num_readonly_unsigned_accounts = 0;
	TypedArray<Pubkey> account_keys;
	String latest_blockhash = "";
	TypedArray<CompiledInstruction> compiled_instructions;
	MergedAccountMetas meta_list;

	bool is_versioned_transaction = false;

	Array address_lookup_tables;

	void compile_instruction(const Variant &instruction);
	void recalculate_headers();
	void read_accounts_from_bytes(const PackedByteArray &bytes, uint8_t num_accounts, bool is_signer, bool is_writable, int64_t &cursor);

	bool is_versioned();

protected:
public:
	Message() = default;

	/**
	 * @brief Creates the transaction message.
	 *
	 * Creates the message base from a merged list of account metas and a payer.
	 * This method combines the payer and the list of account metas into one and builds the
	 * message header.
	 *
	 * @param merged_meta_list A merged list of account metas.
	 * @param payer Payer of the transaction.
	 */
	void create(const MergedAccountMetas &merged_meta_list, Variant &payer);

	/**
	 * @brief Recreates a complete transaction message.
	 *
	 * This method parses the message header and performs some error checks.
	 * If valid the method reads all the properties of the serialized message.
	 * This method can be used to recreate a message from a serialized transaction.
	 *
	 * @param bytes The serialized message bytes.
	 */
	void create(const PackedByteArray &bytes);

	/**
	 * @brief Set the latest blockhash for the message.
	 *
	 * @param blockhash Blockhash as base58 encoded string.
	 */
	void set_latest_blockhash(const String &blockhash);

	/**
	 * @brief Serializes the entire message.
	 *
	 * @return Byte array of the serialized message.
	 */
	PackedByteArray serialize();

	/**
	 * @brief Serialize the message blockhash.
	 *
	 * The deserialized blockhash will be returned if the blockhash is valid.
	 * If the blockhash is invalid, the method will return an empty byte array.
	 *
	 * @return Byte array of the serialized blockhash
	 */
	PackedByteArray serialize_blockhash();

	/**
	 * @brief Serializes the lookup tables.
	 *
	 * @return Byte array of the serialized lookup table.
	 */
	PackedByteArray serialize_lookup_tables();

	/**
	 * @brief Get the compiled instructions array.
	 *
	 * @return Array of the compiled transactions.
	 */
	[[nodiscard]] TypedArray<CompiledInstruction> get_compiled_instructions() const;

	/**
	 * @brief Get the amount of required signers.
	 *
	 * @return Amount of required signers.
	 */
	[[nodiscard]] int get_amount_signers() const;

	/**
	 * @brief Get the objects marked as signers
	 *
	 * This method returns all key object where signer is set to true.
	 *
	 * @note The array can consist of Pubkeys, if signer is not yet provided.
	 *
	 * @return Array of key objects marked as signers.
	 */
	Array get_signers();

	/**
	 * @brief Set the address lookup tables array.
	 *
	 * @param address_lookup_tables Address lookup table.
	 */
	void set_address_lookup_tables(const Array &address_lookup_tables);

	/**
	 * @brief Get the address lookup tables object
	 *
	 * @return Address lookup table array.
	 */
	Array get_address_lookup_tables();

	/**
	 * @brief Compiles all provided instructions.
	 *
	 * This method compiles the provided list of instructions and sets its internal
	 * list of compiled instructions.
	 *
	 * @param instructions List of instructions to compile.
	 */
	void compile_instructions(const TypedArray<Instruction> &instructions);

	/**
	 * @brief Supplies signers to the message.
	 *
	 * This finds the account metas that matches the addresses of the provided signers.
	 * It then sets the provided signer object for that account meta if needed.
	 *
	 * @param signers Array of signers.
	 */
	void supply_signers(const Array &signers);

	/**
	 * @brief Checks if message is valid
	 *
	 * This checks if the message has a list of account metas, message header is
	 * populated, and list of compiled transaction is not empty.
	 *
	 * @note The message may be considered valid but might still have corrupted data.
	 *
	 * @return true if the message is considered valid.
	 * @return false if the message is considered invalid.
	 */
	[[nodiscard]] bool is_valid() const;
	~Message() = default;
};

} // namespace godot

#endif