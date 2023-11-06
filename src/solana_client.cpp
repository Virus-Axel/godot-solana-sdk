#include "solana_client.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/tls_options.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

std::string SolanaClient::url;
std::string SolanaClient::commitment;
std::string SolanaClient::encoding;
std::string SolanaClient::transaction_detail;
std::string SolanaClient::identity;
uint64_t SolanaClient::min_context_slot = 0;
uint64_t SolanaClient::filter_offset = 0;
uint64_t SolanaClient::filter_length = 0;
uint64_t SolanaClient::max_transaction_version = 0;
uint64_t SolanaClient::first_slot = 0;
uint64_t SolanaClient::last_slot = 0;
bool SolanaClient::min_constext_slot_enabled = false;
bool SolanaClient::account_filter_enabled = false;
bool SolanaClient::max_transaction_version_enabled = false;
bool SolanaClient::rewards = true;
bool SolanaClient::slot_range_enabled = false;

void SolanaClient::append_commitment(Array& options){
    if(!commitment.empty()){
        add_to_param_dict(options, "commitment", String(SolanaClient::commitment.c_str()));
    }
}

void SolanaClient::append_min_context_slot(Array& options){
    if(min_constext_slot_enabled){
        add_to_param_dict(options, "minContextSlot", min_context_slot);
    }
}

void SolanaClient::append_encoding(Array& options){
    if(!encoding.empty()){
        add_to_param_dict(options, "encoding", String(SolanaClient::encoding.c_str()));
    }
}

void SolanaClient::append_transaction_detail(Array& options){
    add_to_param_dict(options, "transactionDetails", String(SolanaClient::transaction_detail.c_str()));
}

void SolanaClient::append_account_filter(Array& options){
    if(account_filter_enabled){
        Dictionary filter;
        filter["offset"] = filter_offset;
        filter["length"] = filter_length;
        add_to_param_dict(options, "dataSlice", filter);
    }
}

void SolanaClient::append_data_filter(Array& options){
    
}

void SolanaClient::append_max_transaction_version(Array& options){
    if(max_transaction_version_enabled){
        add_to_param_dict(options, "maxSupportedTransactionVersion", max_transaction_version);
    }
}

void SolanaClient::append_rewards(Array& options){
    add_to_param_dict(options, "rewards", rewards);
}

void SolanaClient::append_identity(Array& options){
    if(!identity.empty()){
        add_to_param_dict(options, "identity", String(identity.c_str()));
    }
}

void SolanaClient::append_slot_range(Array& options){
    if(slot_range_enabled){
        Dictionary range;
        range["first"] = first_slot;
        range["last"] = last_slot;
        add_to_param_dict(options, "range", range);
    }
}

void SolanaClient::append_limit(Array& options){
}

void SolanaClient::add_to_param_dict(Array &options, const String& key, const Variant& value){
    if(options.back().get_type() == Variant::DICTIONARY){
        Dictionary dict = options.back();
        dict[key] = value;
    }
    else{
        options.append(make_rpc_param(key, value));
    }
}

Dictionary SolanaClient::make_rpc_dict(const String& method, const Array& params){
    Dictionary request_body;

    request_body["id"] = 1;
    request_body["jsonrpc"] = "2.0";
    request_body["method"] = method;
    request_body["params"] = params;

    return request_body;
}

Dictionary SolanaClient::make_rpc_param(const Variant& key, const Variant& value){
    Dictionary result;
    result[key] = value;
    return result;
}

Dictionary SolanaClient::quick_http_request(const String& request_body){
#ifndef WEB_ENABLED
	const int32_t POLL_DELAY_MSEC = 100;

	// Set headers
	PackedStringArray http_headers;
	http_headers.append("Content-Type: application/json");
	http_headers.append("Accept-Encoding: json");
	
	// Connect to RPC API URL.
	HTTPClient handler;

	Error err = handler.connect_to_host(String(url.c_str()), 443, TLSOptions::client_unsafe());

	// Wait until a connection is established.
	godot::HTTPClient::Status status = handler.get_status();
	while(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		handler.poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
		status = handler.get_status();
	}

	// Make a POST request
	err = handler.request(godot::HTTPClient::METHOD_POST, "/", http_headers, request_body);

	if(err != Error::OK){
		gdextension_interface_print_warning("Error sending request.", "quick_http_request", "solana_sdk.cpp", __LINE__, false);
		return Dictionary();
	}

	// Poll until we have a response.
	status = handler.get_status();
	while(status == HTTPClient::STATUS_REQUESTING){
		handler.poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
		status = handler.get_status();
	}

	// Collect the response body.
	PackedByteArray response_data;
	while(status == HTTPClient::STATUS_BODY){
		response_data.append_array(handler.read_response_body_chunk());
		handler.poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
		status = handler.get_status();
	}

	handler.close();

	// Parse out the blockhash.
	JSON json;
	err = json.parse(response_data.get_string_from_utf8());

	if(err != Error::OK){
		gdextension_interface_print_warning("Error getting response data.", "quick_http_request", "solana_sdk.cpp", __LINE__, false);
		return Dictionary();
	}

	return json.get_data();

#else
    String web_script = "\
        var request = new XMLHttpRequest();\
        request.open('POST', '{0}', false);\
        request.setRequestHeader('Content-Type', 'application/json');\
        request.setRequestHeader('Accept-Encoding', 'json');\
        request.send(null);\
        \
        Module.solanaClientResponse = request.response;\
        print(request.response);\
        return request.response;";

    Array params;
    params.append(String(url.c_str()));

    return JavaScriptBridge::get_singleton()->eval(web_script.format(params));

#endif


}

Dictionary SolanaClient::get_latest_blockhash(){
    Array params;
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getLatestBlockhash", params)));
}

Dictionary SolanaClient::get_block_production(){
    Array params;
    append_commitment(params);
    append_identity(params);
    append_slot_range(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlockProduction", params)));
}

Dictionary SolanaClient::get_account_info(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getAccountInfo", params)));
}


Dictionary SolanaClient::get_balance(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBalance", params)));
}

Dictionary SolanaClient::get_block(uint64_t slot, const String& detail){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_encoding(params);
    append_transaction_detail(params);
    append_max_transaction_version(params);
    append_rewards(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlock", params)));
}

Dictionary SolanaClient::get_block_height(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlockHeight", params)));
}

Dictionary SolanaClient::get_block_commitment(uint64_t slot_number){
    Array params;
    params.append(slot_number);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlockCommitment", params)));
}

Dictionary SolanaClient::get_blocks(uint64_t start_slot, const Variant& end_slot){
    Array params;
    params.append(start_slot);
    if(end_slot.get_type() == Variant::INT){
        params.append(end_slot);
    }
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlocks", params)));
}

Dictionary SolanaClient::get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot){
    Array params;
    params.append(start_slot);
    params.append(end_slot);
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlocksWithLimit", params)));
}

Dictionary SolanaClient::get_block_time(uint64_t slot){
    Array params;
    params.append(slot);

    return quick_http_request(JSON::stringify(make_rpc_dict("getBlockTime", params)));
}

Dictionary SolanaClient::get_cluster_nodes(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getClusterNodes", Array())));
}

Dictionary SolanaClient::get_epoch_info(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getEpochInfo", params)));
}

Dictionary SolanaClient::get_epoch_schedule(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getEpochSchedule", Array())));
}

Dictionary SolanaClient::get_fee_for_message(const String& encoded_message){
    Array params;
    params.append(encoded_message);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getFeeForMessage", params)));
}

Dictionary SolanaClient::get_first_available_block(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getFirstAvailableBlock", Array())));
}

Dictionary SolanaClient::get_genesis_hash(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getGenesisHash", Array())));
}

Dictionary SolanaClient::get_health(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getHealth", Array())));
}

Dictionary SolanaClient::get_highest_snapshot_slot(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getHighestSnapshotSlot", Array())));
}

Dictionary SolanaClient::get_identity(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getIdentity", Array())));
}

Dictionary SolanaClient::get_inflation_governor(){
    Array params;
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getInflationGovernor", params)));
}

Dictionary SolanaClient::get_inflation_rate(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getInflationRate", Array())));
}

Dictionary SolanaClient::get_inflation_reward(const PackedStringArray accounts, const Variant& epoch){
    Array params;
    params.append(accounts);
    append_commitment(params);
    if(epoch.get_type() == Variant::INT){
        add_to_param_dict(params, "epoch", epoch);
    }
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getInflationReward", params)));
}

Dictionary SolanaClient::get_largest_accounts(const String& filter){
    Array params;
    append_commitment(params);
    if(!filter.is_empty()){
        add_to_param_dict(params, "filter", filter);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getLargestAccounts", params)));
}

Dictionary SolanaClient::get_leader_schedule(const Variant& slot){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_identity(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getLeaderSchedule", params)));
}

Dictionary SolanaClient::get_max_retransmit_slot(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getMaxRetransmitSlot", Array())));
}

Dictionary SolanaClient::get_max_shred_insert_slot(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getMaxShredInsertSlot", Array())));
}

Dictionary SolanaClient::get_minimum_balance_for_rent_extemption(uint64_t data_size){
    Array params;
    params.append(data_size);
    append_commitment(params);
    
    return quick_http_request(JSON::stringify(make_rpc_dict("getMinimumBalanceForRentExemption", params)));
}

Dictionary SolanaClient::get_multiple_accounts(const PackedStringArray accounts){
    Array params;
    params.append(accounts);
    append_commitment(params);
    append_min_context_slot(params);
    append_account_filter(params);
    append_encoding(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getMultipleAccounts", params)));
}

Dictionary SolanaClient::get_program_accounts(const String& program_address, bool with_context){
    Array params;
    params.append(program_address);
    append_commitment(params);
    append_min_context_slot(params);
    add_to_param_dict(params, "withContext", with_context);
    append_encoding(params);
    append_account_filter(params);
    append_data_filter(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getProgramAccounts", params)));
}

Dictionary SolanaClient::get_recent_performance_samples(){
    Array params;
    append_limit(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getRecentPerformanceSamples", params)));
}

Dictionary SolanaClient::get_recent_prioritization_fees(PackedStringArray account_addresses){
    Array params;
    if(!account_addresses.is_empty()){
        params.append(account_addresses);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getRecentPrioritizationFees", params)));
}

Dictionary SolanaClient::get_signature_for_address(const String& address, const String& before, const String& until){
    Array params;
    params.append(address);
    append_commitment(params);
    append_min_context_slot(params);
    append_limit(params);
    if(!before.is_empty()){
        add_to_param_dict(params, "before", before);
    }
    if(!until.is_empty()){
        add_to_param_dict(params, "until", until);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getSignaturesForAddress", params)));
}

Dictionary SolanaClient::get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history){
    Array params;

    add_to_param_dict(params, "searchTransactionHistory", search_transaction_history);

    return quick_http_request(JSON::stringify(make_rpc_dict("getSignatureStatuses", params)));
}

Dictionary SolanaClient::get_slot(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getSlot", params)));
}

Dictionary SolanaClient::get_slot_leader(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getSlotLeader", params)));
}

Dictionary SolanaClient::get_slot_leaders(const Variant& start_slot, const Variant& slot_limit){
    Array params;

    if(start_slot.get_type() == Variant::NIL){
        params.append(start_slot);
    }
    if(slot_limit.get_type() == Variant::NIL){
        params.append(slot_limit);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getSlotLeaders", params)));
}

Dictionary SolanaClient::get_stake_activation(const String& account){
    Array params;

    params.append(account);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getStakeActivation", params)));
}

Dictionary SolanaClient::get_stake_minimum_delegation(){
    Array params;

    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getStakeMinimumDelegation", params)));
}

Dictionary SolanaClient::get_supply(bool exclude_non_circulating){
    Array params;

    append_commitment(params);
    add_to_param_dict(params, "excludeNonCirculatingAccountsList", exclude_non_circulating);

    return quick_http_request(JSON::stringify(make_rpc_dict("getSupply", params)));
}

Dictionary SolanaClient::get_token_account_balance(const String& token_account){
    Array params;

    params.append(token_account);
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getTokenAccountBalance", params)));
}

Dictionary SolanaClient::get_token_accounts_by_delegate(const String& account_delegate, const String& mint, const String& program_id){
    Array params;

    Dictionary dict_argument;

    if(!mint.is_empty()){
        dict_argument["mint"] = mint;
    }
    else if(!program_id.is_empty()){
        dict_argument["programId"] = program_id;
    }

    append_commitment(params);
    append_min_context_slot(params);
    append_account_filter(params);
    append_encoding(params);

    if(!dict_argument.is_empty()){
        params.insert(0, dict_argument);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getTokenAccountsByDelegate", params)));
}

Dictionary SolanaClient::get_token_accounts_by_owner(const String& owner, const String &mint, const String& program_id){
    Array params;
    Dictionary dict_argument;

    params.append(owner);

    if(!mint.is_empty()){
        dict_argument["mint"] = mint;
    }
    else if(!program_id.is_empty()){
        dict_argument["programId"] = program_id;
    }

    append_commitment(params);
    append_min_context_slot(params);
    append_account_filter(params);
    append_encoding(params);

    if(!dict_argument.is_empty()){
        params.insert(1, dict_argument);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("getTokenAccountsByOwner", params)));
}

Dictionary SolanaClient::get_token_largest_account(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getTokenLargestAccounts", params)));
}

Dictionary SolanaClient::get_token_supply(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getTokenSupply", params)));
}

Dictionary SolanaClient::get_transaction(const String& signature){
    Array params;

    params.append(signature);
    append_commitment(params);
    append_max_transaction_version(params);
    append_encoding(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getTransaction", params)));
}

Dictionary SolanaClient::get_transaction_count(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("getTransactionCount", params)));
}

Dictionary SolanaClient::get_version(){
    return quick_http_request(JSON::stringify(make_rpc_dict("getVersion", Array())));
}

Dictionary SolanaClient::get_vote_accounts(const String& vote_pubkey, bool keep_unstaked_delinquents){
    Array params;

    append_commitment(params);
    if(!vote_pubkey.is_empty()){
        add_to_param_dict(params, "votePubkey", vote_pubkey);
    }
    add_to_param_dict(params, "keepUnstakedDelinquents", keep_unstaked_delinquents);

    return quick_http_request(JSON::stringify(make_rpc_dict("getVoteAccounts", params)));
}

Dictionary SolanaClient::is_blockhash_valid(const String &blockhash){
    Array params;

    params.append(blockhash);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("isBlockhashValid", params)));
}

Dictionary SolanaClient::minimum_ledger_slot(){
    return quick_http_request(JSON::stringify(make_rpc_dict("minimumLedgerSlot", Array())));
}

Dictionary SolanaClient::request_airdrop(const String& address, uint64_t lamports){
    Array params;

    params.append(address);
    params.append(lamports);
    append_commitment(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("requestAirdrop", params)));
}

Dictionary SolanaClient::send_transaction(const String& encoded_transaction, uint64_t max_retries, bool skip_preflight){
    Array params;

    params.append(encoded_transaction);
    append_encoding(params);
    add_to_param_dict(params, "skipPreflight", skip_preflight);
    add_to_param_dict(params, "preflightCommitment", String(commitment.c_str()));
    add_to_param_dict(params, "maxRetries", max_retries);
    append_min_context_slot(params);

    return quick_http_request(JSON::stringify(make_rpc_dict("sendTransaction", params)));
}

Dictionary SolanaClient::simulate_transaction(const String& encoded_transaction, bool sig_verify, bool replace_blockhash, Array account_addresses, const String& account_encoding){
    Array params;

    params.append(encoded_transaction);
    append_commitment(params);

    add_to_param_dict(params, "sigVerify", sig_verify);
    add_to_param_dict(params, "replaceRecentBlockhash", replace_blockhash);
    append_min_context_slot(params);
    append_encoding(params);

    if(!account_addresses.is_empty()){
        Dictionary accounts;
        accounts["addresses"] = account_addresses;
        accounts["encoding"] = account_encoding;
        add_to_param_dict(params, "accounts", accounts);
    }

    return quick_http_request(JSON::stringify(make_rpc_dict("simulateTransaction", params)));
}

void SolanaClient::_bind_methods(){
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_latest_blockhash"), &SolanaClient::get_latest_blockhash);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_balance", "account"), &SolanaClient::get_balance);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_account_info", "account"), &SolanaClient::get_account_info);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("set_url", "url"), &SolanaClient::set_url);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_block", "slot"), &SolanaClient::get_block);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_block_height"), &SolanaClient::get_block_height);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_block_commitment", "slot"), &SolanaClient::get_block_commitment);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_blocks", "start_slot", "end_slot"), &SolanaClient::get_blocks);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_blocks_with_limit", "start_slot", "end_slot"), &SolanaClient::get_blocks_with_limit);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_block_time", "slot"), &SolanaClient::get_block_time);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_cluster_nodes"), &SolanaClient::get_cluster_nodes);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_epoch_info"), &SolanaClient::get_epoch_info);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_epoch_schedule"), &SolanaClient::get_epoch_schedule);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_fee_for_message", "encoded_message"), &SolanaClient::get_fee_for_message);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_first_available_block"), &SolanaClient::get_first_available_block);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_genesis_hash"), &SolanaClient::get_genesis_hash);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_health"), &SolanaClient::get_health);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_highest_snapshot_slot"), &SolanaClient::get_highest_snapshot_slot);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_identity"), &SolanaClient::get_identity);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_inflation_governor"), &SolanaClient::get_inflation_governor);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_inflation_rate"), &SolanaClient::get_inflation_rate);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_inflation_reward", "accounts", "epoch"), &SolanaClient::get_inflation_reward);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_largest_accounts", "filter"), &SolanaClient::get_largest_accounts);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_leader_schedule", "slot"), &SolanaClient::get_leader_schedule);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_max_retransmit_slot"), &SolanaClient::get_max_retransmit_slot);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_max_shred_insert_slot"), &SolanaClient::get_max_shred_insert_slot);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_minimum_balance_for_rent_extemption", "data_size"), &SolanaClient::get_minimum_balance_for_rent_extemption);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_multiple_accounts", "accounts"), &SolanaClient::get_multiple_accounts);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_program_accounts", "program_address", "with_context"), &SolanaClient::get_program_accounts);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_recent_performance_samples"), &SolanaClient::get_recent_performance_samples);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_recent_prioritization_fees", "account_addresses"), &SolanaClient::get_recent_prioritization_fees);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_signature_for_address", "address", "before", "until"), &SolanaClient::get_signature_for_address);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_signature_statuses", "signatures", "search_transaction_history"), &SolanaClient::get_signature_statuses);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_slot"), &SolanaClient::get_slot);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_slot_leader"), &SolanaClient::get_slot_leader);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_slot_leaders", "start_slot", "slot_limit"), &SolanaClient::get_slot_leaders);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_stake_activation", "account"), &SolanaClient::get_stake_activation);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_stake_minimum_delegation"), &SolanaClient::get_stake_minimum_delegation);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_supply", "exclude_non_circulating"), &SolanaClient::get_supply);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_token_account_balance", "token_account"), &SolanaClient::get_token_account_balance);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_token_accounts_by_delegate", "account_delegate", "mint", "program_id"), &SolanaClient::get_token_accounts_by_delegate);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_token_accounts_by_owner", "owner", "mint", "program_id"), &SolanaClient::get_token_accounts_by_owner);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_token_largest_account", "token_mint"), &SolanaClient::get_token_largest_account);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_token_supply", "token_mint"), &SolanaClient::get_token_supply);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_transaction", "signature"), &SolanaClient::get_transaction);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_transaction_count"), &SolanaClient::get_transaction_count);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_version"), &SolanaClient::get_version);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_vote_accounts", "vote_pubkey", "keep_unstaked_delinquents"), &SolanaClient::get_vote_accounts);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("is_blockhash_valid", "blockhash"), &SolanaClient::is_blockhash_valid);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("minimum_ledger_slot"), &SolanaClient::minimum_ledger_slot);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("request_airdrop", "address", "lamports"), &SolanaClient::request_airdrop);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("send_transaction", "encoded_transaction", "max_retries", "skip_preflight"), &SolanaClient::send_transaction);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("simulate_transaction", "encoded_transaction", "sig_verify", "replace_blockhash", "account_addresses", "account_encoding"), &SolanaClient::simulate_transaction);
}

SolanaClient::SolanaClient(){
    transaction_detail = "full";
}

void SolanaClient::set_url(const String& url){
	SolanaClient::url = url.ascii();
}

void SolanaClient::set_commitment(const String& commitment){
    SolanaClient::commitment = commitment.utf8();
}

void SolanaClient::set_encoding(const String& encoding){
    SolanaClient::encoding = encoding.utf8();
}

void SolanaClient::set_transaction_detail(const String& transaction_detail){
    SolanaClient::transaction_detail = transaction_detail.utf8();
}

void SolanaClient::enable_min_context_slot(int slot){
    SolanaClient::min_constext_slot_enabled = true;
    SolanaClient::min_context_slot = slot;
}

void SolanaClient::disable_min_context_slot(){
    SolanaClient::min_constext_slot_enabled = false;
    SolanaClient::min_context_slot = 0;
}

void SolanaClient::enable_account_filter(uint64_t offset, uint64_t length){
    SolanaClient::account_filter_enabled = true;
    SolanaClient::filter_offset = offset;
    SolanaClient::filter_length = length;
}

void SolanaClient::disable_account_filter(){
    SolanaClient::account_filter_enabled = false;
    SolanaClient::filter_offset = 0;
    SolanaClient::filter_length = 0;
}

void SolanaClient::enable_max_transaction_version(uint64_t version){
    max_transaction_version_enabled = true;
    max_transaction_version = version;
}

void SolanaClient::disable_max_transaction_version(){
    max_transaction_version_enabled = false;
    max_transaction_version = 0;
}

void SolanaClient::enable_rewards(){
    rewards = true;
}

void SolanaClient::disable_rewards(){
    rewards = false;
}

void SolanaClient::enable_identity(const String& identity){
    SolanaClient::identity = identity.utf8();
}

void SolanaClient::disable_identity(){
    identity = "";
}

void SolanaClient::enable_slot_range(uint64_t first, uint64_t last){
    SolanaClient::first_slot = first;
    SolanaClient::last_slot = last;
    slot_range_enabled = true;
}

void SolanaClient::disable_slot_range(){
    SolanaClient::first_slot = 0;
    SolanaClient::last_slot = 0;
    slot_range_enabled = false;
}

SolanaClient::~SolanaClient(){
    
}

}