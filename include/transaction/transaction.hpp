#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "message.hpp"
#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot {

enum ConfirmationLevel : uint8_t {
	CONFIRMED = 0,
	PROCESSED,
	FINALIZED,
};

const uint32_t DEFAULT_UNIT_LIMIT = 800000;
const uint32_t DEFAULT_UNIT_PRICE = 8000;

/**
 * @brief Used to construct and send transactions on Solana blockchain.
 *
 * This class is an essential class in all Solana interaction. It is used in all underlying
 * on-chain communication where the state of the blockchain is changed. It handles the whole
 * procees of compiling, signing and sending.
 */
class Transaction : public SolanaClient {
	GDCLASS_CUSTOM(Transaction, SolanaClient)

private:
	unsigned int processed_connections = 0;
	unsigned int confirmed_connections = 0;
	unsigned int finalized_connections = 0;
	unsigned int active_subscriptions = 0;

	uint32_t unit_limit = DEFAULT_UNIT_LIMIT;
	uint32_t unit_price = DEFAULT_UNIT_PRICE;
	uint32_t ready_signature_amount = 0;

	Message message;

	Array address_lookup_tables;
	Array instructions;
	Variant payer;
	Array signers;
	TypedArray<PackedByteArray> signatures;

	String latest_blockhash_string = "";
	String result_signature = "";
	String latest_commitment = "";

	SolanaClient *send_client = nullptr;
	SolanaClient *blockhash_client = nullptr;
	SolanaClient *subscribe_client = nullptr;

	bool has_cumpute_budget_instructions = false;
	bool external_payer = false;
	bool pending_blockhash = false;
	bool pending_send = false;
	bool append_budget_instructions = true;

	bool skip_preflight = false;
	bool deserialized = false;

	static bool are_all_bytes_zeroes(const PackedByteArray &bytes);

	void _get_property_list(List<PropertyInfo> *p_list) const;
	void _signer_signed(const PackedByteArray &signature, int32_t index);
	void _signer_failed(const int32_t index);

	[[nodiscard]] bool is_phantom_payer() const;
	[[nodiscard]] bool has_valid_payer() const;
	void create_message();
	void check_fully_signed();
	void reset_state();

	void sign_at_index(uint32_t index);
	void copy_connection_state();
	void subscribe_to_signature(ConfirmationLevel confirmation_level);
	void subscribe_to_signature();

	void _emit_processed_callback(const Dictionary &params);
	void _emit_confirmed_callback(const Dictionary &params);
	void _emit_finalized_callback(const Dictionary &params);

	void check_transaction_simulation(const Dictionary &params);

protected:
	/**
	 * @brief @bindmethods{Transaction, Node}
	 */
	static void _bind_methods();

public:
	Transaction();

	/**
	 * @brief Assembles this Transaction from a serialized Transaction.
	 *
	 * @param bytes The serialized Transaction to reconstruct.
	 */
	void from_bytes(const PackedByteArray &bytes);

	/**
	 * @brief Creates a new Transaction from a serialized Transaction.
	 *
	 * @param bytes The serialized Transaction to reconstruct.
	 * @return Variant Reconstructed Transaction.
	 */
	static Variant new_from_bytes(const PackedByteArray &bytes);

	/**
	 * @_ready
	 */
	void _ready() override;

	/**
	 * @_process
	 */
	void _process(double delta) override;

	/**
	 * @setter{instruction, p_value}
	 */
	void set_instructions(const Array &p_value);

	/**
	 * @getter{instruction, Array}
	 */
	Array get_instructions();

	/**
	 * @setter{payer, p_value}
	 */
	void set_payer(const Variant &p_value);

	/**
	 * @getter{payer, Variant}
	 */
	Variant get_payer();

	// The following functions need to comply with godot layout.
	/**
	 * @_set{Transaction}
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @_get{Transaction}
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;

	/**
	 * @setter{signers, p_value}
	 */
	void set_signers(const Array &p_value);

	/**
	 * @getter{signers, Array}
	 */
	Array get_signers();

	/**
	 * @setter{unit_limit, value}
	 */
	void set_unit_limit(uint32_t value);

	/**
	 * @getter{unit_limit, uint32_t}
	 */
	[[nodiscard]] uint32_t get_unit_limit() const;

	/**
	 * @setter{unit_limit, value}
	 */
	void set_unit_price(uint32_t value);

	/**
	 * @getter{unit_price, uint32_t}
	 */
	[[nodiscard]] uint32_t get_unit_price() const;

	/**
	 * @setter{external_payer, p_value}
	 */
	void set_external_payer(bool p_value);

	/**
	 * @getter{external_payer, bool}
	 */
	[[nodiscard]] bool get_external_payer() const;

	/**
	 * @brief Sends a request to update latest blockhash or updates from parameter.
	 *
	 * If the custom_hash is empty the latest blockhash will be fetched from the RPC node.
	 * If the custom_hash is provided, the latest blockhash will be set to the provided hash.
	 *
	 * @param custom_hash Base58 encoded blockhash, can be empty.
	 */
	void update_latest_blockhash(const String &custom_hash = "");

	/**
	 * @brief Adds an instruction to the instruction array.
	 *
	 * @param instruction Instruction to add.
	 */
	void add_instruction(const Variant &instruction);

	/**
	 * @serializer{Transaction}
	 */
	PackedByteArray serialize();

	/**
	 * @serializer{Transaction's Message}
	 */
	PackedByteArray serialize_message();

	/**
	 * @serializer{Transaction's signers}
	 */
	PackedByteArray serialize_signers();

	/**
	 * @brief Sign the transaction with available signers.
	 *
	 * Asynchronously fills the signature array with the available signers. If a WalletAdapter is
	 * present as a signer, a popup window should appear for user approval. Signers provided from
	 * AccountMeta's when building an instruction are saved internally and does not need to be added
	 * with set_signers.
	 *
	 * @return Error Status of sign.
	 */
	Error sign();

	/**
	 * @brief Send a Transaction to the RPC node.
	 *
	 * @note Transactions should be fully signed at this point.
	 */
	void send();

	/**
	 * @brief Sign and send a Transaction.
	 *
	 * @return Variant OK.
	 */
	Variant sign_and_send();

	/**
	 * @brief Adds required signatures using provided list of signers.
	 *
	 * @param array Signatures to sign with.
	 * @return Error Status of signing.
	 */
	Error partially_sign(const Array &array);

	/**
	 * @brief Called when send is finalized.
	 *
	 * @param params Send response.
	 */
	void send_callback(Dictionary params);

	/**
	 * @brief Called when blockhash request is finalized.
	 *
	 * @param params Request response.
	 */
	void blockhash_callback(Dictionary params);

	/**
	 * @setter{address_lookup_tables}
	 */
	void set_address_lookup_tables(const Array &address_lookup_tables);

	/**
	 * @getter{address_lookup_tables, Array}
	 */
	Array get_address_lookup_tables();

	~Transaction() override = default;
};
} //namespace godot

#endif