#ifndef SOLANA_SDK_RPC_SINGLE_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_SINGLE_HTTP_REQUEST_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

namespace godot {

typedef struct{
    String request;
    Dictionary parsed_url;
    float timeout;
    int request_identifier;
    Callable callback;
} RequestData;

class RpcSingleHttpRequestClient : public HTTPClient{
    GDCLASS(RpcSingleHttpRequestClient, HTTPClient)
private:
    std::queue<RequestData> request_queue;
    unsigned int local_rpc_id = 0;

    void update_timeouts(const float delta);
    bool is_pending() const;
    bool has_request() const;
    bool completed() const;
    bool is_response_valid(const Dictionary& response) const;

    Error connect_to();
    Error send_next_request();
    void finalize_request(const Dictionary& response);

protected:

    static void _bind_methods();
public:
    void process(const float delta);

    Dictionary synchronous_request(const Dictionary& request_body, const Dictionary& parsed_url);
    void asynchronous_request(const Dictionary& request_body, Dictionary parsed_url, const Callable &callback, float timeout = 20.0F);
};

}

#endif