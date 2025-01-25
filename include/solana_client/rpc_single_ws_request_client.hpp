#ifndef SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP

#include <godot_cpp/classes/http_client.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <queue>

namespace godot {

typedef struct {
	uint32_t query_id;
	uint32_t queue_id;
} SubItemId;

typedef struct {
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

class RpcSingleWsRequestClient : public WebSocketPeer {
	GDCLASS(RpcSingleWsRequestClient, WebSocketPeer)
private:
	bool connecting = false;
	unsigned int last_processed_frame = 0;

	std::deque<WsRequestData> request_queue;
	std::vector<SubscriptionData> subscriptions;

	void process_package(const PackedByteArray &packet_data);
	void connect_ws(const String &url);
	void add_subscription(const SubItemId &identifiers);
	void remove_subscription(unsigned int index);
	void call_subscription_callback(unsigned int query_id, const Dictionary &params);
	void call_confirmation_callback(unsigned int query_id, const Dictionary &params);
	void finalize_request(unsigned int query_id, const Dictionary &result);
	void close_if_done();
	void remove_request(unsigned int index);
	void process_timeouts(double delta);
	unsigned int request_index_from_id(unsigned int query_id);

protected:
	bool pending_request = false;

	static void _bind_methods();

public:
	bool is_pending() const;

	void process(double delta);
	void enqueue_ws_request(const Dictionary &request_body, const Callable &callback, const Callable &confirmation_callback, const Dictionary &url, float timeout = 20.0F);
	void unsubscribe_all(const Callable &callback, const Dictionary &url, float timeout = 20.0F);
};

}; //namespace godot

#endif