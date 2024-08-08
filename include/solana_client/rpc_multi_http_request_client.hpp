#ifndef SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

#include "rpc_single_http_request_client.hpp"

namespace godot {

class RpcMultiHttpRequestClient : public HTTPClient{
    GDCLASS(RpcMultiHttpRequestClient, HTTPClient)
private:
    TypedArray<RpcSingleHttpRequestClient> requests;

protected:
    bool pending_request = false;

    static void _bind_methods();
public:

    void process(float delta);
    void asynchronous_request(const Dictionary& request_body, Dictionary parsed_url, const Callable &callback, float timeout = 20.0F);

};

}

#endif