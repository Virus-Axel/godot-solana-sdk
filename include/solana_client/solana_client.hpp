#ifndef SOLANA_CLIENT_SOLANA_CLIENT_HPP
#define SOLANA_CLIENT_SOLANA_CLIENT_HPP
#include <cstdint>
#include <string>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "rpc_multi_http_request_client.hpp"
#include "rpc_single_ws_request_client.hpp"
#include "solana_utils.hpp"

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
	GDCLASS_CUSTOM(SolanaClient, Node)

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

	Callable ws_callback = callable_mp(this, &SolanaClient::ws_response_callback);
	;
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
	/**
	 * @brief @bindmethods{MplCandyMachine, Node}
	 */
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

	/**
	 * @_process
	 */
	void _process(double delta) override;

	/**
	 * @_ready
	 */
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

	/**
	 * @setter{url}
	 */
	void set_ws_url(const String &url);

	/**
	 * @getter{url, String}
	 */
	String get_ws_url();

	/**
	 * @setter{commitment}
	 */
	void set_commitment(const String &commitment);

	/**
	 * @getter{commitment, String}
	 */
	String get_commitment();

	/**
	 * @setter{encoding}
	 */
	void set_encoding(const String &encoding);

	/**
	 * @getter{encoding, String}
	 */
	String get_encoding();

	/**
	 * @setter{transaction_detail}
	 */
	void set_transaction_detail(const String &transaction_detail);

	/**
	 * @brief Enable min context slot for subsequent client queries.
	 *
	 * @param slot Min context slot to use.
	 */
	void enable_min_context_slot(int slot);

	/**
	 * @brief Disable min context slot for subsequent client queries.
	 */
	void disable_min_context_slot();

	/**
	 * @brief Enable account filter for subsequent client queries.
	 *
	 * @param offset Data offset of filter.
	 * @param length Data length of filter.
	 */
	void enable_account_filter(uint64_t offset, uint64_t length);

	/**
	 * @brief Disable account filter for subsequent client queries.
	 */
	void disable_account_filter();

	/**
	 * @brief Enable max transaction version for subsequent client queries.
	 *
	 * @param version Maximum transaction version.
	 */
	void enable_max_transaction_version(uint64_t version);

	/**
	 * @brief Disable max transaction version for subsequent client queries.
	 */
	void disable_max_transaction_version();

	/**
	 * @brief Enable rewards for subsequent client queries.
	 */
	void enable_rewards();

	/**
	 * @brief Disable rewards for subsequent client queries.
	 */
	void disable_rewards();

	/**
	 * @brief Enable identity for subsequent client queries.
	 *
	 * @param identity Identiry to set.
	 */
	void enable_identity(const String &identity);

	/**
	 * @brief Disable identity for subsequent client queries.
	 */
	void disable_identity();

	/**
	 * @brief Enable slot range for subsequent client queries.
	 *
	 * @param first Begining slot in slot range.
	 * @param last End slot in slot range.
	 */
	void enable_slot_range(uint64_t first, uint64_t last);

	/**
	 * @brief Disable slot range for subsequent client queries.
	 */
	void disable_slot_range();

	/**
	 * @setter{async_override, use_async}
	 */
	void set_async_override(bool use_async);

	/**
	 * @getter{async_override, bool}
	 */
	[[nodiscard]] bool get_async_override() const;

	/**
	 * @brief Check if client is ready for sending a request.
	 *
	 * @return true When client is ready to send a new request.
	 * @return false When client is busy.
	 */
	[[nodiscard]] bool is_ready() const;

	/**
	 * @brief RPC request to get account info.
	 *
	 * Returns all information associated with the account of provided Pubkey
	 *
	 * @param account Account key to get information from.
	 */
	void get_account_info(const Variant &account);

	/**
	 * @brief Get the lamport balance of the account of provided Pubkey
	 *
	 * @param account Account key to get balance of.
	 */
	void get_balance(const Variant &account);

	/**
	 * @brief Get identity and transaction information about a confirmed block in the ledger.
	 *
	 * @param slot Slot to get identity and transaction information from.
	 */
	void get_block(uint64_t slot);

	/**
	 * @brief Get the current block height of the node.
	 */
	void get_block_height();

	/**
	 * @brief Get the latest blockhash
	 */
	void get_latest_blockhash();

	/**
	 * @brief Get recent block production information from the current or previous epoch.
	 */
	void get_block_production();

	/**
	 * @brief Get commitment for particular block.
	 *
	 * @param slot_number block to get commitment for.
	 */
	void get_block_commitment(uint64_t slot_number);

	/**
	 * @brief Get a list of confirmed blocks between two slots.
	 *
	 * @param start_slot Start slot of slot range.
	 * @param end_slot End slot of slot range.
	 */
	void get_blocks(uint64_t start_slot, const Variant &end_slot = Variant::NIL);

	/**
	 * @brief Get a list of confirmed blocks starting at the given slot.
	 *
	 * @param start_slot Start slot of slot range.
	 * @param end_slot End slot of slot range.
	 */
	void get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot);

	/**
	 * @brief Get the estimated production time of a block.
	 *
	 * @param slot Block number, identified by slot.
	 */
	void get_block_time(uint64_t slot);

	/**
	 * @brief Get information about all the nodes participating in the cluster.
	 */
	void get_cluster_nodes();

	/**
	 * @brief Get information about the current epoch.
	 */
	void get_epoch_info();

	/**
	 * @brief Get the epoch schedule information from this cluster's genesis config.
	 */
	void get_epoch_schedule();

	/**
	 * @brief Get the fee the network will charge for a particular message.
	 *
	 * @param encoded_message Encoded message.
	 */
	void get_fee_for_message(const String &encoded_message);

	/**
	 * @brief Get the slot of the lowest confirmed block that has not been purged from the ledger.
	 */
	void get_first_available_block();

	/**
	 * @brief Get the genesis hash.
	 */
	void get_genesis_hash();

	/**
	 * @brief Get the current health of the node.
	 *
	 * A healthy node is one that is within HEALTH_CHECK_SLOT_DISTANCE slots of the latest cluster confirmed slot.
	 */
	void get_health();

	/**
	 * @brief Get the highest slot information that the node has snapshots for.
	 *
	 * This will find the highest full snapshot slot, and the highest incremental
	 * snapshot slot based on the full snapshot slot, if there is one.
	 */
	void get_highest_snapshot_slot();

	/**
	 * @brief Get the identity pubkey for the current node.
	 */
	void get_identity();

	/**
	 * @brief Get the current inflation governor.
	 */
	void get_inflation_governor();

	/**
	 * @brief Get the specific inflation values for the current epoch.
	 */
	void get_inflation_rate();

	/**
	 * @brief Get the inflation / staking reward for a list of addresses for an epoch.
	 *
	 * @param accounts Array of addresses.
	 * @param epoch Epoch to get infloation / staking rewards for.
	 */
	void get_inflation_reward(const Array &accounts, const Variant &epoch = Variant::NIL);

	/**
	 * @brief Get the 20 largest accounts, by lamport balance.
	 *
	 * @note results may be cached up to two hours.
	 *
	 * @param filter Filter results by account type.
	 */
	void get_largest_accounts(const String &filter = "");

	/**
	 * @brief Get the leader schedule for an epoch.
	 *
	 * @param slot Slot that epoch corresponds to.
	 */
	void get_leader_schedule(const Variant &slot = Variant::NIL);

	/**
	 * @brief Get the max slot seen from retransmit stage.
	 */
	void get_max_retransmit_slot();

	/**
	 * @brief Get the max slot seen from after shred insert.
	 */
	void get_max_shred_insert_slot();

	/**
	 * @brief Get minimum balance required to make account rent exempt.
	 *
	 * @param data_size Size of account.
	 */
	void get_minimum_balance_for_rent_extemption(uint64_t data_size);

	/**
	 * @brief Get the account information for a list of Pubkeys.
	 *
	 * @param accounts Array of accounts.
	 */
	void get_multiple_accounts(const Array &accounts);

	/**
	 * @brief Get all accounts owned by the provided program Pubkey.
	 *
	 * @param program_address Address of program.
	 * @param filters Array of filters to filter accounts on.
	 * @param with_context Determines if the result will be wrapped in RpcResponse JSON object.
	 */
	void get_program_accounts(const Variant &program_address, const Array &filters = Array(), bool with_context = false);

	/**
	 * @brief Get a list of recent performance samples, in reverse slot order.
	 *
	 * Performance samples are taken every 60 seconds and include the number of
	 * transactions and slots that occur in a given time window.
	 */
	void get_recent_performance_samples();

	/**
	 * @brief Get a list of prioritization fees from recent blocks.
	 *
	 * @param account_addresses Optional account addresses.
	 */
	void get_recent_prioritization_fees(const Array &account_addresses);

	/**
	 * @brief Get signatures for confirmed transactions that include the given address in their accountKeys list.
	 *
	 * @note Returns signatures backwards in time from the provided signature or most recent confirmed block.
	 *
	 * @param address Addresses to get signatures from.
	 * @param before Start searching backwards from this transaction signature.
	 * @param until Search until this transaction signature.
	 */
	void get_signature_for_address(const Variant &address, const String &before = "", const String &until = "");

	/**
	 * @brief Get the statuses of a list of signatures.
	 *
	 * @note Each signature must be a txid, the first signature of a transaction.
	 *
	 * @param signatures Signatures to get statuses from.
	 * @param search_transaction_history Determines if cache is searched only or not.
	 */
	void get_signature_statuses(const PackedStringArray &signatures, bool search_transaction_history = false);

	/**
	 * @brief Get the slot that has reached the given or default commitment level.
	 */
	void get_slot();

	/**
	 * @brief Get the current slot leader.
	 */
	void get_slot_leader();

	/**
	 * @brief Get the slot leaders for a given slot range
	 *
	 * @param start_slot Start slot of slot range.
	 * @param slot_limit End slot of slot range.
	 */
	void get_slot_leaders(const Variant &start_slot = Variant(), const Variant &slot_limit = Variant());

	/**
	 * @brief Get the stake activation for an account
	 *
	 * @deprecated RPC call is not available anymore.
	 *
	 * @param account Account to get stake activation for.
	 */
	void get_stake_activation(const Variant &account);

	/**
	 * @brief Get the stake minimum delegation, in lamports.
	 */
	void get_stake_minimum_delegation();

	/**
	 * @brief Get information about the current supply.
	 *
	 * @param exclude_non_circulating Determines whether non circulating accounts ere excluded from list response
	 */
	void get_supply(bool exclude_non_circulating = false);

	/**
	 * @brief Get the token balance of an SPL Token account.
	 *
	 * @param token_account Token account to get token balance of.
	 */
	void get_token_account_balance(const Variant &token_account);

	/**
	 * @brief Get all SPL Token accounts by approved Delegate.
	 *
	 * @param account_delegate Account delegate key.
	 * @param mint Mint token.
	 * @param program_id Program ID.
	 */
	void get_token_accounts_by_delegate(const Variant &account_delegate, const Variant &mint = nullptr, const Variant &program_id = nullptr);

	/**
	 * @brief all SPL Token accounts by token owner.
	 *
	 * @param owner Account owner.
	 * @param mint Specific token Mint to limit accounts to.
	 * @param program_id Token program that owns the accounts.
	 */
	void get_token_accounts_by_owner(const Variant &owner, const Variant &mint = nullptr, const Variant &program_id = nullptr);

	/**
	 * @brief Get the 20 largest accounts of a particular SPL Token type.
	 *
	 * @param token_mint Token Mint to get largest accounts for.
	 */
	void get_token_largest_account(const Variant &token_mint);

	/**
	 * @brief Get the total supply of an SPL Token type.
	 *
	 * @param token_mint Token mint to get supply of.
	 */
	void get_token_supply(const Variant &token_mint);

	/**
	 * @brief Get transaction details for a confirmed transaction
	 *
	 * @param signature Signature of transaction to get.
	 */
	void get_transaction(const String &signature);

	/**
	 * @brief Get the current Transaction count from the ledger.
	 */
	void get_transaction_count();

	/**
	 * @brief Get the current Solana version running on the node.
	 */
	void get_version();

	/**
	 * @brief Get the account info and associated stake for all the voting accounts in the current bank.
	 *
	 * @param vote_pubkey Only return results for this validator vote address.
	 * @param keep_unstaked_delinquents Determines whether or not to filter out delinquent validators with no stake.
	 */
	void get_vote_accounts(const Variant &vote_pubkey = "", bool keep_unstaked_delinquents = false);

	/**
	 * @brief Finds out whether a blockhash is still valid or not.
	 *
	 * @param blockhash The blockhash of the block to evaluate.
	 */
	void is_blockhash_valid(const String &blockhash);

	/**
	 * @brief Get the lowest slot that the node has information about in its ledger.
	 */
	void minimum_ledger_slot();

	/**
	 * @brief Requests an airdrop of lamports to a Pubkey.
	 *
	 * @param address Pubkey of account to receive lamports.
	 * @param lamports Amount of lamports to airdrop.
	 */
	void request_airdrop(const Variant &address, uint64_t lamports);

	/**
	 * @brief Submit a signed transaction to the cluster for processing.
	 *
	 * If the node's rpc service receives the transaction, this method immediately succeeds,
	 * without waiting for any confirmations. A successful response from this method does not
	 * guarantee the transaction is processed or confirmed by the cluster.
	 *
	 * @param encoded_transaction Signed and encoded serialized transaction.
	 * @param max_retries Maximum number of times for the RPC node to retry
	 * sending the transaction to the leader. If this parameter not provided,
	 * the RPC node will retry the transaction until it is finalized or until
	 * the blockhash expires.
	 * @param skip_preflight Determines whether or not to skip the preflight transaction checks.
	 */
	void send_transaction(const String &encoded_transaction, uint64_t max_retries = UINT64_MAX, bool skip_preflight = false);

	/**
	 * @brief Simulate sending a transaction.
	 *
	 * @param encoded_transaction Signed and encoded serialized transaction.
	 * @param sig_verify If true the transaction signatures will be verified (conflicts with replaceRecentBlockhash).
	 * @param replace_blockhash If true the transaction recent blockhash will be replaced with the most recent blockhash (conflicts with sigVerify).
	 * @param account_addresses Array of accounts to return in result.
	 * @param account_encoding Encoding of accounts.
	 */
	void simulate_transaction(const String &encoded_transaction, bool sig_verify = false, bool replace_blockhash = false, const Array &account_addresses = Array(), const String &account_encoding = "base64");

	/**
	 * @brief Get the information of a compressed/standard asset including metadata and owner.
	 *
	 * @param asset_id The id of the asset.
	 */
	void get_asset(const Variant &asset_id);

	/**
	 * @brief Get the merkle tree proof information for a compressed asset.
	 *
	 * @param asset_id 	The id of the asset.
	 */
	void get_asset_proof(const Variant &asset_id);

	/**
	 * @brief Get the list of assets given an authority address.
	 *
	 * @param authority The address of the authority of the assets.
	 * @param page The index of the "page" to retrieve.
	 * @param limit The maximum number of assets to retrieve.
	 */
	void get_assets_by_authority(const Variant &authority, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);

	/**
	 * @brief Get the list of assets given a creator address.
	 *
	 * @warning Metaplex recommends to fetch data with onlyVerified: true to make sure the asset actually belongs to that creator.
	 *
	 * @param creator_address The address of the creator of the assets.
	 * @param only_verified Indicates whether to retrieve only verified assets or not.
	 * @param page The index of the "page" to retrieve.
	 * @param limit The maximum number of assets to retrieve.
	 */
	void get_assets_by_creator_address(const Variant &creator_address, bool only_verified = false, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);

	/**
	 * @brief Get the list of assets given a group (key, value) pair.
	 *
	 * @param group_key The key of the group (e.g., "collection").
	 * @param group_value The value of the group.
	 * @param page The index of the "page" to retrieve.
	 * @param limit The maximum number of assets to retrieve.
	 */
	void get_assets_by_group(const String &group_key, const Variant &group_value, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT);

	/**
	 * @brief Get the list of assets given an owner address.
	 *
	 * @param owner The address of the owner of the assets.
	 * @param page The index of the "page" to retrieve.
	 * @param limit The maximum number of assets to retrieve.
	 * @param show_fungible Includes fungible assets if true.
	 */
	void get_assets_by_owner(const Variant &owner, uint32_t page = 1, uint32_t limit = ASSET_DEFAULT_FETCH_LIMIT, bool show_fungible = false);

	/**
	 * @brief Subscribe to an account to receive notifications when the lamports or data for a given account public key changes.
	 *
	 * @param account_key Account to subscribe to.
	 * @param callback Callback to be called on notifications.
	 */
	void account_subscribe(const Variant &account_key, const Callable &callback);

	/**
	 * @brief Subscribe to receive a notification when the transaction with the given signature reaches the specified commitment level.
	 *
	 * @param signature Signature of transaction to subscribe to.
	 * @param callback Callback to be called when transaction commitment changes.
	 * @param commitment How finalized a block is at that point in time.
	 */
	void signature_subscribe(const String &signature, const Callable &callback, const String &commitment);

	/**
	 * @brief Subscribe to a program to receive notifications when the lamports or data for an account owned by the given program changes.
	 *
	 * @param program_id Program ID to subscribe to.
	 * @param callback Callable to be called on a change.
	 */
	void program_subscribe(const Variant &program_id, const Callable &callback);

	/**
	 * @brief Subscribe to receive notification anytime a new root is set by the validator.
	 *
	 * @param callback Callback to be called when a new root is set.
	 */
	void root_subscribe(const Callable &callback);

	/**
	 * @brief Subscribe to receive notification anytime a slot is processed by the validator.
	 *
	 * @param callback Callback to be called when a slot is processed.
	 */
	void slot_subscribe(const Callable &callback);

	/**
	 * @brief Unsubscribe all subscriptions.
	 *
	 * @param callback
	 */
	void unsubscribe_all(const Callable &callback);

	~SolanaClient() override = default;
};
} //namespace godot

#endif