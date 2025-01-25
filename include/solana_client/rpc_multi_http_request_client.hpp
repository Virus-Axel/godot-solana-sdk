#ifndef SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP

#include <godot_cpp/classes/http_client.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <queue>

#include "rpc_single_http_request_client.hpp"

namespace godot {

class RpcMultiHttpRequestClient : public HTTPClient {
	GDCLASS(RpcMultiHttpRequestClient, HTTPClient)
private:
	TypedArray<RpcSingleHttpRequestClient> requests;

protected:
	unsigned int last_processed_frame = 0;
	bool pending_request = false;

	static void _bind_methods();

public:
	void process(double delta);
	void asynchronous_request(const Dictionary &request_body, const Dictionary &parsed_url, const Callable &callback, float timeout = 20.0F);
};

} //namespace godot

#endif