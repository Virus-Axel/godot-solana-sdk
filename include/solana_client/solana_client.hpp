#ifndef SOLANA_CLIENT_SOLANA_CLIENT_HPP
#define SOLANA_CLIENT_SOLANA_CLIENT_HPP
#include <cstdint>
#include <string>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "rpc_multi_http_request_client.hpp"
#include "rpc_single_ws_request_client.hpp"

namespace godot {

const uint32_t ASSET_DEFAULT_FETCH_LIMIT = 10;
const float DEFAULT_TIMEOUT_SECONDS = 20.0F;

const uint32_t HTTPS_PORT = 443;
const uint32_t WSS_PORT = 443;

/**
 * @class SolanaClient
 * @brief Manager for RPC HTTP API communication.
 *
 * Interacts with an RPC node on a Solana cluster. This class
 * takes user requests and sends them sequentially or in parallel to an
 * RPC endpoints. The SolanaClient also manages the web socket interface.
 */
class SolanaClient : public Node {
	GDCLASS(SolanaClient, Node)

private:
	float timeout = DEFAULT_TIMEOUT_SECONDS;

	uint32_t http_port_override = 0;
	uint32_t ws_port_override = 0;

	String url_override = "";
	String ws_url = "";

	bool use_tls = true;
	bool async_override = false;
	bool pending_request = false;

	String commitment = "confirmed";
	String encoding = "base64";
	std::string transaction_detail;
	std::string identity;
	uint64_t min_context_slot = 0;
	uint64_t filter_offset = 0;
	uint64_t filter_length = 0;
	uint64_t max_transaction_version = 0;
	uint64_t first_slot = 0;
	uint64_t last_slot = 0;
	bool account_filter_enabled = false;
	bool min_constext_slot_enabled = false;
	bool max_transaction_version_enabled = false;
	bool rewards = false;
	bool slot_range_enabled = false;

	Callable ws_callback = callable_mp(this, &SolanaClient::ws_response_callback);;
	Callable rpc_callback = callable_mp(this, &SolanaClient::response_callback);

	String ws_from_http(const String &http_url);
	String get_real_url();
	[[nodiscard]] uint32_t get_real_http_port() const;
	[[nodiscard]] uint32_t get_real_ws_port() const;
	String get_real_ws_url();

	static PackedStringArray get_address_strings(const Array &keys);
	static RpcSingleWsRequestClient *get_current_ws_client();
	static RpcMultiHttpRequestClient *get_current_http_client();

	void append_commitment(Array &options);
	void append_min_context_slot(Array &options) const;
	void append_encoding(Array &options);
	void append_account_filter(Array &options) const;
	static void append_data_filter(Array &options, const Array &filters);
	void append_transaction_detail(Array &options);
	void append_max_transaction_version(Array &options) const;
	void append_rewards(Array &options) const;
	void append_identity(Array &options);
	void append_slot_range(Array &options) const;
	void append_limit(Array &options);

	static void add_to_param_dict(Array &options, const String &key, const Variant &value);

	static Dictionary make_rpc_param(const Variant &key, const Variant &value);
	void quick_http_request(const Dictionary &request_body);

	void response_callback(const Dictionary &params);
	void ws_response_callback(const Dictionary &params);

protected:
	static void _bind_methods();

public:
	/** Incrementing ID of the next request. */
	static unsigned int global_rpc_id;

	SolanaClient();

	/**
	 * @brief Parses a URL string into its components
	 *
	 * Parses scheme, userinfo, host, port, path, query and fragment from a URL string.
	 * If the information is missing, the field will not be present in the result dictionary.
	 *
	 * @param url URL Sting to parse the data from.
	 * @return Dictionary with the parsed URL information.
	 */
	static Dictionary parse_url(const String &url);

	/**
	 * @brief Assembles a URL string from a dictionary of components.
	 *
	 * This is the reverse of parse_url. The method takes the a dictionary with the fields
	 * scheme, userinfo, host, port, path, query and fragment. It extracts the fields and
	 * creates a URL string.
	 *
	 * @param url_components A dictionary with URL data.
	 * @return An assembled URL String.
	 */
	static String assemble_url(const Dictionary &url_components);

	/**
	 * @brief Get the next request identifier.
	 *
	 * @return The request ID of the next request.
	 */
	static unsigned int get_next_request_identifier();

	void _process(double delta) override;
	void _ready() override;

	/**
	 * @brief Builds a dictionary representing the RPC request body.
	 *
	 * Takes a RPC method and parameters and creates a RPC request body as a Dictionary.
	 * The request body will contain a request identifier and the identifier will be
	 * incremented.
	 *
	 * @param method Name of the RPC method.
	 * @param params Array of parameters.
	 * @return Dictionary representing the request body.
	 */
	static Dictionary make_rpc_dict(const String &method, const Array &params);

	/**
	 * @brief Set the callback of the http requests.
	 *
	 * This method will set a custom callback to use when getting an http response.
	 * The default method is response_callback.
	 *
	 * @param callback The callback to run when http respponse is received.
	 */
	void set_callback(const Callable &callback);

	/**
	 * @brief Set the url to use by this client instance.
	 *
	 * The SolanaClient uses the URL in project settings. However, this method overrides that URL.
	 * By calling this method this instance will use the provided URL instead. Once the URL override
	 * is set it cannot be unset.
	 *
	 * @param url
	 */

	void set_url_override(const String &url);

	/**
	 * @brief Get the url override URL.
	 *
	 * @return Overridden URL String.
	 */
	String get_url_override();

	/**
	 * @brief Set the timeout of requests
	 *
	 * Sets timeout to use when calling RPC endpoint. When a request takes longer that the timeout,
	 * the SolanaClient will stop waiting and throw a timeout response instead. This will trigger
	 * response signal too, but with a timeout error.
	 *
	 * @param timeout Timeout in seconds.
	 */
	void set_timeout(float timeout);

	/**
	 * @brief Get the current timeout.
	 *
	 * @return Current timeout in seconds.
	 */
	[[nodiscard]] float get_timeout() const;

	void set_ws_url(const String &url);
	String get_ws_url();

	void set_commitment(const String &commitment);
	String get_commitment();

	void set_encoding(const String &encoding);
	String get_encoding();

	void set_transaction_detail(const String &transaction_detail);

	void enable_min_context_slot(int slot);
	void disable_min_context_slot();

	void enable_account_filter(uint64_t offset, uint64_t length);
	void disable_account_filter();

	void enable_max_transaction_version(uint64_t version);
	void disable_max_transaction_version();

	void enable_rewards();
	void disable_rewards();

	void enable_identity(const String &identity);
	void disable_identity();

	void enable_slot_range(uint64_t first, uint64_t last);
	void disable_slot_range();

	void set_async_override(bool use_async);
	[[nodiscard]] bool get_async_override() const;
	[[nodiscard]] bool is_ready() const;

	void get_account_info(const Variant &account);
	void get_balance(const Variant &account);
	void get_block(uint64_t slot);
	void get_block_height();
	void get_latest_blockhash();
	void get_block_production();
	void get_block_commitment(uint64_t slot_number);
	void get_blocks(uint64_t start_slot, const Variant &end_slot = Variant::NIL);
	void get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot);
	void get_block_time(uint64_t slot);
	void get_cluster_nodes();
	void get_epoch_info();
	void get_epoch_schedule();
	void get_fee_for_message(const String &encoded_message);
	void get_first_available_block();
	void get_genesis_hash();
	void get_health();
	void get_highest_snapshot_slot();
	void get_identity();
	void get_inflation_governor();
	void get_inflation_rate();
	void get_inflation_reward(const Array &accounts, const Variant &epoch = Variant::NIL);
	void get_largest_accounts(const String &filter = "");
	void get_leader_schedule(const Variant &slot = Variant::NIL);
	void get_max_retransmit_slot();
	void get_max_shred_insert_slot();
	void get_minimum_balance_for_rent_extemption(uint64_t data_size);
	void get_multiple_accounts(const Array &accounts);
	void get_program_accounts(const Variant &program_address, const Array &filters = Array(), bool with_context = false);
	void get_recent_performance_samples();
	void get_recent_prioritization_fees(const Array &account_addresses);
	void get_signature_for_address(const Variant &address, const String &before = "", const String &until = "");
	void get_signature_statuses(const PackedStringArray &signatures, bool search_transaction_history = false);
	void get_slot();
	void get_slot_leader();
	void get_slot_leaders(const Variant &start_slot = Variant(), const Variant &slot_limit = Variant());
	void get_stake_activation(const Variant &account);
	void get_stake_minimum_delegation();
	void get_supply(bool exclude_non_circulating = false);
	void get_token_account_balance(const Variant &token_account);
	void get_token_accounts_by_delegate(const Variant &account_delegate, const Variant &mint = nullptr, const Variant &program_id = nullptr);
	void get_token_accounts_by_owner(const Variant &owner, const Variant &mint = nullptr, const Variant &program_id = nullptr);
	void get_token_largest_account(const Variant &token_mint);
	void get_token_supply(const Variant &token_mint);
	void get_transaction(const String &signature);
	void get_transaction_count();
	void get_version();
	void get_vote_accounts(const Variant &vote_pubkey = "", bool keep_unstaked_delinquents = false);
	void is_blockhash_valid(const String &blockhash);
	void minimum_ledger_slot();
	void request_airdrop(const Variant &address, uint64_t lamports);
	void send_transaction(const String &encoded_transaction, uint64_t max_retries = UINT64_MAX, bool skip_preflight = false);
	void simulate_transaction(const String &encoded_transaction, bool sig_verify = false, bool replace_blockhash = false, const Array &account_addresses = Array(), const String &account_encoding = "base64");

	void get_asset(const Variant &asset_id);
	void get_asset_proof(const Variant &asset_id);
	void get_assets_by_authority(const Variant &authority, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);
	void get_assets_by_creator_address(const Variant &creator_address, bool only_verified = false, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);
	void get_assets_by_group(const String &group_key, const Variant &group_value, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);
	void get_assets_by_owner(const Variant &owner, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);

	void account_subscribe(const Variant &account_key, const Callable &callback);
	void signature_subscribe(const String &signature, const Callable &callback, const String &commitment);
	void program_subscribe(const Variant &program_id, const Callable &callback);
	void root_subscribe(const Callable &callback);
	void slot_subscribe(const Callable &callback);

	void unsubscribe_all(const Callable &callback);

	~SolanaClient() override = default;
};
} //namespace godot

#endif