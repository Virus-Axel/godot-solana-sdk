#ifndef SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_MULTI_HTTP_REQUEST_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

namespace godot {

class RpcMultiHttpRequestClient : public HTTPClient{
    GDCLASS(RpcMultiHttpRequestClient, HTTPClient)
private:
    std::queue<String> request_queue;
    std::vector<std::pair<int, Callable>> callbacks;
    
    float elapsed_time = 0;
    float timeout = 20.0;
    Callable http_callback;

    unsigned int local_rpc_id = 0;

    Dictionary  http_request_body;

protected:
    bool pending_request = false;

    static void _bind_methods();
public:

};

}

#endif