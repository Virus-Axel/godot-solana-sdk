#include "solana_client.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/tls_options.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#include <pubkey.hpp>
#include <godot_cpp/classes/engine.hpp>

namespace godot{

unsigned int SolanaClient::global_rpc_id = 1;

using internal::gdextension_interface_print_warning;

String SolanaClient::ws_from_http(const String& http_url){
    Dictionary parsed_url = parse_url(http_url);
    // See if tls should be used.
    use_tls = true;
    if(!parsed_url.has("scheme")){
    }
    else if(parsed_url["scheme"] == "http"){
        use_tls = false;
    }

    Dictionary ws_url = parsed_url;
    if(use_tls){
        ws_url["scheme"] = "wss";
    }
    else{
        ws_url["scheme"] = "ws";
    }
    ws_url["port"] = get_real_ws_port();
   return assemble_url(ws_url);
}

String SolanaClient::get_real_url(){
    if(url_override.is_empty()){
        return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_url");
    }
    else{
        return url_override;
    }
}

uint32_t SolanaClient::get_real_http_port(){
    if(http_port_override == 0){
        return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_http_port");
    }
    else{
        return http_port_override;
    }
}

uint32_t SolanaClient::get_real_ws_port(){
    if(ws_port_override == 0){
        return ProjectSettings::get_singleton()->get_setting("solana_sdk/client/default_ws_port");
    }
    else{
        return ws_port_override;
    }
}

String SolanaClient::get_real_ws_url(){
    if(ws_url.is_empty()){
        return ws_from_http(get_real_url());
    }
    else{
        return ws_url;
    }
}

WsRpcCall *SolanaClient::ws_client(){
    Object* ptr = Engine::get_singleton()->get_singleton("ws_client");
    return Object::cast_to<WsRpcCall>(ptr);
}

RpcMultiHttpRequestClient *SolanaClient::http_client(){
    Object* ptr = Engine::get_singleton()->get_singleton("http_client");
    return Object::cast_to<RpcMultiHttpRequestClient>(ptr);
}

void SolanaClient::append_commitment(Array& options){
    if(!commitment.is_empty()){
        add_to_param_dict(options, "commitment", commitment);
    }
}

void SolanaClient::append_min_context_slot(Array& options){
    if(min_constext_slot_enabled){
        add_to_param_dict(options, "minContextSlot", min_context_slot);
    }
}

void SolanaClient::append_encoding(Array& options){
    if(!encoding.is_empty()){
        add_to_param_dict(options, "encoding", encoding);
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
    if(options.is_empty()){
        options.append(make_rpc_param(key, value));
        return;
    }
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

    request_body["id"] = global_rpc_id;
    SolanaClient::global_rpc_id += 1;

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


void SolanaClient::quick_http_request(const Dictionary& request_body, const Callable& callback){
    Dictionary parsed_url = parse_url(get_real_url());
    const uint32_t real_port = get_real_http_port();

    // Check if port is set in URL.
    if(parsed_url.has("port")){
        if(real_port != (uint32_t)parsed_url["port"]){
            WARN_PRINT_ONCE_ED("Port is specified in URL and is not the same as port setting, ignoring port setting.");
        }
    }
    else{
        parsed_url["port"] = real_port;
    }

    if(is_inside_tree() || async_override){
        http_client()->asynchronous_request(request_body, parsed_url, Callable(this, "response_callback"));
    }
    else{
        ERR_FAIL_EDMSG("SolanaClient must be in scene tree.");
    }
}

void SolanaClient::response_callback(const Dictionary &params){
    pending_request = false;
    emit_signal("http_response_received", params);
}

void SolanaClient::ws_response_callback(const Dictionary &params){
    emit_signal("socket_response_received", params);
}

void SolanaClient::get_latest_blockhash(){
    Array params;
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getLatestBlockhash", params));
}

void SolanaClient::get_block_production(){
    Array params;
    append_commitment(params);
    append_identity(params);
    append_slot_range(params);

    return quick_http_request(make_rpc_dict("getBlockProduction", params));
}

void SolanaClient::get_account_info(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(make_rpc_dict("getAccountInfo", params));
}


void SolanaClient::get_balance(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(make_rpc_dict("getBalance", params));
}

void SolanaClient::get_block(uint64_t slot, const String& detail){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_encoding(params);
    append_transaction_detail(params);
    append_max_transaction_version(params);
    append_rewards(params);

    return quick_http_request(make_rpc_dict("getBlock", params));
}

void SolanaClient::get_block_height(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getBlockHeight", params));
}

void SolanaClient::get_block_commitment(uint64_t slot_number){
    Array params;
    params.append(slot_number);

    return quick_http_request(make_rpc_dict("getBlockCommitment", params));
}

void SolanaClient::get_blocks(uint64_t start_slot, const Variant& end_slot){
    Array params;
    params.append(start_slot);
    if(end_slot.get_type() == Variant::INT){
        params.append(end_slot);
    }
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getBlocks", params));
}

void SolanaClient::get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot){
    Array params;
    params.append(start_slot);
    params.append(end_slot);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getBlocksWithLimit", params));
}

void SolanaClient::get_block_time(uint64_t slot){
    Array params;
    params.append(slot);

    return quick_http_request(make_rpc_dict("getBlockTime", params));
}

void SolanaClient::get_cluster_nodes(){
    return quick_http_request(make_rpc_dict("getClusterNodes", Array()));
}

void SolanaClient::get_epoch_info(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getEpochInfo", params));
}

void SolanaClient::get_epoch_schedule(){
    return quick_http_request(make_rpc_dict("getEpochSchedule", Array()));
}

void SolanaClient::get_fee_for_message(const String& encoded_message){
    Array params;
    params.append(encoded_message);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getFeeForMessage", params));
}

void SolanaClient::get_first_available_block(){
    return quick_http_request(make_rpc_dict("getFirstAvailableBlock", Array()));
}

void SolanaClient::get_genesis_hash(){
    return quick_http_request(make_rpc_dict("getGenesisHash", Array()));
}

void SolanaClient::get_health(){
    return quick_http_request(make_rpc_dict("getHealth", Array()));
}

void SolanaClient::get_highest_snapshot_slot(){
    return quick_http_request(make_rpc_dict("getHighestSnapshotSlot", Array()));
}

void SolanaClient::get_identity(){
    return quick_http_request(make_rpc_dict("getIdentity", Array()));
}

void SolanaClient::get_inflation_governor(){
    Array params;
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getInflationGovernor", params));
}

void SolanaClient::get_inflation_rate(){
    return quick_http_request(make_rpc_dict("getInflationRate", Array()));
}

void SolanaClient::get_inflation_reward(const PackedStringArray accounts, const Variant& epoch){
    Array params;
    params.append(accounts);
    append_commitment(params);
    if(epoch.get_type() == Variant::INT){
        add_to_param_dict(params, "epoch", epoch);
    }
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getInflationReward", params));
}

void SolanaClient::get_largest_accounts(const String& filter){
    Array params;
    append_commitment(params);
    if(!filter.is_empty()){
        add_to_param_dict(params, "filter", filter);
    }

    return quick_http_request(make_rpc_dict("getLargestAccounts", params));
}

void SolanaClient::get_leader_schedule(const Variant& slot){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_identity(params);

    return quick_http_request(make_rpc_dict("getLeaderSchedule", params));
}

void SolanaClient::get_max_retransmit_slot(){
    return quick_http_request(make_rpc_dict("getMaxRetransmitSlot", Array()));
}

void SolanaClient::get_max_shred_insert_slot(){
    return quick_http_request(make_rpc_dict("getMaxShredInsertSlot", Array()));
}

void SolanaClient::get_minimum_balance_for_rent_extemption(uint64_t data_size){
    Array params;
    params.append(data_size);
    append_commitment(params);
    
    return quick_http_request(make_rpc_dict("getMinimumBalanceForRentExemption", params));
}

void SolanaClient::get_multiple_accounts(const PackedStringArray accounts){
    Array params;
    params.append(accounts);
    append_commitment(params);
    append_min_context_slot(params);
    append_account_filter(params);
    append_encoding(params);

    return quick_http_request(make_rpc_dict("getMultipleAccounts", params));
}

void SolanaClient::get_program_accounts(const String& program_address, bool with_context){
    Array params;
    params.append(program_address);
    append_commitment(params);
    append_min_context_slot(params);
    add_to_param_dict(params, "withContext", with_context);
    append_encoding(params);
    append_account_filter(params);
    append_data_filter(params);

    return quick_http_request(make_rpc_dict("getProgramAccounts", params));
}

void SolanaClient::get_recent_performance_samples(){
    Array params;
    append_limit(params);

    return quick_http_request(make_rpc_dict("getRecentPerformanceSamples", params));
}

void SolanaClient::get_recent_prioritization_fees(PackedStringArray account_addresses){
    Array params;
    if(!account_addresses.is_empty()){
        params.append(account_addresses);
    }

    return quick_http_request(make_rpc_dict("getRecentPrioritizationFees", params));
}

void SolanaClient::get_signature_for_address(const String& address, const String& before, const String& until){
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

    return quick_http_request(make_rpc_dict("getSignaturesForAddress", params));
}

void SolanaClient::get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history){
    Array params;

    params.append(signatures);
    add_to_param_dict(params, "searchTransactionHistory", search_transaction_history);

    return quick_http_request(make_rpc_dict("getSignatureStatuses", params));
}

void SolanaClient::get_slot(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getSlot", params));
}

void SolanaClient::get_slot_leader(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getSlotLeader", params));
}

void SolanaClient::get_slot_leaders(const Variant& start_slot, const Variant& slot_limit){
    Array params;

    if(start_slot.get_type() == Variant::NIL){
        params.append(start_slot);
    }
    if(slot_limit.get_type() == Variant::NIL){
        params.append(slot_limit);
    }

    return quick_http_request(make_rpc_dict("getSlotLeaders", params));
}

void SolanaClient::get_stake_activation(const String& account){
    Array params;

    params.append(account);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getStakeActivation", params));
}

void SolanaClient::get_stake_minimum_delegation(){
    Array params;

    append_commitment(params);

    return quick_http_request(make_rpc_dict("getStakeMinimumDelegation", params));
}

void SolanaClient::get_supply(bool exclude_non_circulating){
    Array params;

    append_commitment(params);
    add_to_param_dict(params, "excludeNonCirculatingAccountsList", exclude_non_circulating);

    return quick_http_request(make_rpc_dict("getSupply", params));
}

void SolanaClient::get_token_account_balance(const String& token_account){
    Array params;

    params.append(token_account);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenAccountBalance", params));
}

void SolanaClient::get_token_accounts_by_delegate(const String& account_delegate, const String& mint, const String& program_id){
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

    return quick_http_request(make_rpc_dict("getTokenAccountsByDelegate", params));
}

void SolanaClient::get_token_accounts_by_owner(const String& owner, const String &mint, const String& program_id){
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

    return quick_http_request(make_rpc_dict("getTokenAccountsByOwner", params));
}

void SolanaClient::get_token_largest_account(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenLargestAccounts", params));
}

void SolanaClient::get_token_supply(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenSupply", params));
}

void SolanaClient::get_transaction(const String& signature){
    Array params;

    params.append(signature);
    append_commitment(params);
    append_max_transaction_version(params);
    append_encoding(params);

    return quick_http_request(make_rpc_dict("getTransaction", params));
}

void SolanaClient::get_transaction_count(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getTransactionCount", params));
}

void SolanaClient::get_version(){
    return quick_http_request(make_rpc_dict("getVersion", Array()));
}

void SolanaClient::get_vote_accounts(const String& vote_pubkey, bool keep_unstaked_delinquents){
    Array params;

    append_commitment(params);
    if(!vote_pubkey.is_empty()){
        add_to_param_dict(params, "votePubkey", vote_pubkey);
    }
    add_to_param_dict(params, "keepUnstakedDelinquents", keep_unstaked_delinquents);

    return quick_http_request(make_rpc_dict("getVoteAccounts", params));
}

void SolanaClient::is_blockhash_valid(const String &blockhash){
    Array params;

    params.append(blockhash);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("isBlockhashValid", params));
}

void SolanaClient::minimum_ledger_slot(){
    return quick_http_request(make_rpc_dict("minimumLedgerSlot", Array()));
}

void SolanaClient::request_airdrop(const String& address, uint64_t lamports){
    Array params;

    params.append(address);
    params.append(lamports);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("requestAirdrop", params));
}

void SolanaClient::send_transaction(const String& encoded_transaction, uint64_t max_retries, bool skip_preflight){
    Array params;

    params.append(encoded_transaction);
    append_encoding(params);
    add_to_param_dict(params, "skipPreflight", skip_preflight);
    add_to_param_dict(params, "preflightCommitment", commitment);
    add_to_param_dict(params, "maxRetries", max_retries);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("sendTransaction", params));
}

void SolanaClient::simulate_transaction(const String& encoded_transaction, bool sig_verify, bool replace_blockhash, Array account_addresses, const String& account_encoding){
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

    return quick_http_request(make_rpc_dict("simulateTransaction", params));
}

void SolanaClient::account_subscribe(const Variant &account_key, const Callable &callback){
    Array params;
    params.append(Pubkey(account_key).to_string());
    add_to_param_dict(params, "encoding", encoding);
    add_to_param_dict(params, "commitment", commitment);

    ws_client()->enqueue_ws_request(make_rpc_dict("accountSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::signature_subscribe(const String &signature, const Callable &callback, const String &commitment){
    Array params;
    params.append(signature);
    add_to_param_dict(params, "commitment", commitment);

    ws_client()->enqueue_ws_request(make_rpc_dict("signatureSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::program_subscribe(const String &program_id, const Callable &callback){
    Array params;
    params.append(program_id);
    append_commitment(params);
    append_account_filter(params);
    append_encoding(params);

    ws_client()->enqueue_ws_request(make_rpc_dict("programSubscribe", params), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::root_subscribe(const Callable &callback){
    ws_client()->enqueue_ws_request(make_rpc_dict("rootSubscribe", Array()), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::slot_subscribe(const Callable &callback){
    ws_client()->enqueue_ws_request(make_rpc_dict("slotSubscribe", Array()), callback, ws_callback, parse_url(get_real_ws_url()));
}

void SolanaClient::unsubscribe_all(const Callable &callback){
    ws_client()->unsubscribe_all(callback, parse_url(get_real_ws_url()));
}

void SolanaClient::_bind_methods(){
    ClassDB::add_signal("SolanaClient", MethodInfo("socket_response_received"));
    ClassDB::add_signal("SolanaClient", MethodInfo("http_response_received", PropertyInfo(Variant::DICTIONARY, "response")));

    ClassDB::bind_static_method("SolanaClient", D_METHOD("assemble_url", "url"), &SolanaClient::assemble_url);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_next_request_identifier"), &SolanaClient::get_next_request_identifier);

    ClassDB::bind_method(D_METHOD("response_callback", "params"), &SolanaClient::response_callback);
    ClassDB::bind_method(D_METHOD("ws_response_callback", "params"), &SolanaClient::ws_response_callback);
    ClassDB::bind_method(D_METHOD("set_url_override", "url_override"), &SolanaClient::set_url_override);
    ClassDB::bind_method(D_METHOD("get_url_override"), &SolanaClient::get_url_override);
    ClassDB::bind_method(D_METHOD("is_ready"), &SolanaClient::is_ready);

    ClassDB::bind_method(D_METHOD("parse_url", "url"), &SolanaClient::parse_url);

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
    ClassDB::bind_method(D_METHOD("get_program_accounts", "program_address", "with_context"), &SolanaClient::get_program_accounts);
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
    ClassDB::bind_method(D_METHOD("get_token_accounts_by_delegate", "account_delegate", "mint=\"\"", "program_id=\"\""), &SolanaClient::get_token_accounts_by_delegate);
    ClassDB::bind_method(D_METHOD("get_token_accounts_by_owner", "owner", "mint=\"\"", "program_id=\"\""), &SolanaClient::get_token_accounts_by_owner);
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

void SolanaClient::_process(double delta){
    http_client()->process(delta);
    ws_client()->process(delta);
}

void SolanaClient::_ready(){
}

SolanaClient::SolanaClient(){
    transaction_detail = "full";
    ws_callback = Callable(this, "ws_response_callback");
    //create_ws_call();
    //create_http_call();
}

Dictionary SolanaClient::parse_url(const String& url){
    Dictionary result;
    String sliced_url = url;

    // Strip away optional query and fragment strings.
    PackedStringArray strings = sliced_url.split("#");
    if(strings.size() > 1){
        sliced_url = strings[0];
        result["fragment"] = strings[1];
    }
    strings = sliced_url.split("?");
    if(strings.size() > 1){
        sliced_url = strings[0];
        result["query"] = strings[1];
    }

    // Find optional scheme
    strings = sliced_url.split("://");
    if(strings.size() > 1){
        sliced_url = strings[1];
        result["scheme"] = strings[0];
    }

    // Get path and host.
    const int path_location = sliced_url.find("/");
    if(path_location > 0){
        const String path = sliced_url.substr(path_location);
        if(path.length() > 1){
            result["path"] = path;
        }
        sliced_url = sliced_url.substr(0, path_location);
    }
    
    strings = sliced_url.split(":");
    if(strings.size() > 1){
        result["port"] = strings[1].to_int();
        sliced_url = strings[0];
    }

    strings = sliced_url.split("@");
    if(strings.size() > 1){
        result["userinfo"] = strings[0];
        sliced_url = strings[1];
    }

    result["host"] = sliced_url;

    return result;
}

String SolanaClient::assemble_url(const Dictionary& url_components){
    String result = "";

    if(url_components.has("scheme")){
        result += (String) url_components["scheme"] + "://";
    }
    if(url_components.has("userinfo")){
        result += (String) url_components["userinfo"] + "@";
    }
    if(url_components.has("host")){
        result += (String) url_components["host"];
    }
    if(url_components.has("port")){
        result += ":" + String::num_uint64(url_components["port"]);
    }
    if(url_components.has("path")){
        result += (String) url_components["path"];
    }
    if(url_components.has("query")){
        result += "?" + (String) url_components["query"];
    }
    if(url_components.has("fragment")){
        result += "#" + (String) url_components["fragment"];
    }

    return result;
}

unsigned int SolanaClient::get_next_request_identifier(){
    return SolanaClient::global_rpc_id;
}

void SolanaClient::set_url_override(const String& url_override){
    Dictionary parsed_url = parse_url(url_override);

    // See if tls should be used.
    use_tls = true;
    if(!parsed_url.has("scheme")){
        gdextension_interface_print_warning("No scheme specified, enabling tls by default.", "set_url", __FILE__, __LINE__, false);
    }
    else if(parsed_url["scheme"] == "http"){
        use_tls = false;
    }
    else if(parsed_url["scheme"] != "https"){
        gdextension_interface_print_warning("Unknown scheme, enabling tls by default.", "set_url", __FILE__, __LINE__, false);
    }

    if(parsed_url.has("port")){
        http_port_override = parsed_url["port"];
    }

	this->url_override = url_override;
    Dictionary ws_url = parsed_url;
    if(use_tls){
        ws_url["scheme"] = "wss";
    }
    else{
        ws_url["scheme"] = "ws";
    }
    ws_url["port"] = get_real_ws_port();
    this->ws_url = assemble_url(ws_url);
}

void SolanaClient::set_ws_url(const String& url){
    Dictionary parsed_url = parse_url(url);

    ws_url = assemble_url(parsed_url).ascii();
}

String SolanaClient::get_ws_url(){
    return ws_url;
}

String SolanaClient::get_url_override(){
    return url_override;
}

void SolanaClient::set_commitment(const String& commitment){
    this->commitment = commitment;
}

String SolanaClient::get_commitment(){
    return commitment;
}

void SolanaClient::set_encoding(const String& encoding){
    this->encoding = encoding;
}

String SolanaClient::get_encoding(){
    return encoding;
}

void SolanaClient::set_transaction_detail(const String& transaction_detail){
    this->transaction_detail = transaction_detail.utf8();
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

void SolanaClient::set_async_override(bool use_async){
    async_override = use_async;
}

bool SolanaClient::get_async_override(){
    return async_override;
}

void SolanaClient::set_timeout(float timeout){
    this->timeout = timeout;
}

float SolanaClient::get_timeout(){
    return timeout;
}

bool SolanaClient::is_ready(){
    return !pending_request;
}

SolanaClient::~SolanaClient(){
}

}