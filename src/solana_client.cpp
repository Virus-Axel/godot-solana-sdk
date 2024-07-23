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
    ws_url["port"] = 8900;
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

HttpRpcCall *SolanaClient::create_http_call(){
    HttpRpcCall* new_http_call = memnew(HttpRpcCall);
    Callable callback = Callable(this, "response_callback");
    new_http_call->set_http_callback(callback);
    rpc_calls.append(Variant(new_http_call));
    return new_http_call;
}

 WsRpcCall *SolanaClient::create_ws_call(){
    WsRpcCall* new_ws_call = memnew(WsRpcCall);
    rpc_calls.push_back(new_ws_call);
    return new_ws_call;
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

Error HttpRpcCall::make_request(const String& request_body){
    // Set headers
    PackedStringArray http_headers;
    http_headers.append("Content-Type: application/json");
    http_headers.append("Accept-Encoding: json");

    // Make a POST request
    return request(godot::HTTPClient::METHOD_POST, path, http_headers, request_body);
}

Error HttpRpcCall::connect_to(Dictionary url){
    int32_t port = url["port"];
    url.erase("port");

    String connect_url = "https";
    if(url.has("scheme")){
        connect_url = url["scheme"];
    }
    connect_url += "://" + (String) url["host"];

    path = "/";
    if(url.has("path")){
        path = url["path"];
    }
    if(url.has("query")){
        path += String("?") + (String)url["query"];
    }
    if(url.has("fragment")){
        path += String("#") + (String)url["fragment"];
    }

    return connect_to_host(connect_url, port);
}

Dictionary HttpRpcCall::synchronous_request(const Dictionary& request_body, const Dictionary& parsed_url){
    #ifndef WEB_ENABLED
	const int32_t POLL_DELAY_MSEC = 100;
    elapsed_time = 0.0F;

    // Godot does not want the port in the url
    ERR_FAIL_COND_V_EDMSG(!parsed_url.has("port"), Dictionary(), "Internal Error, No port specified. Please report this issue!");  
    Error err = connect_to(parsed_url);
    ERR_FAIL_COND_V_EDMSG(err != Error::OK, Dictionary(), "Failed to connect to server.");

	// Wait until a connection is established.
	godot::HTTPClient::Status status = get_status();
	while(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		HTTPClient::poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
        elapsed_time += float(POLL_DELAY_MSEC) / 1000;
		status = get_status();
        if(elapsed_time > timeout){
            HTTPClient::close();
            ERR_FAIL_V_EDMSG(Dictionary(), "Request timed out.");
        }
	}

	// Make a POST request
	err = make_request(JSON::stringify(request_body));

	ERR_FAIL_COND_V_EDMSG(err != Error::OK, Dictionary(), "Error sending request.");

	// Poll until we have a response.
	status = get_status();
	while(status == HTTPClient::STATUS_REQUESTING){
		HTTPClient::poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
        elapsed_time += float(POLL_DELAY_MSEC) / 1000;
		status = get_status();
        if(elapsed_time > timeout){
            HTTPClient::close();
            ERR_FAIL_V_EDMSG(Dictionary(), "Request timed out.");
        }
	}

	// Collect the response body.
	PackedByteArray response_data;
	while(status == HTTPClient::STATUS_BODY){
		response_data.append_array(read_response_body_chunk());
		HTTPClient::poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
        elapsed_time += float(POLL_DELAY_MSEC) / 1000;
		status = get_status();
        if(elapsed_time > timeout){
            HTTPClient::close();
            ERR_FAIL_V_EDMSG(Dictionary(), "Request timed out.");
        }
	}

	HTTPClient::close();

	// Parse the result json.
	JSON json;
	err = json.parse(response_data.get_string_from_utf8());

#else
    String web_script = "\
        var request = new XMLHttpRequest();\
        request.timeout = {0};\
        request.open('POST', '{1}', false);\
        request.setRequestHeader('Content-Type', 'application/json');\
        request.send('{2}');\
        \
        Module.solanaClientResponse = request.response;\
        request.response";

    Array params;

    params.append(timeout * 1000);
    params.append(SolanaClient::assemble_url(parsed_url));
    params.append(request_body);

    Variant result = JavaScriptBridge::get_singleton()->eval(web_script.format(params));

    JSON json;
	Error err = json.parse(result);

#endif

    if(err != Error::OK){
		gdextension_interface_print_warning("Error getting response data.", "quick_http_request", "solana_sdk.cpp", __LINE__, false);
		return Dictionary();
	}

	return json.get_data();

}

void HttpRpcCall::set_http_callback(const Callable& callback){
    http_callback = callback;
}

void HttpRpcCall::asynchronous_request(const Dictionary& request_body, Dictionary parsed_url){
    local_rpc_id = request_body["id"];

    #ifndef WEB_ENABLED
	
    elapsed_time = 0.0F;

    // Godot does not want the port in the url
    ERR_FAIL_COND_EDMSG(!parsed_url.has("port"), "Internal Error, No port specified. Please report this issue!");  
    Error err = connect_to(parsed_url);
    ERR_FAIL_COND_EDMSG(err != Error::OK, "Failed to connect to server.");

    pending_request = true;

    http_request_body = request_body;

#else
    String web_script = "\
        Module.solanaClientReq{3} = new XMLHttpRequest();\
        Module.solanaClientReq{3}.timeout = {0};\
        Module.solanaClientReq{3}.open('POST', '{1}', true);\
        Module.solanaClientReq{3}.setRequestHeader('Content-Type', 'application/json');\
        Module.solanaClientReq{3}.send('{2}');";

    Array params;

    params.append(timeout * 1000);
    params.append(SolanaClient::assemble_url(parsed_url));
    params.append(request_body);
    params.append(local_rpc_id);

    Variant result = JavaScriptBridge::get_singleton()->eval(web_script.format(params));

    JSON json;

#endif
}

void WsRpcCall::enqueue_ws_request(const Dictionary& request_body, const Callable& callback, const String& url){
    connect_ws(url);
    callbacks.push_back(std::make_pair(0, callback));
    method_names.push_back(request_body["method"]);
    ws_request_queue.push(JSON::stringify(request_body));
    pending_request = true;
}

void HttpRpcCall::_bind_methods(){
    
}

bool HttpRpcCall::is_pending(){
    return pending_request;
}

void HttpRpcCall::poll_http_request(const float delta){
#ifndef WEB_ENABLED
    if(!pending_request){
        return;
    }

    elapsed_time += delta;
    if(elapsed_time > timeout){
        close();
        pending_request = false;
        ERR_FAIL_EDMSG("Request timed out.");
    }

	// Wait until a connection is established.
    poll();
	godot::HTTPClient::Status status = get_status();

    Error err;
    PackedByteArray response_data;
	if(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		return;
	}
    else if(status == HTTPClient::STATUS_CONNECTED){
	    err = make_request(JSON::stringify(http_request_body));

        if(err != Error::OK){
            HTTPClient::close();
            pending_request = false;
            ERR_FAIL_EDMSG("Error sending request.");
        }
    }
	else if(status == HTTPClient::STATUS_REQUESTING){
		return;
	}
    else if(status == HTTPClient::STATUS_BODY){
        // Collect the response body.
        while(status == HTTPClient::STATUS_BODY){
            response_data.append_array(read_response_body_chunk());
            HTTPClient::poll();
            status = get_status();
        }

        // Parse the result json.
        JSON json;
        err = json.parse(response_data.get_string_from_utf8());
        if(err != Error::OK){
            HTTPClient::close();
            pending_request = false;
            ERR_FAIL_EDMSG("Error getting response data.");
        }

        // Return if response is not ours.
        Dictionary json_data = json.get_data();
        if(!json_data.has("id")){
            return;
        }
        if((unsigned int)json_data["id"] != local_rpc_id){
            return;
        }

        HTTPClient::close();
        pending_request = false;

        Array params;
        params.append(json.get_data());

        http_callback.callv(params);
    }
    else{
        return;
    }
#else
    const String poll_script = "try{if(Module.solanaClientReq{0}.readyState == 4){Module.solanaClientReq{0}.responseText}else{''}}catch{''}";
    Array format_params;
    format_params.append(local_rpc_id);

    String result = JavaScriptBridge::get_singleton()->eval(poll_script.format(format_params));
    if(!result.is_empty()){
        Array params;
        Dictionary json_data = JSON::parse_string(result);

        params.append(json_data);
        http_callback.callv(params);
        const String reset_script = "delete Module.solanaClientReq{0};";
        JavaScriptBridge::get_singleton()->eval(reset_script.format(format_params));
    }
#endif
}

void WsRpcCall::_bind_methods(){

}

bool WsRpcCall::is_pending(){
    return pending_request;
}

void WsRpcCall::poll_ws_request(){
    WebSocketPeer::poll();
    WebSocketPeer::State state = get_ready_state();
    switch(state){
        case WebSocketPeer::STATE_OPEN:
            while(get_available_packet_count()){
                const PackedByteArray packet_data = get_packet();
                process_package(packet_data);
            }
            if(!ws_request_queue.empty()){
                send_text(ws_request_queue.front());
                ws_request_queue.pop();
            }
        break;
        default:
        break;
    }
}

Dictionary SolanaClient::quick_http_request(const Dictionary& request_body, const Callable& callback){
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
        create_http_call()->asynchronous_request(request_body, parsed_url);
        return Dictionary();
    }
    else{
        return create_http_call()->synchronous_request(request_body, parsed_url);
    }
}

void WsRpcCall::process_package(const PackedByteArray& packet_data){
    Dictionary json = JSON::parse_string(packet_data.get_string_from_ascii());
    const Variant result = json["result"];

    if(json.has("method")){
        int subscription = ((Dictionary)((Dictionary)json)["params"])["subscription"];
        for(unsigned int i = 0; i < callbacks.size(); i++){
            if(callbacks[i].first == subscription){
                Array args;
                args.append(((Dictionary)json)["params"]);
                pending_request = false;
                callbacks[i].second.callv(args);
            }
        }

        return;
    }
    else if(result.get_type() == Variant::BOOL){
        return;
    }

    if(result.get_type() != Variant::FLOAT){
        gdextension_interface_print_warning("Web socket failed", "process_package", __FILE__, __LINE__, false);
        
        int index = callbacks.size() - 1;
        for(int i = callbacks.size() - 1; i > 0; i--){
            if(callbacks[i - 1].first == 0){
                index--;
            }
        }

        if(index < callbacks.size() && index > 0){
            callbacks.erase(callbacks.begin() + index);
            method_names.erase(method_names.begin() + index);
        }
        
        return;
    }

    // Find lowest uninitialized callback.
    int index = callbacks.size() - 1;
    for(int i = callbacks.size() - 1; i > 0; i--){
        if(callbacks[i - 1].first == 0){
            index--;
        }
    }
    callbacks[index].first = (int) result;
}

void WsRpcCall::unsubscribe_all(const Callable &callback){
    for(unsigned int i = 0; i < callbacks.size(); i++){
        if(callbacks[i].second == callback && callbacks[i].first != 0){
            Array params;
            params.append(callbacks[i].first);
            ws_request_queue.push(JSON::stringify(SolanaClient::make_rpc_dict(method_names[i], params)));

            callbacks.erase(callbacks.begin() + i);
            method_names.erase(method_names.begin() + i);
            i--;
        }
    }
}

void WsRpcCall::connect_ws(const String& url){
    if(get_ready_state() == WebSocketPeer::STATE_CLOSED){
        connect_to_url(url);
    }
}

void SolanaClient::response_callback(const Dictionary &params){
    pending_request = false;
    emit_signal("http_response_received", params);
}

Dictionary SolanaClient::get_latest_blockhash(){
    Array params;
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getLatestBlockhash", params));
}

Dictionary SolanaClient::get_block_production(){
    Array params;
    append_commitment(params);
    append_identity(params);
    append_slot_range(params);

    return quick_http_request(make_rpc_dict("getBlockProduction", params));
}

Dictionary SolanaClient::get_account_info(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(make_rpc_dict("getAccountInfo", params));
}


Dictionary SolanaClient::get_balance(const String& account){
    Array params;
    params.append(account);
    append_commitment(params);
    append_encoding(params);
    append_min_context_slot(params);
    append_account_filter(params);

    return quick_http_request(make_rpc_dict("getBalance", params));
}

Dictionary SolanaClient::get_block(uint64_t slot, const String& detail){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_encoding(params);
    append_transaction_detail(params);
    append_max_transaction_version(params);
    append_rewards(params);

    return quick_http_request(make_rpc_dict("getBlock", params));
}

Dictionary SolanaClient::get_block_height(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getBlockHeight", params));
}

Dictionary SolanaClient::get_block_commitment(uint64_t slot_number){
    Array params;
    params.append(slot_number);

    return quick_http_request(make_rpc_dict("getBlockCommitment", params));
}

Dictionary SolanaClient::get_blocks(uint64_t start_slot, const Variant& end_slot){
    Array params;
    params.append(start_slot);
    if(end_slot.get_type() == Variant::INT){
        params.append(end_slot);
    }
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getBlocks", params));
}

Dictionary SolanaClient::get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot){
    Array params;
    params.append(start_slot);
    params.append(end_slot);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getBlocksWithLimit", params));
}

Dictionary SolanaClient::get_block_time(uint64_t slot){
    Array params;
    params.append(slot);

    return quick_http_request(make_rpc_dict("getBlockTime", params));
}

Dictionary SolanaClient::get_cluster_nodes(){
    return quick_http_request(make_rpc_dict("getClusterNodes", Array()));
}

Dictionary SolanaClient::get_epoch_info(){
    Array params;
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getEpochInfo", params));
}

Dictionary SolanaClient::get_epoch_schedule(){
    return quick_http_request(make_rpc_dict("getEpochSchedule", Array()));
}

Dictionary SolanaClient::get_fee_for_message(const String& encoded_message){
    Array params;
    params.append(encoded_message);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getFeeForMessage", params));
}

Dictionary SolanaClient::get_first_available_block(){
    return quick_http_request(make_rpc_dict("getFirstAvailableBlock", Array()));
}

Dictionary SolanaClient::get_genesis_hash(){
    return quick_http_request(make_rpc_dict("getGenesisHash", Array()));
}

Dictionary SolanaClient::get_health(){
    return quick_http_request(make_rpc_dict("getHealth", Array()));
}

Dictionary SolanaClient::get_highest_snapshot_slot(){
    return quick_http_request(make_rpc_dict("getHighestSnapshotSlot", Array()));
}

Dictionary SolanaClient::get_identity(){
    return quick_http_request(make_rpc_dict("getIdentity", Array()));
}

Dictionary SolanaClient::get_inflation_governor(){
    Array params;
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getInflationGovernor", params));
}

Dictionary SolanaClient::get_inflation_rate(){
    return quick_http_request(make_rpc_dict("getInflationRate", Array()));
}

Dictionary SolanaClient::get_inflation_reward(const PackedStringArray accounts, const Variant& epoch){
    Array params;
    params.append(accounts);
    append_commitment(params);
    if(epoch.get_type() == Variant::INT){
        add_to_param_dict(params, "epoch", epoch);
    }
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getInflationReward", params));
}

Dictionary SolanaClient::get_largest_accounts(const String& filter){
    Array params;
    append_commitment(params);
    if(!filter.is_empty()){
        add_to_param_dict(params, "filter", filter);
    }

    return quick_http_request(make_rpc_dict("getLargestAccounts", params));
}

Dictionary SolanaClient::get_leader_schedule(const Variant& slot){
    Array params;
    params.append(slot);
    append_commitment(params);
    append_identity(params);

    return quick_http_request(make_rpc_dict("getLeaderSchedule", params));
}

Dictionary SolanaClient::get_max_retransmit_slot(){
    return quick_http_request(make_rpc_dict("getMaxRetransmitSlot", Array()));
}

Dictionary SolanaClient::get_max_shred_insert_slot(){
    return quick_http_request(make_rpc_dict("getMaxShredInsertSlot", Array()));
}

Dictionary SolanaClient::get_minimum_balance_for_rent_extemption(uint64_t data_size){
    Array params;
    params.append(data_size);
    append_commitment(params);
    
    return quick_http_request(make_rpc_dict("getMinimumBalanceForRentExemption", params));
}

Dictionary SolanaClient::get_multiple_accounts(const PackedStringArray accounts){
    Array params;
    params.append(accounts);
    append_commitment(params);
    append_min_context_slot(params);
    append_account_filter(params);
    append_encoding(params);

    return quick_http_request(make_rpc_dict("getMultipleAccounts", params));
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

    return quick_http_request(make_rpc_dict("getProgramAccounts", params));
}

Dictionary SolanaClient::get_recent_performance_samples(){
    Array params;
    append_limit(params);

    return quick_http_request(make_rpc_dict("getRecentPerformanceSamples", params));
}

Dictionary SolanaClient::get_recent_prioritization_fees(PackedStringArray account_addresses){
    Array params;
    if(!account_addresses.is_empty()){
        params.append(account_addresses);
    }

    return quick_http_request(make_rpc_dict("getRecentPrioritizationFees", params));
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

    return quick_http_request(make_rpc_dict("getSignaturesForAddress", params));
}

Dictionary SolanaClient::get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history){
    Array params;

    params.append(signatures);
    add_to_param_dict(params, "searchTransactionHistory", search_transaction_history);

    return quick_http_request(make_rpc_dict("getSignatureStatuses", params));
}

Dictionary SolanaClient::get_slot(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getSlot", params));
}

Dictionary SolanaClient::get_slot_leader(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getSlotLeader", params));
}

Dictionary SolanaClient::get_slot_leaders(const Variant& start_slot, const Variant& slot_limit){
    Array params;

    if(start_slot.get_type() == Variant::NIL){
        params.append(start_slot);
    }
    if(slot_limit.get_type() == Variant::NIL){
        params.append(slot_limit);
    }

    return quick_http_request(make_rpc_dict("getSlotLeaders", params));
}

Dictionary SolanaClient::get_stake_activation(const String& account){
    Array params;

    params.append(account);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getStakeActivation", params));
}

Dictionary SolanaClient::get_stake_minimum_delegation(){
    Array params;

    append_commitment(params);

    return quick_http_request(make_rpc_dict("getStakeMinimumDelegation", params));
}

Dictionary SolanaClient::get_supply(bool exclude_non_circulating){
    Array params;

    append_commitment(params);
    add_to_param_dict(params, "excludeNonCirculatingAccountsList", exclude_non_circulating);

    return quick_http_request(make_rpc_dict("getSupply", params));
}

Dictionary SolanaClient::get_token_account_balance(const String& token_account){
    Array params;

    params.append(token_account);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenAccountBalance", params));
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

    return quick_http_request(make_rpc_dict("getTokenAccountsByDelegate", params));
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

    return quick_http_request(make_rpc_dict("getTokenAccountsByOwner", params));
}

Dictionary SolanaClient::get_token_largest_account(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenLargestAccounts", params));
}

Dictionary SolanaClient::get_token_supply(const String& token_mint){
    Array params;

    params.append(token_mint);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("getTokenSupply", params));
}

Dictionary SolanaClient::get_transaction(const String& signature){
    Array params;

    params.append(signature);
    append_commitment(params);
    append_max_transaction_version(params);
    append_encoding(params);

    return quick_http_request(make_rpc_dict("getTransaction", params));
}

Dictionary SolanaClient::get_transaction_count(){
    Array params;

    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("getTransactionCount", params));
}

Dictionary SolanaClient::get_version(){
    return quick_http_request(make_rpc_dict("getVersion", Array()));
}

Dictionary SolanaClient::get_vote_accounts(const String& vote_pubkey, bool keep_unstaked_delinquents){
    Array params;

    append_commitment(params);
    if(!vote_pubkey.is_empty()){
        add_to_param_dict(params, "votePubkey", vote_pubkey);
    }
    add_to_param_dict(params, "keepUnstakedDelinquents", keep_unstaked_delinquents);

    return quick_http_request(make_rpc_dict("getVoteAccounts", params));
}

Dictionary SolanaClient::is_blockhash_valid(const String &blockhash){
    Array params;

    params.append(blockhash);
    append_commitment(params);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("isBlockhashValid", params));
}

Dictionary SolanaClient::minimum_ledger_slot(){
    return quick_http_request(make_rpc_dict("minimumLedgerSlot", Array()));
}

Dictionary SolanaClient::request_airdrop(const String& address, uint64_t lamports){
    Array params;

    params.append(address);
    params.append(lamports);
    append_commitment(params);

    return quick_http_request(make_rpc_dict("requestAirdrop", params));
}

Dictionary SolanaClient::send_transaction(const String& encoded_transaction, uint64_t max_retries, bool skip_preflight){
    Array params;

    params.append(encoded_transaction);
    append_encoding(params);
    add_to_param_dict(params, "skipPreflight", skip_preflight);
    add_to_param_dict(params, "preflightCommitment", commitment);
    add_to_param_dict(params, "maxRetries", max_retries);
    append_min_context_slot(params);

    return quick_http_request(make_rpc_dict("sendTransaction", params));
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

    return quick_http_request(make_rpc_dict("simulateTransaction", params));
}

void SolanaClient::account_subscribe(const Variant &account_key, const Callable &callback){
    Array params;
    params.append(Pubkey(account_key).to_string());
    add_to_param_dict(params, "commitment", commitment);

    create_ws_call()->enqueue_ws_request(make_rpc_dict("accountSubscribe", params), callback, get_real_ws_url());
}

void SolanaClient::signature_subscribe(const String &signature, const Callable &callback, const String &commitment){
    Array params;
    params.append(signature);
    add_to_param_dict(params, "commitment", commitment);
    create_ws_call()->enqueue_ws_request(make_rpc_dict("signatureSubscribe", params), callback, get_real_ws_url());
}

void SolanaClient::program_subscribe(const String &program_id, const Callable &callback){
    Array params;
    params.append(program_id);
    append_commitment(params);
    append_account_filter(params);
    append_encoding(params);

    create_ws_call()->enqueue_ws_request(make_rpc_dict("programSubscribe", params), callback, get_real_ws_url());
}

void SolanaClient::root_subscribe(const Callable &callback){
    create_ws_call()->enqueue_ws_request(make_rpc_dict("rootSubscribe", Array()), callback, get_real_ws_url());
}

void SolanaClient::slot_subscribe(const Callable &callback){
    create_ws_call()->enqueue_ws_request(make_rpc_dict("slotSubscribe", Array()), callback, get_real_ws_url());
}

void SolanaClient::unsubscribe_all(const Callable &callback){
    for(unsigned int i = 0; i < rpc_calls.size(); i++){
        // TODO: Remove RpcCall's ability to do multiple ws requests.
        if(((Object*)rpc_calls[i])->get_class() == "WsRpcCall"){
            Object::cast_to<WsRpcCall>(rpc_calls[i])->unsubscribe_all(callback);
        }
    }
}

void SolanaClient::_bind_methods(){
    ClassDB::add_signal("SolanaClient", MethodInfo("socket_response_received"));
    ClassDB::add_signal("SolanaClient", MethodInfo("http_response_received", PropertyInfo(Variant::DICTIONARY, "response")));

    ClassDB::bind_static_method("SolanaClient", D_METHOD("assemble_url", "url"), &SolanaClient::assemble_url);
    ClassDB::bind_static_method("SolanaClient", D_METHOD("get_next_request_identifier"), &SolanaClient::get_next_request_identifier);

    ClassDB::bind_method(D_METHOD("response_callback", "params"), &SolanaClient::response_callback);
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
    for(unsigned int i = 0; i < rpc_calls.size(); i++){
        if(((Object*)rpc_calls[i])->get_class() == "HttpRpcCall"){
            HttpRpcCall *http_call = Object::cast_to<HttpRpcCall>(rpc_calls[i]);
            http_call->poll_http_request(delta);
            if(!http_call->is_pending()){
                rpc_calls.erase(i);
            }
        }
        else if(((Object*)rpc_calls[i])->get_class() == "WsRpcCall"){
            WsRpcCall *ws_call = Object::cast_to<WsRpcCall>(rpc_calls[i]);
            ws_call->poll_ws_request();
            if(!ws_call->is_pending()){
                rpc_calls.erase(i);
            }
        }
        else{
            ERR_FAIL_MSG("Internal bug, Please report");
        } 
    }
}

void SolanaClient::_ready(){
}

SolanaClient::SolanaClient(){
    transaction_detail = "full";
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