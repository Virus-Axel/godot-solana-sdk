#include "solana_sdk.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/tls_options.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <utils.hpp>

using namespace godot;

const char BASE_58_MAP[] = {
	'1', '2', '3', '4', '5', '6', '7', '8',
	'9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
	'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
	'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z' };

const unsigned char ALPHABET_MAP[] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8, 255, 255, 255, 255, 255, 255,
	255,  9, 10, 11, 12, 13, 14, 15, 16, 255, 17, 18, 19, 20, 21, 255,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 255, 255, 255, 255, 255,
	255, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 255, 44, 45, 46,
	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

using internal::gdextension_interface_print_warning;

std::string SolanaSDK::url;

const std::string SolanaSDK::SPL_TOKEN_ADDRESS = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";
const std::string SolanaSDK::SPL_ASSOCIATED_TOKEN_ADDRESS = "ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL";

void SolanaSDK::_bind_methods() {
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_encode", "input"), &SolanaSDK::bs58_encode);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_decode", "input"), &SolanaSDK::bs58_decode);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("get_latest_blockhash"), &SolanaSDK::get_latest_blockhash);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("set_url", "url"), &SolanaSDK::set_url);
	ClassDB::bind_static_method("SolanaSDK", D_METHOD("get_url"), &SolanaSDK::get_url);
}

SolanaSDK::SolanaSDK() {
	url = TESTNET_URL.ascii();
}

Dictionary SolanaSDK::quick_http_request(const String& request_body){
	const int32_t POLL_DELAY_MSEC = 10;

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
}


PackedByteArray SolanaSDK::bs58_decode(String str){
	PackedByteArray result;
	if (str.length() == 0){
		return result;
    }
	
	// Worst case size
	result.resize(str.to_utf8_buffer().size() * 2);
	result[0] = 0;

	int resultlen = 1;
	for (unsigned int i = 0; i < str.to_utf8_buffer().size(); i++){
		unsigned int carry = (unsigned int) ALPHABET_MAP[str.to_utf8_buffer()[i]];
		if (carry == 255){
			return PackedByteArray();
        }
		for (int j = 0; j < resultlen; j++){
			carry += (unsigned int) (result[j]) * 58;
			result[j] = (char) (carry & 0xff);
			carry >>= 8;
        }

		while (carry > 0){
			result[resultlen++] = carry & 0xff;
			carry >>= 8;
        }
    }

    for (int i = 0; i < str.to_utf8_buffer().size() && str[i] == '1'; i++)
        result[resultlen++] = 0;

	int i = resultlen - 1;
	int z = (resultlen >> 1) + (resultlen & 1);
	
	while(i >= z){
		int k = result[i];
		result[i] = result[resultlen - i - 1];
		result[resultlen - i - 1] = k;
		i -= 1;
    }

	return result.slice(0, resultlen);
}

String SolanaSDK::bs58_encode(PackedByteArray input){
	PackedByteArray encoded;
	
	// Calculate the total length of the result
	unsigned int encoded_size = (input.size() * 138 / 100) + 1;
	
	encoded.resize(encoded_size);
	unsigned int digit_size = 1;
	
	for(unsigned int i = 0; i < input.size(); i++){
		int carry = int(input[i]);
		
		for (unsigned int j = 0; j < digit_size; j++){
			carry = carry + int(encoded[j] << 8);
			encoded[j] = (carry % 58) % 256;
			carry /= 58;
        }
			
		while (carry){
			encoded[digit_size] = (carry % 58) % 256;
			digit_size += 1;
			carry /= 58;
        }
    }

	String result;
	
	for (unsigned int i = 0; i < input.size() - 1; i++){
		if (input[i]){
            break;
        }
		result += BASE_58_MAP[0];
    }
		
	for (unsigned int i = 0; i < digit_size; i++){
		int map_index = encoded[digit_size - 1 - i];
		result += (BASE_58_MAP[map_index]);
    }

	return result;
}

String SolanaSDK::get_latest_blockhash(){
	const godot::String REQUEST_DATA = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"getLatestBlockhash\",\"params\":[{\"commitment\":\"finalized\"}]}";

	Dictionary data = quick_http_request(REQUEST_DATA);

	if(!data.has("result")){
		gdextension_interface_print_warning("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}
	data = data["result"];
	if(!data.has("value")){
		gdextension_interface_print_warning("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}
	data = data["value"];
	if(!data.has("blockhash")){
		gdextension_interface_print_warning("Unexpected response.", "get_latest_blockhash", "solana_sdk.cpp", __LINE__, false);
		return "";
	}

	return data["blockhash"];
}

Variant SolanaSDK::send_transaction(const String& transaction){
	const godot::String REQUEST_DATA = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"sendTransaction\",\"params\":[\"" + transaction + "\",{\"encoding\":\"base64\"}]}";

	return quick_http_request(REQUEST_DATA);
}

void SolanaSDK::set_url(const String& url){
	SolanaSDK::url = url.ascii();
}

String SolanaSDK::get_url(){
	return String(SolanaSDK::url.c_str());
}

SolanaSDK::~SolanaSDK() {
    // add your cleanup here
}
