#include "solana_sdk.hpp"

#include "../wrapper/wrapper.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/tls_options.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <utils.hpp>

using namespace godot;

using internal::gde_interface;

String SolanaSDK::url = "https://api.testnet.solana.com";

void SolanaSDK::_bind_methods() {
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_encode", "input"), &SolanaSDK::bs58_encode);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_decode", "input"), &SolanaSDK::bs58_decode);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("get_latest_blockhash"), &SolanaSDK::get_latest_blockhash);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("set_url", "url"), &SolanaSDK::set_url);
	ClassDB::bind_static_method("SolanaSDK", D_METHOD("get_url"), &SolanaSDK::get_url);
}

SolanaSDK::SolanaSDK() {
	url = TESTNET_URL;
}

Dictionary SolanaSDK::quick_http_request(const String& request_body){
	const int32_t POLL_DELAY_MSEC = 10;

	// Set headers
	PackedStringArray http_headers;
	http_headers.append("Content-Type: application/json");
	http_headers.append("Accept-Encoding: json");
	
	// Connect to RPC API URL.
	HTTPClient handler;
	Error err = handler.connect_to_host(url, 443, TLSOptions::client_unsafe());

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
		gde_interface->print_error("Error sending request.", "quick_http_request", "solana_sdk.cpp", __LINE__, false);
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
		status = handler.get_status();
	}

	handler.close();

	// Parse out the blockhash.
	JSON json;
	err = json.parse(response_data.get_string_from_utf8());

	if(err != Error::OK){
		gde_interface->print_error("Error getting response data.", "quick_http_request", "solana_sdk.cpp", __LINE__, false);
		return Dictionary();
	}

	return json.get_data();
}

PackedByteArray SolanaSDK::bs58_decode(String str){
	PackedByteArray result;
	char buffer[str.length() * 2];
	unsigned int resultlen = ::bs58_decode((const char*) str.to_utf8_buffer().ptr(), str.length(), buffer);
	result.resize(resultlen);
	for(unsigned int i = 0; i < resultlen; i++){
		result[i] = buffer[i];
	}
	return result;
}

String SolanaSDK::bs58_encode(PackedByteArray input){
	PackedByteArray result;
	char buffer[input.size() * 2];
	unsigned int resultlen = ::bs58_encode((const char*) input.ptr(), input.size(), buffer);
	result.resize(resultlen);
	for(unsigned int i = 0; i < resultlen; i++){
		result[i] = buffer[i];
	}
	return result.get_string_from_utf8();
}

String SolanaSDK::get_latest_blockhash(){
	const godot::String REQUEST_DATA = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"getLatestBlockhash\",\"params\":[{\"commitment\":\"finalized\"}]}";

	Dictionary data = quick_http_request(REQUEST_DATA);

	if(!data.has("result")){
		gde_interface->print_error("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}
	data = data["result"];
	if(!data.has("value")){
		gde_interface->print_error("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}
	data = data["value"];
	if(!data.has("blockhash")){
		gde_interface->print_error("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}

	return data["blockhash"];
}

Variant SolanaSDK::send_transaction(const String& transaction){
	const godot::String REQUEST_DATA = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"sendTransaction\",\"params\":[\"" + transaction + "\",{\"encoding\":\"base64\"}]}";

	return quick_http_request(REQUEST_DATA);
}

void SolanaSDK::set_url(const String& url){
	SolanaSDK::url = url;
}

String SolanaSDK::get_url(){
	return SolanaSDK::url;
}

SolanaSDK::~SolanaSDK() {
    // add your cleanup here
}
