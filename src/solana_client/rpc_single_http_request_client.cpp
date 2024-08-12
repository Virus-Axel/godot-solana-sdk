#include "rpc_single_http_request_client.hpp"
#include "solana_client.hpp"

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <godot_cpp/classes/os.hpp>

namespace godot {

const int32_t POLL_DELAY_MSEC = 100;

void RpcSingleHttpRequestClient::update_timeouts(const float delta){
    request_queue.front().timeout -= delta;

    // Remove timed out requests.
    if(request_queue.front().timeout < 0.0){
        finalize_faulty();
        ERR_FAIL_EDMSG("Request timed out.");
    }
}

bool RpcSingleHttpRequestClient::is_pending() const{
    return (get_status() != Status::STATUS_DISCONNECTED);
}

bool RpcSingleHttpRequestClient::has_request() const{
    return request_queue.size();
}

bool RpcSingleHttpRequestClient::is_completed() const{
    return !is_pending() && !has_request();
}

bool RpcSingleHttpRequestClient::is_response_valid(const Dictionary& response) const{
    // Check if response is ours.
    if(!response.has("id")){
        return false;
    }
    if((unsigned int)response["id"] != request_queue.front().request_identifier){
        return false;
    }

    return true;
}

Error RpcSingleHttpRequestClient::connect_to(){
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

    return connect_to_host(connect_url, port);
}

Error RpcSingleHttpRequestClient::send_next_request(){
    response_data.clear();

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

    return request(godot::HTTPClient::METHOD_POST, path, http_headers, request_body);
}

void RpcSingleHttpRequestClient::finalize_faulty(){
    finalize_request(Dictionary());
}

void RpcSingleHttpRequestClient::finalize_request(const Dictionary& response){
    HTTPClient::close();

    if(request_queue.front().callback.is_valid()){
        Array params;
        params.append(response);
        request_queue.front().callback.callv(params);
    }

    request_queue.pop();
}

void RpcSingleHttpRequestClient::process(const float delta){
    if(is_completed()){
        return;
    }

    // Make new connection if queue is not empty
    if(!is_pending()){
        Error err = connect_to();
        if(err != Error::OK){
            finalize_faulty();
            ERR_FAIL_EDMSG("Failed to connect to RPC node.");
        }
    }
    else{
        update_timeouts(delta);
    }

    poll();
	godot::HTTPClient::Status status = get_status();
    
    // Wait until a connection is established.
	if(status == HTTPClient::STATUS_CONNECTING || status == HTTPClient::STATUS_RESOLVING){
		return;
	}
    else if(status == HTTPClient::STATUS_CONNECTED){
	    Error err = send_next_request();

        if(err != Error::OK){
            finalize_faulty();
            ERR_FAIL_EDMSG("Error sending request.");
        }
    }
	else if(status == HTTPClient::STATUS_REQUESTING){
		return;
	}
    else if(status == HTTPClient::STATUS_BODY){
        // Collect the response body.
        response_data.append_array(read_response_body_chunk());
        status = get_status();

        // If there is more data, return and read it next frame.
        if(status == HTTPClient::STATUS_BODY){
            return;
        }

        // Parse the result json.
        JSON json;
        Error err = json.parse(response_data.get_string_from_utf8());

        if(err != Error::OK){
            finalize_faulty();
            ERR_FAIL_EDMSG("Error getting response data.");
        }

        Dictionary json_data = json.get_data();
        
        if(!is_response_valid(json_data)){
            // Request could be from another solana client. Keep processing request.
            response_data.clear();

            return;
        }

        finalize_request(json_data);
    }
    else{
        finalize_faulty();
        ERR_PRINT_ONCE_ED("Cannot connect");
        return;
    }
}

void RpcSingleHttpRequestClient::_bind_methods(){
}

void RpcSingleHttpRequestClient::asynchronous_request(const Dictionary& request_body, Dictionary parsed_url, const Callable &callback, float timeout){
    RequestData data = {request_body, parsed_url, timeout, request_body["id"], callback};
    request_queue.push(data);
}

};