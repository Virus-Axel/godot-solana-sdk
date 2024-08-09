#ifndef SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

namespace godot{


typedef struct{
    Dictionary request;
    Dictionary parsed_url;
    float timeout;
    int request_identifier;
    Callable callback;
    Callable confirmation_callback;
} WsRequestData;

typedef struct {
    unsigned int identifier;
    String method_name;
    Dictionary url;
    Callable callback;
} SubscriptionData;

class WsRpcCall : public WebSocketPeer{
    GDCLASS(WsRpcCall, WebSocketPeer)
private:
    bool connecting = false;

    std::deque<WsRequestData> request_queue;
    std::vector<SubscriptionData> subscriptions;

    void process_package(const PackedByteArray& packet_data);
    void connect_ws(const String& url);
    void add_subscription(unsigned int id, unsigned int sub_id);
    void remove_subscription(unsigned int index);
    void call_subscription_callback(unsigned int id, const Dictionary& params);
    void call_confirmation_callback(unsigned int id, const Dictionary &params);
    void finalize_request(unsigned int id, const Dictionary& result);
    void close_if_done();
    void remove_request(unsigned int index);
    void process_timeouts(float delta);
    unsigned int request_index_from_id(unsigned int id);
    
protected:
    bool pending_request = false;

    static void _bind_methods();
public:
    bool is_pending();

    void process(float delta);
    void enqueue_ws_request(const Dictionary& request_body, const Callable& callback, const Callable& confirmation_callback, const Dictionary& url, float timeout = 20.0F);
    void unsubscribe_all(const Callable &callback, const Dictionary& url, float timeout = 20.0F);
};

};


#endif