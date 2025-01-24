#include "rpc_single_ws_request_client.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/web_socket_peer.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"

#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot {

void RpcSingleWsRequestClient::enqueue_ws_request(const Dictionary &request_body, const Callable &callback, const Callable &confirmation_callback, const Dictionary &url, float timeout) {
	request_queue.push_back(WsRequestData{ request_body, url, timeout, request_body["id"], callback, confirmation_callback });
}

void RpcSingleWsRequestClient::_bind_methods() {}

bool RpcSingleWsRequestClient::is_pending() const {
	return pending_request;
}

void RpcSingleWsRequestClient::process(double delta) {
	const unsigned int current_frame = Engine::get_singleton()->get_process_frames();
	if (current_frame == last_processed_frame) {
		return;
	}
	last_processed_frame = current_frame;

	WebSocketPeer::poll();
	const WebSocketPeer::State state = get_ready_state();
	switch (state) {
		case WebSocketPeer::STATE_OPEN:
			if (!request_queue.empty() && !is_pending()) {
				pending_request = true;
				connecting = false;
				send_text(JSON::stringify(request_queue.front().request));
			} else {
				process_timeouts(delta);
			}

			while (get_available_packet_count() != 0) {
				const PackedByteArray packet_data = get_packet();
				process_package(packet_data);
			}
			break;

		case WebSocketPeer::STATE_CLOSED:
			if (!request_queue.empty() && !connecting) {
				connect_ws(SolanaClient::assemble_url(request_queue.front().parsed_url));
			} else if (connecting) {
				process_timeouts(delta);
			}
			break;

		default:
			process_timeouts(delta);
			break;
	}
}

void RpcSingleWsRequestClient::process_package(const PackedByteArray &packet_data) {
	Dictionary json = JSON::parse_string(packet_data.get_string_from_ascii());

	const Variant result = json["result"];

	// Subscription trigger.
	if (json.has("method")) {
		const int subscription_id = static_cast<Dictionary>(static_cast<Dictionary>(json)["params"])["subscription"];
		call_subscription_callback(subscription_id, static_cast<Dictionary>(json)["params"]);

		return;
	}

	// Unsubscribe confirmation.
	if (result.get_type() == Variant::BOOL) {
		return;
	}

	// Check for subscribe confirmations.
	if (result.get_type() == Variant::FLOAT) {
		const unsigned int rpc_id = static_cast<Dictionary>(json)["id"];
		const unsigned int subscription_id = result;

		add_subscription({ .query_id = rpc_id, .queue_id = subscription_id });
		finalize_request(rpc_id, json);
	} else {
		finalize_request(json["id"], Dictionary());
		ERR_FAIL_EDMSG_CUSTOM("Web socket failed");
	}
}

void RpcSingleWsRequestClient::unsubscribe_all(const Callable &callback, const Dictionary &url, float timeout) {
	for (unsigned int i = 0; i < subscriptions.size(); i++) {
		if (subscriptions[i].callback == callback) {
			const String unsubscribe_method_name = subscriptions[i].method_name.replace("Subscribe", "Unsubscribe");

			const Array params = build_array(subscriptions[i].identifier);
			Dictionary request_body = SolanaClient::make_rpc_dict(unsubscribe_method_name, params);

			request_queue.push_back(WsRequestData{ request_body, url, timeout, request_body["id"], Callable(), Callable() });

			remove_subscription(i);
			i--;
		}
	}
}

void RpcSingleWsRequestClient::connect_ws(const String &url) {
	if (get_ready_state() == WebSocketPeer::STATE_CLOSED && !connecting) {
		const Error err = connect_to_url(url);
		if (err != Error::OK) {
			finalize_request(0, Dictionary());
			ERR_FAIL_EDMSG_CUSTOM("Ws failed to connect");
		}
		connecting = true;
	}
}

void RpcSingleWsRequestClient::add_subscription(const SubItemId &identifiers) {
	const unsigned int request_index = request_index_from_id(identifiers.query_id);
	if (request_index == request_queue.size()) {
		return;
	}
	const Dictionary &request_body = request_queue[request_index].request;
	subscriptions.push_back(SubscriptionData{ identifiers.queue_id, request_body["method"], request_queue.front().parsed_url, request_queue.front().callback });
}

void RpcSingleWsRequestClient::remove_subscription(unsigned int index) {
	subscriptions.erase(subscriptions.begin() + index);
}

void RpcSingleWsRequestClient::call_subscription_callback(unsigned int query_id, const Dictionary &params) {
	for (auto &subscription : subscriptions) {
		if (subscription.identifier == query_id) {
			const Array args = build_array(params);
			subscription.callback.callv(args);
		}
	}
}

void RpcSingleWsRequestClient::call_confirmation_callback(unsigned int index, const Dictionary &params) {
	const Array args = build_array(params);
	if (request_queue[index].confirmation_callback.is_valid()) {
		request_queue[index].confirmation_callback.callv(args);
	}
}

void RpcSingleWsRequestClient::finalize_request(unsigned int query_id, const Dictionary &result) {
	const unsigned int index = request_index_from_id(query_id);

	if (index >= request_queue.size()) {
		ERR_PRINT_ONCE_ED_CUSTOM("Internal error, please report");
	} else {
		call_confirmation_callback(index, result);
		remove_request(index);
	}

	pending_request = false;
	close_if_done();
}

void RpcSingleWsRequestClient::close_if_done() {
	if (subscriptions.empty()) {
		close();
	}
}

void RpcSingleWsRequestClient::remove_request(unsigned int index) {
	request_queue.erase(request_queue.begin() + index);
}

void RpcSingleWsRequestClient::process_timeouts(double delta) {
	for (auto &queue_item : request_queue) {
		queue_item.timeout -= delta;

		if (queue_item.timeout < 0.0F) {
			connecting = false;
			finalize_request(queue_item.request_identifier, Dictionary());
			ERR_FAIL_EDMSG_CUSTOM("Ws request timed out.");
		}
	}
}

unsigned int RpcSingleWsRequestClient::request_index_from_id(unsigned int query_id) {
	for (unsigned int i = 0; i < request_queue.size(); i++) {
		if (request_queue[i].request_identifier == query_id) {
			return i;
		}
	}
	return request_queue.size();
}

}; //namespace godot