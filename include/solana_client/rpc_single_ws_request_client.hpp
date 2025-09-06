#ifndef SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_SINGLE_WS_REQUEST_CLIENT_HPP

#include <cstdint>
#include <deque>
#include <vector>

#include "godot_cpp/classes/web_socket_peer.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"

#include "rpc_single_http_request_client.hpp"
#include "solana_utils.hpp"

namespace godot {

/**
 * @brief A mapping struct of an internal ID and queue ID used in RPC communication.
 */
typedef struct { // NOLINT(modernize-use-using)
	uint32_t query_id; ///< Internal ID for query items.
	uint32_t queue_id; ///< ID used in RPC communication.
} SubItemId;

/**
 * @brief Contains information about a web socket queue item.
 */
typedef struct { // NOLINT(modernize-use-using)
	Dictionary request; ///< Web socket request.
	Dictionary parsed_url; ///< URL to send request to.
	double timeout; ///< Timeout of request.
	int request_identifier; ///< Request Identifier.
	Callable callback; ///< Callback to call on subscription notifications.
	Callable confirmation_callback; ///< Callback to call on request response.
} WsRequestData;

/**
 * @brief Contains details about a subscription.
 */
typedef struct { // NOLINT(modernize-use-using)
	unsigned int identifier; ///< RPC ID of a subscription.
	String method_name; ///< Web Socket method name.
	Dictionary url; ///< URL of request.
	Callable callback; ///< Callback to call on notifications.
} SubscriptionData;

/**
 * @brief Handles web socket requests synchronously in a queue.
 */
class RpcSingleWsRequestClient : public WebSocketPeer {
	GDCLASS_CUSTOM(RpcSingleWsRequestClient, WebSocketPeer)
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
	bool pending_request = false;

protected:
	/**
	 * @brief bindmethods{RpcSingleWsRequestClient, Node}
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Check if a request is pending.
	 *
	 * @return true If a request is pending.
	 * @return false If no request is pending.
	 */
	[[nodiscard]] bool is_pending() const;

	/**
	 * @brief Process the requests client.
	 *
	 * @param delta Elapsed time since last process.
	 */
	void process(double delta);

	/**
	 * @brief Enqueue a web socket request.
	 *
	 * @param request_body Request Dictionary.
	 * @param callback Callback to be called when subscription notification is received.
	 * @param confirmation_callback Callback to be called when request response is received.
	 * @param url URL to send request to.
	 * @param timeout Timeout of request.
	 */
	void enqueue_ws_request(const Dictionary &request_body, const Callable &callback, const Callable &confirmation_callback, const Dictionary &url, float timeout = DEFAULT_REQUEST_TIMEOUT);

	/**
	 * @brief Send requests to unsubscribe all active subscriptions with a certain Callable connected.
	 *
	 * @param callback Callback to identify subscriptions to unsubscribe to.
	 * @param url URL to use for requests.
	 * @param timeout Timeout of requests.
	 */
	void unsubscribe_all(const Callable &callback, const Dictionary &url, float timeout = DEFAULT_REQUEST_TIMEOUT);
};

}; //namespace godot

#endif