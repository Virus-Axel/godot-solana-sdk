#include "rpc_single_http_request_client.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/http_client.hpp"
#include "godot_cpp/classes/java_script_bridge.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

const int32_t POLL_DELAY_MSEC = 100;

void RpcSingleHttpRequestClient::process_message_sending() {
	const Error err = send_next_request();

	if (err != Error::OK) {
		finalize_faulty(err);
		ERR_FAIL_EDMSG_CUSTOM("Error sending request.");
	}
}

void RpcSingleHttpRequestClient::process_body() {
	// Collect the response body.
	response_data.append_array(read_response_body_chunk());
	const godot::HTTPClient::Status status = get_status();

	// If there is more data, return and read it next frame.
	if (status == HTTPClient::STATUS_BODY) {
		return;
	}

	// Parse the result json.
	const String response_string = response_data.get_string_from_utf8();
	const Variant json_data = JSON::parse_string(response_string);

	if (json_data.get_type() != Variant::DICTIONARY) {
		UtilityFunctions::print_verbose(VERBOSE_LOG_PREFIX, response_string);
		finalize_faulty(Error::ERR_PARSE_ERROR);
		ERR_FAIL_EDMSG_CUSTOM("Error getting response data.");
	}

	if (!is_response_valid(json_data)) {
		// Request could be from another solana client. Keep processing request.
		response_data.clear();

		return;
	}

	finalize_request(json_data);
}

void RpcSingleHttpRequestClient::initiate_connection() {
	const Error err = connect_to();
	if (err != Error::OK) {
		finalize_faulty(err);
		ERR_FAIL_EDMSG_CUSTOM("Failed to connect to RPC node.");
	}
}

void RpcSingleHttpRequestClient::update_timeouts(const double delta) {
	request_queue.front().timeout -= delta;
}

bool RpcSingleHttpRequestClient::is_pending() const {
	return (get_status() != Status::STATUS_DISCONNECTED);
}

bool RpcSingleHttpRequestClient::has_request() const {
	return !request_queue.empty();
}

bool RpcSingleHttpRequestClient::is_completed() const {
	return !is_pending() && !has_request();
}

void RpcSingleHttpRequestClient::set_skip_id(bool skip_id) {
	this->skip_id = skip_id;
}

bool RpcSingleHttpRequestClient::is_response_valid(const Dictionary &response) const {
	if (skip_id) {
		return true;
	}

	// Check if response is ours.
	if (!response.has("id")) {
		return false;
	}
	if (static_cast<unsigned int>(response["id"]) != request_queue.front().request_identifier) {
		return false;
	}

	return true;
}

bool RpcSingleHttpRequestClient::is_timed_out() const {
	return request_queue.front().timeout < 0.0;
}

Error RpcSingleHttpRequestClient::connect_to() {
	// Do nothing if already connected.
	if (get_status() != Status::STATUS_DISCONNECTED) {
		return Error::OK;
	}

	Dictionary url = request_queue.front().parsed_url;

	// Save port and remove it from URL.
	const int32_t port = url["port"];
	url.erase("port");

	// Format URL string
	String connect_url = "https";
	if (url.has("scheme")) {
		connect_url = url["scheme"];
	}
	connect_url += "://" + static_cast<String>(url["host"]);

	return connect_to_host(connect_url, port);
}

Error RpcSingleHttpRequestClient::send_next_request() {
	response_data.clear();

	// Set headers.
	PackedStringArray http_headers;
	http_headers.append("Content-Type: application/json");
	http_headers.append("Accept-Encoding: json");

	// Find path from url.
	String path = "/";
	const Dictionary &url = request_queue.front().parsed_url;
	if (url.has("path")) {
		path = url["path"];
	}
	if (url.has("query")) {
		path += String("?") + static_cast<String>(url["query"]);
	}
	if (url.has("fragment")) {
		path += String("#") + static_cast<String>(url["fragment"]);
	}

	// Make a POST request.
	const String request_body = JSON::stringify(request_queue.front().request);

	return request(godot::HTTPClient::METHOD_POST, path, http_headers, request_body);
}

void RpcSingleHttpRequestClient::finalize_faulty(const Error error) {
	HTTPClient::close();
	if (request_queue.front().error_callback.is_valid()) {
		const Array params = Array::make(error);
		request_queue.front().error_callback.callv(params);
	}
	request_queue.pop();
}

void RpcSingleHttpRequestClient::finalize_request(const Dictionary &response) {
	HTTPClient::close();

	if (request_queue.front().callback.is_valid()) {
		const Array params = Array::make(response);
		request_queue.front().callback.callv(params);
	}

	request_queue.pop();
}

void RpcSingleHttpRequestClient::process(const double delta) {
	if (is_completed()) {
		return;
	}

	// Make new connection if queue is not empty
	if (!is_pending()) {
		initiate_connection();
	} else {
		update_timeouts(delta);

		// Remove timed out requests.
		if (is_timed_out()) {
			finalize_faulty(Error::ERR_TIMEOUT);
			ERR_FAIL_EDMSG_CUSTOM("Request timed out.");
		}
	}

	poll();
	const godot::HTTPClient::Status status = get_status();

	switch (status) {
		// Wait until a connection is established.
		case HTTPClient::STATUS_CONNECTING:
		case HTTPClient::STATUS_RESOLVING:
		case HTTPClient::STATUS_REQUESTING:
			return;
			break;

		case HTTPClient::STATUS_CONNECTED:
			process_message_sending();
			break;
		case HTTPClient::STATUS_BODY:
			process_body();
			break;
		default:
			finalize_faulty(Error::ERR_CANT_CONNECT);
			ERR_PRINT_ONCE_ED_CUSTOM("Cannot connect");
			return;
			break;
	}
}

void RpcSingleHttpRequestClient::_bind_methods() {
}

void RpcSingleHttpRequestClient::asynchronous_request(const Dictionary &request_body, const Dictionary &parsed_url, const Callable &callback, const Callable &error_callback, float timeout) {
	RequestData data;
	if (skip_id) {
		data = RequestData{ .request = request_body, .parsed_url = parsed_url, .timeout = timeout, .request_identifier = 0, .callback = callback, .error_callback = error_callback };
	} else {
		data = RequestData{ .request = request_body, .parsed_url = parsed_url, .timeout = timeout, .request_identifier = request_body["id"], .callback = callback, .error_callback = error_callback };
	}

	request_queue.push(data);
}

}; //namespace godot
