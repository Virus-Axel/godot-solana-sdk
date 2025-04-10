#include "solana_client.hpp"

#include <cstdint>

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/java_script_bridge.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "pubkey.hpp"
#include "rpc_multi_http_request_client.hpp"
#include "rpc_single_ws_request_client.hpp"
#include "solana_utils.hpp"

namespace godot {

unsigned int SolanaClient::global_rpc_id = 1;

String SolanaClient::ws_from_http(const String &http_url) {
	Dictionary parsed_url = parse_url(http_url);
	// See if tls should be used.
	use_tls = true;
	if (!parsed_url.has("scheme")) {
	} else if (parsed_url["scheme"] == "http") {
		use_tls = false;
	}

	Dictionary ws_url = parsed_url;
	if (use_tls) {
		ws_url["scheme"] = "wss";
	} else {
		ws_url["scheme"] = "ws";
	}
	ws_url["port"] = get_real_ws_port();
	return assemble_url(ws_url);
}

String SolanaClient::get_real_url() {
	if (url_override.is_empty()) {
		return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_url");
	}
	return url_override;
}

uint32_t SolanaClient::get_real_http_port() const {
	if (http_port_override == 0) {
		return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_http_port");
	}
	return http_port_override;
}

uint32_t SolanaClient::get_real_ws_port() const {
	if (ws_port_override == 0) {
		return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_ws_port");
	}
	return ws_port_override;
}

String SolanaClient::get_real_ws_url() {
	if (ws_url.is_empty()) {
		return ws_from_http(get_real_url());
	}
	return ws_url;
}

PackedStringArray SolanaClient::get_address_strings(const Array &keys) {
	PackedStringArray account_strings;
	for (unsigned int i = 0; i < keys.size(); i++) {
		account_strings.append(Pubkey::string_from_variant(keys[i]));
	}
	return account_strings;
}

RpcSingleWsRequestClient *SolanaClient::get_current_ws_client() {
	Object *ptr = Engine::get_singleton()->get_singleton("ws_client");
	return Object::cast_to<RpcSingleWsRequestClient>(ptr);
}

RpcMultiHttpRequestClient *SolanaClient::get_current_http_client() {
	Object *ptr = Engine::get_singleton()->get_singleton("http_client");
	return Object::cast_to<RpcMultiHttpRequestClient>(ptr);
}

void SolanaClient::append_commitment(Array &options) {
	if (!commitment.is_empty()) {
		add_to_param_dict(options, "commitment", commitment);
	}
}

void SolanaClient::append_min_context_slot(Array &options) const {
	if (min_constext_slot_enabled) {
		add_to_param_dict(options, "minContextSlot", min_context_slot);
	}
}

void SolanaClient::append_encoding(Array &options) {
	if (!encoding.is_empty()) {
		add_to_param_dict(options, "encoding", encoding);
	}
}

void SolanaClient::append_transaction_detail(Array &options) {
	add_to_param_dict(options, "transactionDetails", String(SolanaClient::transaction_detail.c_str()));
}

void SolanaClient::append_account_filter(Array &options) const {
	if (account_filter_enabled) {
		Dictionary filter;
		filter["offset"] = filter_offset;
		filter["length"] = filter_length;
		add_to_param_dict(options, "dataSlice", filter);
	}
}

void SolanaClient::append_data_filter(Array &options, const Array &filters) {
	if (!filters.is_empty()) {
		add_to_param_dict(options, "filters", filters);
	}
}

void SolanaClient::append_max_transaction_version(Array &options) const {
	if (max_transaction_version_enabled) {
		add_to_param_dict(options, "maxSupportedTransactionVersion", max_transaction_version);
	}
}

void SolanaClient::append_rewards(Array &options) const {
	add_to_param_dict(options, "rewards", rewards);
}

void SolanaClient::append_identity(Array &options) {
	if (!identity.empty()) {
		add_to_param_dict(options, "identity", String(identity.c_str()));
	}
}

void SolanaClient::append_slot_range(Array &options) const {
	if (slot_range_enabled) {
		Dictionary range;
		range["first"] = first_slot;
		range["last"] = last_slot;
		add_to_param_dict(options, "range", range);
	}
}

void SolanaClient::append_limit(Array &options) {
}

void SolanaClient::add_to_param_dict(Array &options, const String &key, const Variant &value) {
	if (options.is_empty()) {
		options.append(make_rpc_param(key, value));
		return;
	}
	if (options.back().get_type() == Variant::DICTIONARY) {
		Dictionary dict = options.back();
		dict[key] = value;
	} else {
		options.append(make_rpc_param(key, value));
	}
}

Dictionary SolanaClient::make_rpc_dict(const String &method, const Array &params) {
	Dictionary request_body;

	request_body["id"] = global_rpc_id;
	SolanaClient::global_rpc_id += 1;

	request_body["jsonrpc"] = "2.0";
	request_body["method"] = method;
	request_body["params"] = params;

	return request_body;
}

Dictionary SolanaClient::make_rpc_param(const Variant &key, const Variant &value) {
	Dictionary result;
	result[key] = value;
	return result;
}

void SolanaClient::quick_http_request(const Dictionary &request_body) {
	Dictionary parsed_url = parse_url(get_real_url());
	const uint32_t real_port = get_real_http_port();
	// Check if port is set in URL.
	if (parsed_url.has("port")) {
		if (real_port != static_cast<uint32_t>(parsed_url["port"])) {
			WARN_PRINT_ONCE_ED_CUSTOM("Port is specified in URL and is not the same as port setting, ignoring port setting.");
		}
	} else {
		parsed_url["port"] = real_port;
	}

	if (is_inside_tree() || async_override) {
		get_current_http_client()->asynchronous_request(request_body, parsed_url, rpc_callback);
	} else {
		ERR_FAIL_EDMSG_CUSTOM("SolanaClient must be in scene tree.");
	}
}

void SolanaClient::response_callback(const Dictionary &params) {
	pending_request = false;
	emit_signal("http_response_received", params);
}

void SolanaClient::ws_response_callback(const Dictionary &params) {
	emit_signal("socket_response_received", params);
}

void SolanaClient::get_latest_blockhash() {
	Array params;
	append_commitment(params);

	quick_http_request(make_rpc_dict("getLatestBlockhash", params));
}

void SolanaClient::get_block_production() {
	Array params;
	append_commitment(params);
	append_identity(params);
	append_slot_range(params);

	quick_http_request(make_rpc_dict("getBlockProduction", params));
}

void SolanaClient::get_account_info(const Variant &account) {
	Array params;
	params.append(Pubkey::string_from_variant(account));
	append_commitment(params);
	append_encoding(params);
	append_min_context_slot(params);
	append_account_filter(params);

	quick_http_request(make_rpc_dict("getAccountInfo", params));
}

void SolanaClient::get_balance(const Variant &account) {
	Array params;
	params.append(Pubkey::string_from_variant(account));
	append_commitment(params);
	append_encoding(params);
	append_min_context_slot(params);
	append_account_filter(params);

	quick_http_request(make_rpc_dict("getBalance", params));
}

void SolanaClient::get_block(uint64_t slot) {
	Array params;
	params.append(slot);
	append_commitment(params);
	append_encoding(params);
	append_transaction_detail(params);
	append_max_transaction_version(params);
	append_rewards(params);

	quick_http_request(make_rpc_dict("getBlock", params));
}

void SolanaClient::get_block_height() {
	Array params;
	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getBlockHeight", params));
}

void SolanaClient::get_block_commitment(uint64_t slot_number) {
	Array params;
	params.append(slot_number);

	quick_http_request(make_rpc_dict("getBlockCommitment", params));
}

void SolanaClient::get_blocks(uint64_t start_slot, const Variant &end_slot) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(start_slot);
	if (end_slot.get_type() == Variant::INT) {
		params.append(end_slot);
	}
	append_commitment(params);

	quick_http_request(make_rpc_dict("getBlocks", params));
}

void SolanaClient::get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(start_slot);
	params.append(end_slot);
	append_commitment(params);

	quick_http_request(make_rpc_dict("getBlocksWithLimit", params));
}

void SolanaClient::get_block_time(uint64_t slot) {
	Array params;
	params.append(slot);

	quick_http_request(make_rpc_dict("getBlockTime", params));
}

void SolanaClient::get_cluster_nodes() {
	quick_http_request(make_rpc_dict("getClusterNodes", Array()));
}

void SolanaClient::get_epoch_info() {
	Array params;
	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getEpochInfo", params));
}

void SolanaClient::get_epoch_schedule() {
	quick_http_request(make_rpc_dict("getEpochSchedule", Array()));
}

void SolanaClient::get_fee_for_message(const String &encoded_message) {
	Array params;
	params.append(encoded_message);
	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getFeeForMessage", params));
}

void SolanaClient::get_first_available_block() {
	quick_http_request(make_rpc_dict("getFirstAvailableBlock", Array()));
}

void SolanaClient::get_genesis_hash() {
	quick_http_request(make_rpc_dict("getGenesisHash", Array()));
}

void SolanaClient::get_health() {
	quick_http_request(make_rpc_dict("getHealth", Array()));
}

void SolanaClient::get_highest_snapshot_slot() {
	quick_http_request(make_rpc_dict("getHighestSnapshotSlot", Array()));
}

void SolanaClient::get_identity() {
	quick_http_request(make_rpc_dict("getIdentity", Array()));
}

void SolanaClient::get_inflation_governor() {
	Array params;
	append_commitment(params);

	quick_http_request(make_rpc_dict("getInflationGovernor", params));
}

void SolanaClient::get_inflation_rate() {
	quick_http_request(make_rpc_dict("getInflationRate", Array()));
}

void SolanaClient::get_inflation_reward(const Array &accounts, const Variant &epoch) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;

	const PackedStringArray account_strings = get_address_strings(accounts);

	params.append(account_strings);
	append_commitment(params);
	if (epoch.get_type() == Variant::INT) {
		add_to_param_dict(params, "epoch", epoch);
	}
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getInflationReward", params));
}

void SolanaClient::get_largest_accounts(const String &filter) {
	Array params;
	append_commitment(params);
	if (!filter.is_empty()) {
		add_to_param_dict(params, "filter", filter);
	}

	quick_http_request(make_rpc_dict("getLargestAccounts", params));
}

void SolanaClient::get_leader_schedule(const Variant &slot) {
	Array params;
	params.append(slot);
	append_commitment(params);
	append_identity(params);

	quick_http_request(make_rpc_dict("getLeaderSchedule", params));
}

void SolanaClient::get_max_retransmit_slot() {
	quick_http_request(make_rpc_dict("getMaxRetransmitSlot", Array()));
}

void SolanaClient::get_max_shred_insert_slot() {
	quick_http_request(make_rpc_dict("getMaxShredInsertSlot", Array()));
}

void SolanaClient::get_minimum_balance_for_rent_extemption(uint64_t data_size) {
	Array params;
	params.append(data_size);
	append_commitment(params);

	quick_http_request(make_rpc_dict("getMinimumBalanceForRentExemption", params));
}

void SolanaClient::get_multiple_accounts(const Array &accounts) {
	Array params;
	params.append(get_address_strings(accounts));
	append_commitment(params);
	append_min_context_slot(params);
	append_account_filter(params);
	append_encoding(params);

	quick_http_request(make_rpc_dict("getMultipleAccounts", params));
}

void SolanaClient::get_program_accounts(const Variant &program_address, const Array &filters, bool with_context) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(Pubkey::string_from_variant(program_address));
	append_commitment(params);
	append_min_context_slot(params);
	add_to_param_dict(params, "withContext", with_context);
	append_encoding(params);
	append_account_filter(params);
	append_data_filter(params, filters);
	quick_http_request(make_rpc_dict("getProgramAccounts", params));
}

void SolanaClient::get_recent_performance_samples() {
	Array params;
	append_limit(params);

	quick_http_request(make_rpc_dict("getRecentPerformanceSamples", params));
}

void SolanaClient::get_recent_prioritization_fees(const Array &account_addresses) {
	Array params;
	if (!account_addresses.is_empty()) {
		params.append(get_address_strings(account_addresses));
	}

	quick_http_request(make_rpc_dict("getRecentPrioritizationFees", params));
}

void SolanaClient::get_signature_for_address(const Variant &address, const String &before, const String &until) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(Pubkey::string_from_variant(address));
	append_commitment(params);
	append_min_context_slot(params);
	append_limit(params);
	if (!before.is_empty()) {
		add_to_param_dict(params, "before", before);
	}
	if (!until.is_empty()) {
		add_to_param_dict(params, "until", until);
	}

	quick_http_request(make_rpc_dict("getSignaturesForAddress", params));
}

void SolanaClient::get_signature_statuses(const PackedStringArray &signatures, bool search_transaction_history) {
	Array params;

	params.append(signatures);
	add_to_param_dict(params, "searchTransactionHistory", search_transaction_history);

	quick_http_request(make_rpc_dict("getSignatureStatuses", params));
}

void SolanaClient::get_slot() {
	Array params;

	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getSlot", params));
}

void SolanaClient::get_slot_leader() {
	Array params;

	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getSlotLeader", params));
}

void SolanaClient::get_slot_leaders(const Variant &start_slot, const Variant &slot_limit) {
	Array params;

	if (start_slot.get_type() == Variant::NIL) {
		params.append(start_slot);
	}
	if (slot_limit.get_type() == Variant::NIL) {
		params.append(slot_limit);
	}

	quick_http_request(make_rpc_dict("getSlotLeaders", params));
}

void SolanaClient::get_stake_activation(const Variant &account) {
	Array params;

	params.append(Pubkey::string_from_variant(account));
	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getStakeActivation", params));
}

void SolanaClient::get_stake_minimum_delegation() {
	Array params;

	append_commitment(params);

	quick_http_request(make_rpc_dict("getStakeMinimumDelegation", params));
}

void SolanaClient::get_supply(bool exclude_non_circulating) {
	Array params;

	append_commitment(params);
	add_to_param_dict(params, "excludeNonCirculatingAccountsList", exclude_non_circulating);

	quick_http_request(make_rpc_dict("getSupply", params));
}

void SolanaClient::get_token_account_balance(const Variant &token_account) {
	Array params;

	params.append(Pubkey::string_from_variant(token_account));
	append_commitment(params);

	quick_http_request(make_rpc_dict("getTokenAccountBalance", params));
}

void SolanaClient::get_token_accounts_by_delegate(const Variant &account_delegate, const Variant &mint, const Variant &program_id) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(Pubkey::string_from_variant(account_delegate));

	Dictionary dict_argument;

	if (mint.get_type() != Variant::NIL) {
		dict_argument["mint"] = Pubkey::string_from_variant(mint);
	} else if (program_id.get_type() != Variant::NIL) {
		dict_argument["programId"] = Pubkey::string_from_variant(program_id);
	}

	append_commitment(params);
	append_min_context_slot(params);
	append_account_filter(params);
	append_encoding(params);

	if (!dict_argument.is_empty()) {
		params.insert(0, dict_argument);
	}

	quick_http_request(make_rpc_dict("getTokenAccountsByDelegate", params));
}

void SolanaClient::get_token_accounts_by_owner(const Variant &owner, const Variant &mint, const Variant &program_id) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	Dictionary dict_argument;

	params.append(Pubkey::string_from_variant(owner));

	if (mint.get_type() != Variant::NIL) {
		dict_argument["mint"] = Pubkey::string_from_variant(mint);
	} else if (program_id.get_type() != Variant::NIL) {
		dict_argument["programId"] = Pubkey::string_from_variant(program_id);
	}

	append_commitment(params);
	append_min_context_slot(params);
	append_account_filter(params);
	append_encoding(params);

	if (!dict_argument.is_empty()) {
		params.insert(1, dict_argument);
	}

	quick_http_request(make_rpc_dict("getTokenAccountsByOwner", params));
}

void SolanaClient::get_token_largest_account(const Variant &token_mint) {
	Array params;

	params.append(Pubkey::string_from_variant(token_mint));
	append_commitment(params);

	quick_http_request(make_rpc_dict("getTokenLargestAccounts", params));
}

void SolanaClient::get_token_supply(const Variant &token_mint) {
	Array params;

	params.append(Pubkey::string_from_variant(token_mint));
	append_commitment(params);

	quick_http_request(make_rpc_dict("getTokenSupply", params));
}

void SolanaClient::get_transaction(const String &signature) {
	Array params;

	params.append(signature);
	append_commitment(params);
	append_max_transaction_version(params);
	append_encoding(params);

	quick_http_request(make_rpc_dict("getTransaction", params));
}

void SolanaClient::get_transaction_count() {
	Array params;

	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("getTransactionCount", params));
}

void SolanaClient::get_version() {
	quick_http_request(make_rpc_dict("getVersion", Array()));
}

void SolanaClient::get_vote_accounts(const Variant &vote_pubkey, bool keep_unstaked_delinquents) {
	Array params;

	append_commitment(params);
	if (vote_pubkey.get_type() != Variant::NIL) {
		add_to_param_dict(params, "votePubkey", Pubkey::string_from_variant(vote_pubkey));
	}
	add_to_param_dict(params, "keepUnstakedDelinquents", keep_unstaked_delinquents);

	quick_http_request(make_rpc_dict("getVoteAccounts", params));
}

void SolanaClient::is_blockhash_valid(const String &blockhash) {
	Array params;

	params.append(blockhash);
	append_commitment(params);
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("isBlockhashValid", params));
}

void SolanaClient::minimum_ledger_slot() {
	quick_http_request(make_rpc_dict("minimumLedgerSlot", Array()));
}

void SolanaClient::request_airdrop(const Variant &address, uint64_t lamports) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;

	params.append(Pubkey::string_from_variant(address));
	params.append(lamports);
	append_commitment(params);

	quick_http_request(make_rpc_dict("requestAirdrop", params));
}

void SolanaClient::send_transaction(const String &encoded_transaction, uint64_t max_retries, bool skip_preflight) {
	Array params;

	params.append(encoded_transaction);
	append_encoding(params);
	add_to_param_dict(params, "skipPreflight", skip_preflight);
	add_to_param_dict(params, "preflightCommitment", commitment);
	if (max_retries != UINT64_MAX) {
		add_to_param_dict(params, "maxRetries", max_retries);
	}
	append_min_context_slot(params);

	quick_http_request(make_rpc_dict("sendTransaction", params));
}

void SolanaClient::simulate_transaction(const String &encoded_transaction, bool sig_verify, bool replace_blockhash, const Array &account_addresses, const String &account_encoding) {
	Array params;

	params.append(encoded_transaction);
	append_commitment(params);

	add_to_param_dict(params, "sigVerify", sig_verify);
	add_to_param_dict(params, "replaceRecentBlockhash", replace_blockhash);
	append_min_context_slot(params);
	append_encoding(params);

	if (!account_addresses.is_empty()) {
		Dictionary accounts;
		accounts["addresses"] = get_address_strings(account_addresses);
		accounts["encoding"] = account_encoding;
		add_to_param_dict(params, "accounts", accounts);
	}

	quick_http_request(make_rpc_dict("simulateTransaction", params));
}

void SolanaClient::get_asset(const Variant &asset_id) {
	Array params;

	params.append(Pubkey::string_from_variant(asset_id));

	quick_http_request(make_rpc_dict("getAsset", params));
}

void SolanaClient::get_asset_proof(const Variant &asset_id) {
	Array params;
	params.append(Pubkey::string_from_variant(asset_id));

	quick_http_request(make_rpc_dict("getAssetProof", params));
}

void SolanaClient::get_assets_by_authority(const Variant &authority, uint32_t page, uint32_t limit) { // NOLINT(bugprone-easily-swappable-parameters)
	Dictionary param_dict;
	param_dict["authorityAddress"] = Pubkey::string_from_variant(authority);
	param_dict["page"] = page;
	param_dict["limit"] = limit;

	Dictionary rpc_dict = make_rpc_dict("getAssetsByAuthority", Array());
	rpc_dict["params"] = param_dict;

	quick_http_request(rpc_dict);
}

void SolanaClient::get_assets_by_creator_address(const Variant &creator_address, bool only_verified, uint32_t page, uint32_t limit) { // NOLINT(bugprone-easily-swappable-parameters)
	Dictionary params_dict;
	params_dict["creatorAddress"] = Pubkey::string_from_variant(creator_address);
	params_dict["onlyVerified"] = only_verified;
	params_dict["page"] = page;
	params_dict["limit"] = limit;

	Dictionary rpc_dict = make_rpc_dict("getAssetsByCreator", Array());
	rpc_dict["params"] = params_dict;
	quick_http_request(rpc_dict);
}

void SolanaClient::get_assets_by_group(const String &group_key, const Variant &group_value, uint32_t page, uint32_t limit) { // NOLINT(bugprone-easily-swappable-parameters)
	Dictionary params_dict;
	params_dict["groupKey"] = group_key;
	params_dict["groupValue"] = Pubkey::string_from_variant(group_value);
	params_dict["page"] = page;
	params_dict["limit"] = limit;

	Dictionary rpc_dict = make_rpc_dict("getAssetsByGroup", Array());
	rpc_dict["params"] = params_dict;
	quick_http_request(rpc_dict);
}

void SolanaClient::get_assets_by_owner(const Variant &owner, uint32_t page, uint32_t limit) { // NOLINT(bugprone-easily-swappable-parameters)
	Dictionary param_dict;
	param_dict["ownerAddress"] = Pubkey::string_from_variant(owner);
	param_dict["page"] = page;
	param_dict["limit"] = limit;

	Dictionary rpc_dict = make_rpc_dict("getAssetsByOwner", Array());
	rpc_dict["params"] = param_dict;
	quick_http_request(rpc_dict);
}

void SolanaClient::account_subscribe(const Variant &account_key, const Callable &callback) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(Pubkey::string_from_variant(account_key));
	add_to_param_dict(params, "encoding", encoding);
	add_to_param_dict(params, "commitment", commitment);

	get_current_ws_client()->enqueue_ws_request(make_rpc_dict("accountSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::signature_subscribe(const String &signature, const Callable &callback, const String &commitment) {
	Array params;
	params.append(signature);
	add_to_param_dict(params, "commitment", commitment);

	get_current_ws_client()->enqueue_ws_request(make_rpc_dict("signatureSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::program_subscribe(const Variant &program_id, const Callable &callback) { // NOLINT(bugprone-easily-swappable-parameters)
	Array params;
	params.append(Pubkey::string_from_variant(program_id));
	append_commitment(params);
	append_account_filter(params);
	append_encoding(params);

	get_current_ws_client()->enqueue_ws_request(make_rpc_dict("programSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::root_subscribe(const Callable &callback) {
	get_current_ws_client()->enqueue_ws_request(make_rpc_dict("rootSubscribe", Array()), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::slot_subscribe(const Callable &callback) {
	get_current_ws_client()->enqueue_ws_request(make_rpc_dict("slotSubscribe", Array()), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::unsubscribe_all(const Callable &callback) {
	get_current_ws_client()->unsubscribe_all(callback, parse_url(get_real_ws_url()));
}

void SolanaClient::_bind_methods() {
	ClassDB::add_signal("SolanaClient", MethodInfo("socket_response_received"));
	ClassDB::add_signal("SolanaClient", MethodInfo("http_response_received", PropertyInfo(Variant::DICTIONARY, "response")));

	ClassDB::bind_static_method("SolanaClient", D_METHOD("assemble_url", "url"), &SolanaClient::assemble_url);
	ClassDB::bind_static_method("SolanaClient", D_METHOD("parse_url", "url"), &SolanaClient::parse_url);
	ClassDB::bind_static_method("SolanaClient", D_METHOD("get_next_request_identifier"), &SolanaClient::get_next_request_identifier);

	ClassDB::bind_method(D_METHOD("set_url_override", "url_override"), &SolanaClient::set_url_override);
	ClassDB::bind_method(D_METHOD("get_url_override"), &SolanaClient::get_url_override);
	ClassDB::bind_method(D_METHOD("is_ready"), &SolanaClient::is_ready);

	ClassDB::bind_method(D_METHOD("set_ws_url", "url"), &SolanaClient::set_ws_url);
	ClassDB::bind_method(D_METHOD("get_ws_url"), &SolanaClient::get_ws_url);

	ClassDB::bind_method(D_METHOD("set_timeout", "timeout"), &SolanaClient::set_timeout);
	ClassDB::bind_method(D_METHOD("get_timeout"), &SolanaClient::get_timeout);

	ClassDB::bind_method(D_METHOD("set_encoding", "encoding"), &SolanaClient::set_encoding);
	ClassDB::bind_method(D_METHOD("get_encoding"), &SolanaClient::get_encoding);

	ClassDB::bind_method(D_METHOD("set_commitment", "commitment"), &SolanaClient::set_commitment);
	ClassDB::bind_method(D_METHOD("get_commitment"), &SolanaClient::get_commitment);

	ClassDB::bind_method(D_METHOD("set_transaction_detail", "transaction_detail"), &SolanaClient::set_transaction_detail);
	ClassDB::bind_method(D_METHOD("enable_min_context_slot", "slot"), &SolanaClient::enable_min_context_slot);
	ClassDB::bind_method(D_METHOD("disable_min_context_slot"), &SolanaClient::disable_min_context_slot);
	ClassDB::bind_method(D_METHOD("enable_account_filter", "offset", "length"), &SolanaClient::enable_account_filter);
	ClassDB::bind_method(D_METHOD("disable_account_filter"), &SolanaClient::disable_account_filter);
	ClassDB::bind_method(D_METHOD("enable_max_transaction_version", "version"), &SolanaClient::enable_max_transaction_version);
	ClassDB::bind_method(D_METHOD("disable_max_transaction_version"), &SolanaClient::disable_max_transaction_version);
	ClassDB::bind_method(D_METHOD("enable_rewards"), &SolanaClient::enable_rewards);
	ClassDB::bind_method(D_METHOD("disable_rewards"), &SolanaClient::disable_rewards);
	ClassDB::bind_method(D_METHOD("enable_identity", "identity"), &SolanaClient::enable_identity);
	ClassDB::bind_method(D_METHOD("disable_identity"), &SolanaClient::disable_identity);
	ClassDB::bind_method(D_METHOD("enable_slot_range", "first", "last"), &SolanaClient::enable_slot_range);
	ClassDB::bind_method(D_METHOD("disable_slot_range"), &SolanaClient::disable_slot_range);

	ClassDB::bind_method(D_METHOD("get_latest_blockhash"), &SolanaClient::get_latest_blockhash);
	ClassDB::bind_method(D_METHOD("get_balance", "account"), &SolanaClient::get_balance);
	ClassDB::bind_method(D_METHOD("get_account_info", "account"), &SolanaClient::get_account_info);
	ClassDB::bind_method(D_METHOD("get_block", "slot"), &SolanaClient::get_block);
	ClassDB::bind_method(D_METHOD("get_block_height"), &SolanaClient::get_block_height);
	ClassDB::bind_method(D_METHOD("get_block_commitment", "slot"), &SolanaClient::get_block_commitment);
	ClassDB::bind_method(D_METHOD("get_blocks", "start_slot", "end_slot"), &SolanaClient::get_blocks);
	ClassDB::bind_method(D_METHOD("get_blocks_with_limit", "start_slot", "end_slot"), &SolanaClient::get_blocks_with_limit);
	ClassDB::bind_method(D_METHOD("get_block_time", "slot"), &SolanaClient::get_block_time);
	ClassDB::bind_method(D_METHOD("get_cluster_nodes"), &SolanaClient::get_cluster_nodes);
	ClassDB::bind_method(D_METHOD("get_epoch_info"), &SolanaClient::get_epoch_info);
	ClassDB::bind_method(D_METHOD("get_epoch_schedule"), &SolanaClient::get_epoch_schedule);
	ClassDB::bind_method(D_METHOD("get_fee_for_message", "encoded_message"), &SolanaClient::get_fee_for_message);
	ClassDB::bind_method(D_METHOD("get_first_available_block"), &SolanaClient::get_first_available_block);
	ClassDB::bind_method(D_METHOD("get_genesis_hash"), &SolanaClient::get_genesis_hash);
	ClassDB::bind_method(D_METHOD("get_health"), &SolanaClient::get_health);
	ClassDB::bind_method(D_METHOD("get_highest_snapshot_slot"), &SolanaClient::get_highest_snapshot_slot);
	ClassDB::bind_method(D_METHOD("get_identity"), &SolanaClient::get_identity);
	ClassDB::bind_method(D_METHOD("get_inflation_governor"), &SolanaClient::get_inflation_governor);
	ClassDB::bind_method(D_METHOD("get_inflation_rate"), &SolanaClient::get_inflation_rate);
	ClassDB::bind_method(D_METHOD("get_inflation_reward", "accounts", "epoch"), &SolanaClient::get_inflation_reward);
	ClassDB::bind_method(D_METHOD("get_largest_accounts", "filter"), &SolanaClient::get_largest_accounts);
	ClassDB::bind_method(D_METHOD("get_leader_schedule", "slot"), &SolanaClient::get_leader_schedule);
	ClassDB::bind_method(D_METHOD("get_max_retransmit_slot"), &SolanaClient::get_max_retransmit_slot);
	ClassDB::bind_method(D_METHOD("get_max_shred_insert_slot"), &SolanaClient::get_max_shred_insert_slot);
	ClassDB::bind_method(D_METHOD("get_minimum_balance_for_rent_extemption", "data_size"), &SolanaClient::get_minimum_balance_for_rent_extemption);
	ClassDB::bind_method(D_METHOD("get_multiple_accounts", "accounts"), &SolanaClient::get_multiple_accounts);
	ClassDB::bind_method(D_METHOD("get_program_accounts", "program_address", "filters", "with_context"), &SolanaClient::get_program_accounts, DEFVAL(Array()), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_recent_performance_samples"), &SolanaClient::get_recent_performance_samples);
	ClassDB::bind_method(D_METHOD("get_recent_prioritization_fees", "account_addresses"), &SolanaClient::get_recent_prioritization_fees);
	ClassDB::bind_method(D_METHOD("get_signature_for_address", "address", "before", "until"), &SolanaClient::get_signature_for_address);
	ClassDB::bind_method(D_METHOD("get_signature_statuses", "signatures", "search_transaction_history"), &SolanaClient::get_signature_statuses);
	ClassDB::bind_method(D_METHOD("get_slot"), &SolanaClient::get_slot);
	ClassDB::bind_method(D_METHOD("get_slot_leader"), &SolanaClient::get_slot_leader);
	ClassDB::bind_method(D_METHOD("get_slot_leaders", "start_slot", "slot_limit"), &SolanaClient::get_slot_leaders);
	ClassDB::bind_method(D_METHOD("get_stake_activation", "account"), &SolanaClient::get_stake_activation);
	ClassDB::bind_method(D_METHOD("get_stake_minimum_delegation"), &SolanaClient::get_stake_minimum_delegation);
	ClassDB::bind_method(D_METHOD("get_supply", "exclude_non_circulating"), &SolanaClient::get_supply);
	ClassDB::bind_method(D_METHOD("get_token_account_balance", "token_account"), &SolanaClient::get_token_account_balance);
	ClassDB::bind_method(D_METHOD("get_token_accounts_by_delegate", "account_delegate", "mint", "program_id"), &SolanaClient::get_token_accounts_by_delegate, DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("get_token_accounts_by_owner", "owner", "mint", "program_id"), &SolanaClient::get_token_accounts_by_owner, DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("get_token_largest_account", "token_mint"), &SolanaClient::get_token_largest_account);
	ClassDB::bind_method(D_METHOD("get_token_supply", "token_mint"), &SolanaClient::get_token_supply);
	ClassDB::bind_method(D_METHOD("get_transaction", "signature"), &SolanaClient::get_transaction);
	ClassDB::bind_method(D_METHOD("get_transaction_count"), &SolanaClient::get_transaction_count);
	ClassDB::bind_method(D_METHOD("get_version"), &SolanaClient::get_version);
	ClassDB::bind_method(D_METHOD("get_vote_accounts", "vote_pubkey", "keep_unstaked_delinquents"), &SolanaClient::get_vote_accounts);
	ClassDB::bind_method(D_METHOD("is_blockhash_valid", "blockhash"), &SolanaClient::is_blockhash_valid);
	ClassDB::bind_method(D_METHOD("minimum_ledger_slot"), &SolanaClient::minimum_ledger_slot);
	ClassDB::bind_method(D_METHOD("request_airdrop", "address", "lamports"), &SolanaClient::request_airdrop);
	ClassDB::bind_method(D_METHOD("send_transaction", "encoded_transaction", "max_retries", "skip_preflight"), &SolanaClient::send_transaction);
	ClassDB::bind_method(D_METHOD("simulate_transaction", "encoded_transaction", "sig_verify", "replace_blockhash", "account_addresses", "account_encoding"), &SolanaClient::simulate_transaction);

	ClassDB::bind_method(D_METHOD("get_asset", "id"), &SolanaClient::get_asset);
	ClassDB::bind_method(D_METHOD("get_asset_proof", "id"), &SolanaClient::get_asset_proof);
	ClassDB::bind_method(D_METHOD("get_assets_by_authority", "authority", "page"), &SolanaClient::get_assets_by_authority, DEFVAL(10U), DEFVAL(1U));
	ClassDB::bind_method(D_METHOD("get_assets_by_creator_address", "creator_address", "only_verified", "page", "limit"), &SolanaClient::get_assets_by_creator_address, DEFVAL(10U), DEFVAL(1U), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("get_assets_by_group", "group_key", "group_value", "page", "limit"), &SolanaClient::get_assets_by_group, DEFVAL(10U), DEFVAL(1U));
	ClassDB::bind_method(D_METHOD("get_assets_by_owner", "owner", "page", "limit"), &SolanaClient::get_assets_by_owner, DEFVAL(10U), DEFVAL(1U));

	ClassDB::bind_method(D_METHOD("unsubscribe_all", "callback"), &SolanaClient::unsubscribe_all);
	ClassDB::bind_method(D_METHOD("account_subscribe", "account_key", "callback"), &SolanaClient::account_subscribe);
	ClassDB::bind_method(D_METHOD("signature_subscribe", "signature", "callback", "commitment"), &SolanaClient::signature_subscribe);
	ClassDB::bind_method(D_METHOD("program_subscribe", "program_id", "callback"), &SolanaClient::program_subscribe);
	ClassDB::bind_method(D_METHOD("root_subscribe", "callback"), &SolanaClient::root_subscribe);
	ClassDB::bind_method(D_METHOD("slot_subscribe", "callback"), &SolanaClient::slot_subscribe);

	ClassDB::add_property("SolanaClient", PropertyInfo(Variant::STRING, "url_override", PROPERTY_HINT_NONE), "set_url_override", "get_url_override");
	ClassDB::add_property("SolanaClient", PropertyInfo(Variant::STRING, "ws_url", PROPERTY_HINT_NONE), "set_ws_url", "get_ws_url");
	ClassDB::add_property("SolanaClient", PropertyInfo(Variant::STRING, "commitment", PROPERTY_HINT_ENUM, "confirmed,processed,finalized"), "set_commitment", "get_commitment");
	ClassDB::add_property("SolanaClient", PropertyInfo(Variant::STRING, "encoding", PROPERTY_HINT_ENUM, "base64,base58"), "set_encoding", "get_encoding");
	ClassDB::add_property("SolanaClient", PropertyInfo(Variant::FLOAT, "timeout"), "set_timeout", "get_timeout");
}

void SolanaClient::_process(double delta) {
	get_current_http_client()->process(delta);
	get_current_ws_client()->process(delta);
}

void SolanaClient::_ready() {
}

SolanaClient::SolanaClient() :
		transaction_detail("full") {
}

Dictionary SolanaClient::parse_url(const String &url) {
	Dictionary result;
	String sliced_url = url;

	// Strip away optional query and fragment strings.
	PackedStringArray strings = sliced_url.split("#");
	if (strings.size() > 1) {
		sliced_url = strings[0];
		result["fragment"] = strings[1];
	}
	strings = sliced_url.split("?");
	if (strings.size() > 1) {
		sliced_url = strings[0];
		result["query"] = strings[1];
	}

	// Find optional scheme
	strings = sliced_url.split("://");
	if (strings.size() > 1) {
		sliced_url = strings[1];
		result["scheme"] = strings[0];
	}

	// Get path and host.
	const int64_t path_location = sliced_url.find("/");
	if (path_location > 0) {
		const String path = sliced_url.substr(path_location);
		if (path.length() > 1) {
			result["path"] = path;
		}
		sliced_url = sliced_url.substr(0, path_location);
	}

	strings = sliced_url.split(":");
	if (strings.size() > 1) {
		result["port"] = strings[1].to_int();
		sliced_url = strings[0];
	}

	strings = sliced_url.split("@");
	if (strings.size() > 1) {
		result["userinfo"] = strings[0];
		sliced_url = strings[1];
	}

	result["host"] = sliced_url;

	return result;
}

String SolanaClient::assemble_url(const Dictionary &url_components) {
	String result = "";

	if (url_components.has("scheme")) {
		result += static_cast<String>(url_components["scheme"]) + "://";
	}
	if (url_components.has("userinfo")) {
		result += static_cast<String>(url_components["userinfo"]) + "@";
	}
	if (url_components.has("host")) {
		result += static_cast<String>(url_components["host"]);
	}
	if (url_components.has("port")) {
		result += ":" + String::num_uint64(url_components["port"]);
	}
	if (url_components.has("path")) {
		result += static_cast<String>(url_components["path"]);
	}
	if (url_components.has("query")) {
		result += "?" + static_cast<String>(url_components["query"]);
	}
	if (url_components.has("fragment")) {
		result += "#" + static_cast<String>(url_components["fragment"]);
	}

	return result;
}

unsigned int SolanaClient::get_next_request_identifier() {
	return SolanaClient::global_rpc_id;
}

void SolanaClient::set_url_override(const String &url_override) {
	Dictionary parsed_url = parse_url(url_override);

	// See if tls should be used.
	use_tls = true;
	if (!parsed_url.has("scheme")) {
		WARN_PRINT_ONCE_ED_CUSTOM("No scheme specified, enabling tls by default.");
	} else if (parsed_url["scheme"] == "http") {
		use_tls = false;
	} else if (parsed_url["scheme"] != "https") {
		WARN_PRINT_ONCE_ED_CUSTOM("Unknown scheme, enabling tls by default.");
	}

	if (parsed_url.has("port")) {
		http_port_override = parsed_url["port"];
	}

	this->url_override = url_override;
	Dictionary ws_url = parsed_url;
	if (use_tls) {
		ws_url["scheme"] = "wss";
	} else {
		ws_url["scheme"] = "ws";
	}
	ws_url["port"] = get_real_ws_port();
	this->ws_url = assemble_url(ws_url);
}

void SolanaClient::set_ws_url(const String &url) {
	const Dictionary parsed_url = parse_url(url);

	ws_url = assemble_url(parsed_url).ascii();
}

String SolanaClient::get_ws_url() {
	return ws_url;
}

void SolanaClient::set_callback(const Callable &callback) {
	rpc_callback = callback;
}

String SolanaClient::get_url_override() {
	return url_override;
}

void SolanaClient::set_commitment(const String &commitment) {
	this->commitment = commitment;
}

String SolanaClient::get_commitment() {
	return commitment;
}

void SolanaClient::set_encoding(const String &encoding) {
	this->encoding = encoding;
}

String SolanaClient::get_encoding() {
	return encoding;
}

void SolanaClient::set_transaction_detail(const String &transaction_detail) {
	this->transaction_detail = transaction_detail.utf8();
}

void SolanaClient::enable_min_context_slot(int slot) {
	SolanaClient::min_constext_slot_enabled = true;
	SolanaClient::min_context_slot = slot;
}

void SolanaClient::disable_min_context_slot() {
	SolanaClient::min_constext_slot_enabled = false;
	SolanaClient::min_context_slot = 0;
}

void SolanaClient::enable_account_filter(uint64_t offset, uint64_t length) { // NOLINT(bugprone-easily-swappable-parameters)
	SolanaClient::account_filter_enabled = true;
	SolanaClient::filter_offset = offset;
	SolanaClient::filter_length = length;
}

void SolanaClient::disable_account_filter() {
	SolanaClient::account_filter_enabled = false;
	SolanaClient::filter_offset = 0;
	SolanaClient::filter_length = 0;
}

void SolanaClient::enable_max_transaction_version(uint64_t version) {
	max_transaction_version_enabled = true;
	max_transaction_version = version;
}

void SolanaClient::disable_max_transaction_version() {
	max_transaction_version_enabled = false;
	max_transaction_version = 0;
}

void SolanaClient::enable_rewards() {
	rewards = true;
}

void SolanaClient::disable_rewards() {
	rewards = false;
}

void SolanaClient::enable_identity(const String &identity) {
	SolanaClient::identity = identity.utf8();
}

void SolanaClient::disable_identity() {
	identity = "";
}

void SolanaClient::enable_slot_range(uint64_t first, uint64_t last) {
	SolanaClient::first_slot = first;
	SolanaClient::last_slot = last;
	slot_range_enabled = true;
}

void SolanaClient::disable_slot_range() {
	SolanaClient::first_slot = 0;
	SolanaClient::last_slot = 0;
	slot_range_enabled = false;
}

void SolanaClient::set_async_override(bool use_async) {
	async_override = use_async;
}

bool SolanaClient::get_async_override() const {
	return async_override;
}

void SolanaClient::set_timeout(float timeout) {
	this->timeout = timeout;
}

float SolanaClient::get_timeout() const {
	return timeout;
}

bool SolanaClient::is_ready() const {
	return !pending_request;
}

} //namespace godot