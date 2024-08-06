#include "rpc_single_http_request_client.hpp"
#include "solana_client.hpp"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <godot_cpp/classes/os.hpp>

namespace godot {

void RpcSingleHttpRequestClient::update_timeouts(const float delta){
    request_queue.front().timeout -= delta;

    // Remove timed out requests.
    if(request_queue.front().timeout < 0.0){
        close();
        request_queue.pop();
        ERR_FAIL_EDMSG("Request timed out.");
    }
}

bool RpcSingleHttpRequestClient::is_pending() const{
    return (get_status() != Status::STATUS_DISCONNECTED);
}

bool RpcSingleHttpRequestClient::has_request() const{
    return request_queue.size();
}

bool RpcSingleHttpRequestClient::completed() const{
    return !is_pending() && !has_request();
}

bool RpcSingleHttpRequestClient::is_response_valid(const Dictionary& response) const{
    // Check if response is ours.
    if(!response.has("id")){
        return false;
    }
    if((unsigned int)response["id"] != request_queue.front().request_identifier){
        std::cout << request_queue.front().request_identifier << std::endl;
        return false;
    }
    return true;
}

Error RpcSingleHttpRequestClient::connect_to(){
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    // Do nothing if already connected.
    if(get_status() != Status::STATUS_DISCONNECTED){
        return OK;
    }

    Dictionary url = request_queue.front().parsed_url;
    
    // Save port and remove it from URL.
    int32_t port = url["port"];
    url.erase("port");

    // Format URL string
    String connect_url = "https";
    if(url.has("scheme")){
        connect_url = url["scheme"];
    }
    connect_url += "://" + (String) url["host"];
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA000000" << std::endl;

    return connect_to_host(connect_url, port);
}

Error RpcSingleHttpRequestClient::send_next_request(){
    // Set headers.
    PackedStringArray http_headers;
    http_headers.append("Content-Type: application/json");
    http_headers.append("Accept-Encoding: json");

    // Find path from url.
    String path = "/";
    const Dictionary& url = request_queue.front().parsed_url;
    if(url.has("path")){
        path = url["path"];
    }
    if(url.has("query")){
        path += String("?") + (String)url["query"];
    }
    if(url.has("fragment")){
        path += String("#") + (String)url["fragment"];
    }

    // Make a POST request.
    const String request_body = JSON::stringify(request_queue.front().request);
    std::cout << "sending: "<<request_body.ascii() << std::endl;
    return request(godot::HTTPClient::METHOD_POST, path, http_headers, request_body);
}

void RpcSingleHttpRequestClient::finalize_request(const Dictionary& response){
    std::cout << "HERE" << std::endl;
    HTTPClient::close();

    if(request_queue.front().callback.is_valid()){
        Array params;
        params.append(response);
        request_queue.front().callback.callv(params);
    }

    request_queue.pop();
}

void RpcSingleHttpRequestClient::process(const float delta){
#ifndef WEB_ENABLED

    if(completed()){
        return;
    }

    // Make new connection if queue is not empty
    if(!is_pending()){
        Error err = connect_to();
        ERR_FAIL_COND_EDMSG(err != Error::OK, "Failed to connect to RPC node.");
    }
    else{
        update_timeouts(delta);
    }

    poll();
	godot::HTTPClient::Status status = get_status();
    PackedByteArray response_data;
    
    // Wait until a connection is established.
	if(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		return;
	}
    else if(status == HTTPClient::STATUS_CONNECTED){
        std::cout << "CONNECTED" << std::endl;
	    Error err = send_next_request();

        if(err != Error::OK){
            HTTPClient::close();
            ERR_FAIL_EDMSG("Error sending request.");
        }
    }
	else if(status == HTTPClient::STATUS_REQUESTING){
		return;
	}
    else if(status == HTTPClient::STATUS_BODY){
        std::cout << "RESPONSE" << std::endl;
        // Collect the response body.
        while(status == HTTPClient::STATUS_BODY){
            response_data.append_array(read_response_body_chunk());
            HTTPClient::poll();
            status = get_status();
        }

        // Parse the result json.
        JSON json;
        Error err = json.parse(response_data.get_string_from_utf8());
        std::cout << response_data.get_string_from_utf8().ascii() << std::endl;
        if(err != Error::OK){
            HTTPClient::close();
            ERR_FAIL_EDMSG("Error getting response data.");
        }

        Dictionary json_data = json.get_data();
        
        if(!is_response_valid(json_data)){
            std::cout << "invalid" << std::endl;
            return;
        }

        finalize_request(json_data);
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
        request_queue.front().callback.callv(params);
        const String reset_script = "delete Module.solanaClientReq{0};";
        JavaScriptBridge::get_singleton()->eval(reset_script.format(format_params));
    }
#endif
}


Dictionary RpcSingleHttpRequestClient::synchronous_request(const Dictionary& request_body, const Dictionary& parsed_url, float timeout){
    #ifndef WEB_ENABLED
	const int32_t POLL_DELAY_MSEC = 100;
    float elapsed_time = 0.0F;

    RequestData data = {request_body, parsed_url, timeout, 0, Callable()};
    request_queue.push(data);

    Error err = connect_to();
    ERR_FAIL_COND_V_EDMSG(err != Error::OK, Dictionary(), "Failed to connect to server.");

	// Wait until a connection is established.
	godot::HTTPClient::Status status = get_status();
	while(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		HTTPClient::poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
        elapsed_time += float(POLL_DELAY_MSEC) / 1000;
		status = get_status();
        if(elapsed_time > request_queue.front().timeout){
            finalize_request(Dictionary());
            ERR_FAIL_V_EDMSG(Dictionary(), "Request timed out.");
        }
	}

	err = send_next_request();
    if(err != OK){
        finalize_request(Dictionary());
        ERR_FAIL_V_EDMSG(Dictionary(), "Error sending request.");
    }

	// Poll until we have a response.
	status = get_status();
	while(status == HTTPClient::STATUS_REQUESTING){
		HTTPClient::poll();
		OS::get_singleton()->delay_msec(POLL_DELAY_MSEC);
        elapsed_time += float(POLL_DELAY_MSEC) / 1000;
		status = get_status();
        if(elapsed_time > request_queue.front().timeout){
            finalize_request(Dictionary());
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
        if(elapsed_time > request_queue.front().timeout){
            finalize_request(Dictionary());
            ERR_FAIL_V_EDMSG(Dictionary(), "Request timed out.");
        }
	}

	finalize_request(Dictionary());


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

    params.append(request_queue.front().timeout * 1000);
    params.append(SolanaClient::assemble_url(parsed_url));
    params.append(request_body);

    Variant result = JavaScriptBridge::get_singleton()->eval(web_script.format(params));

    JSON json;
	Error err = json.parse(result);

#endif

    ERR_FAIL_COND_V_EDMSG(err != Error::OK, Dictionary(), "Error getting response data.");

	return json.get_data();

}

void RpcSingleHttpRequestClient::_bind_methods(){
}

void RpcSingleHttpRequestClient::asynchronous_request(const Dictionary& request_body, Dictionary parsed_url, const Callable &callback, float timeout){
    RequestData data = {request_body, parsed_url, timeout, request_body["id"], callback};
    request_queue.push(data);
}

};