#ifndef SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP

#include <godot_cpp/classes/http_client.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <queue>

#include "rpc_single_http_request_client.hpp"

namespace godot {

/**
 * @class RpcMultiHttpRequestClient
 * @brief Used to send multiple request simultaneously.
 * 
 * This class can be used to send asynchronous requests. It creates instances of
 * RpcSingleHttpRequestClient and processes the list of clients synchronously.
 */
class RpcMultiHttpRequestClient : public HTTPClient {
	GDCLASS(RpcMultiHttpRequestClient, HTTPClient)
private:
	TypedArray<RpcSingleHttpRequestClient> requests;
	unsigned int last_processed_frame = 0;

protected:

	/**
	 * @bindmethods{RpcMultiHttpRequestClient, Node}
	 */
	static void _bind_methods();

public:
	/**
	 * @brief @_process
	 */
	void process(double delta);

	/**
	 * @brief Initiate an asynchronous request.
	 * 
	 * @param request_body Request to send.
	 * @param parsed_url URL of the request.
	 * @param callback Callback to call on response received.
	 * @param timeout Timeout of request.
	 */
	void asynchronous_request(const Dictionary &request_body, const Dictionary &parsed_url, const Callable &callback, float timeout = 20.0F);
};

} //namespace godot

#endif