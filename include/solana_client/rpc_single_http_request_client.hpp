#ifndef SOLANA_SDK_RPC_SINGLE_HTTP_REQUEST_CLIENT_HPP
#define SOLANA_SDK_RPC_SINGLE_HTTP_REQUEST_CLIENT_HPP

#include <godot_cpp/classes/http_client.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <queue>

namespace godot {

/**
 * @brief Specifies information about a request.
 */
typedef struct {
	Dictionary request; ///< Request to send.
	Dictionary parsed_url; ///< URL Dictionary.
	double timeout; ///< timeout of request.
	int request_identifier; ///< ID to pass with the RPC request.
	Callable callback; ///< Callback that will be called when request response received.
} RequestData;

/**
 * @class RpcSingleHttpRequestClient
 * @brief Handles requests synchronously in a queue.
 */
class RpcSingleHttpRequestClient : public HTTPClient {
	GDCLASS(RpcSingleHttpRequestClient, HTTPClient)
private:
	bool skip_id = false;

	std::queue<RequestData> request_queue;
	PackedByteArray response_data;

	void process_message_sending();
	void process_body();
	void initiate_connection();
	void update_timeouts(const double delta);
	bool is_pending() const;
	bool has_request() const;
	bool is_response_valid(const Dictionary &response) const;

	Error connect_to();
	Error send_next_request();
	void finalize_faulty();
	void finalize_request(const Dictionary &response);

protected:
	/**
	 * @bindmethods{RpcSingleHttpRequestClient, Node}
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Check if request queue is empty.
	 * 
	 * @return true If request queue is empty.
	 * @return false If request is queued.
	 */
	bool is_completed() const;

	/**
	 * @setter{skip_id, skip_id}
	 */
	void set_skip_id(bool skip_id);

	/**
	 * @brief Process the request.
	 * 
	 * @param delta Elapsed time since last process.
	 */
	void process(const double delta);

	/**
	 * @brief Initiate an asynchronous request.
	 * 
	 * @param request_body Request Dictionary.
	 * @param parsed_url URL to send request to.
	 * @param callback Callback to call on response received.
	 * @param timeout Timeout of request.
	 */
	void asynchronous_request(const Dictionary &request_body, const Dictionary &parsed_url, const Callable &callback, float timeout = 20.0F);
};

} //namespace godot

#endif