#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include "account.hpp"
#include "hash.hpp"
#include "instruction.hpp"
#include "pubkey.hpp"
#include <message.hpp>
#include <wallet_adapter/wallet_adapter.hpp>

#include <godot_cpp/classes/node.hpp>
#include <solana_client.hpp>

namespace godot {

enum ConfirmationLevel {
	CONFIRMED,
	PROCESSED,
	FINALIZED,
};

class Transaction : public Node {
	GDCLASS(Transaction, Node)

private:
	unsigned int processed_connections = 0;
	unsigned int confirmed_connections = 0;
	unsigned int finalized_connections = 0;
	unsigned int active_subscriptions = 0;

	uint32_t unit_limit = 800000;
	uint32_t unit_price = 8000;
	uint32_t ready_signature_amount = 0;

	Message message;

	WalletAdapter *signer_controller = nullptr;

	Array address_lookup_tables;
	Array instructions;
	Variant payer;
	Array signers;
	TypedArray<PackedByteArray> signatures;

	String latest_blockhash_string = "";
	String result_signature = "";
	String latest_commitment = "";
	String url_override = "";

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
	void _signer_signed(const PackedByteArray &signature, const int32_t index);
	void _signer_failed();

	bool is_phantom_payer() const;
	bool has_valid_payer() const;
	void create_message();
	void check_fully_signed();
	void reset_state();

	void sign_at_index(const uint32_t index);
	void copy_connection_state();
	void subscribe_to_signature(ConfirmationLevel confirmation_level);
	void subscribe_to_signature();

	void _emit_processed_callback(const Dictionary &params);
	void _emit_confirmed_callback(const Dictionary &params);
	void _emit_finalized_callback(const Dictionary &params);

protected:
	static void _bind_methods();

public:
	Transaction();
	void from_bytes(const PackedByteArray &bytes);

	static Variant new_from_bytes(const PackedByteArray &bytes);

	void _ready() override;
	void _process(double delta) override;

	void set_instructions(const Array &p_value);
	Array get_instructions();

	void set_payer(const Variant &p_value);
	Variant get_payer();

	void set_url_override(const String &p_value);

	// The following functions need to comply with godot layout.
	/* NOLINTBEGIN(bugprone-easily-swappable-parameters)*/
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	/* NOLINTBEGIN(bugprone-easily-swappable-parameters)*/

	void set_signers(const Array &p_value);
	Array get_signers();

	void set_unit_limit(const uint32_t value);
	uint32_t get_unit_limit() const;

	void set_unit_price(const uint32_t value);
	uint32_t get_unit_price() const;

	void set_external_payer(bool p_value);
	bool get_external_payer() const;

	void update_latest_blockhash(const String &custom_hash = "");
	void add_instruction(const Variant &instruction);

	PackedByteArray serialize();
	PackedByteArray serialize_message();
	PackedByteArray serialize_signers();
	Error sign();
	void send();
	Variant sign_and_send();
	Error partially_sign(const Array &array);

	void send_callback(Dictionary params);
	void blockhash_callback(Dictionary params);
	void set_address_lookup_tables(const Array &address_lookup_tables);
	Array get_address_lookup_tables();

	~Transaction() = default;
};
} //namespace godot

#endif