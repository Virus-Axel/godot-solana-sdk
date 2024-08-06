#include "rpc_single_ws_request_client.hpp"

#include <godot_cpp/classes/json.hpp>
#include <solana_client.hpp>

namespace godot{

void WsRpcCall::enqueue_ws_request(const Dictionary& request_body, const Callable& callback, const Dictionary& url, float timeout){
    request_queue.push_front(RequestData{request_body, url, timeout, request_body["id"], callback});
}

void WsRpcCall::_bind_methods(){}

bool WsRpcCall::is_pending(){
    return pending_request;
}

void WsRpcCall::process(float delta){
    WebSocketPeer::poll();
    WebSocketPeer::State state = get_ready_state();
    switch(state){
        case WebSocketPeer::STATE_OPEN:
            if(!is_pending() && !request_queue.empty()){
                pending_request = true;
                std::cout << "WS: " << JSON::stringify(request_queue.front().request).ascii() << std::endl;
                send_text(JSON::stringify(request_queue.front().request));
            }

            while(get_available_packet_count()){
                std::cout << "AHHHH" << std::endl;
                const PackedByteArray packet_data = get_packet();
                process_package(packet_data);
            }
        break;

        case WebSocketPeer::STATE_CLOSED:
            if(!request_queue.empty()){
                connect_ws(SolanaClient::assemble_url(request_queue.front().parsed_url));
            }
        break;

        default:
        break;
    }
}

void WsRpcCall::process_package(const PackedByteArray& packet_data){
    Dictionary json = JSON::parse_string(packet_data.get_string_from_ascii());

    const Variant result = json["result"];

    // Subscription trigger.
    if(json.has("method")){
        std::cout << packet_data.get_string_from_ascii().ascii() << std::endl;
        int subscription_id = ((Dictionary)((Dictionary)json)["params"])["subscription"];
        std::cout << "TRIGGER " << subscription_id << std::endl;
        call_subscription_callback(subscription_id, ((Dictionary)json)["params"]);

        return;
    }
    else if(result.get_type() == Variant::BOOL){
        return;
    }

    // Check for subscribe confirmations.
    if(result.get_type() == Variant::FLOAT){
        std::cout << packet_data.get_string_from_ascii().ascii() << std::endl;
        unsigned int rpc_id = ((Dictionary)json)["id"];
        unsigned int subscription_id = result;
        std::cout << "CONFIRMING  " << rpc_id << " with sub id " << subscription_id << std::endl;
        add_subscription(rpc_id, subscription_id);
        finalize_request(rpc_id);
        std::cout << "OUT" << std::endl;
    }
    else{
        ERR_FAIL_EDMSG("Web socket failed");
    }
}

void WsRpcCall::unsubscribe_all(const Callable &callback, const Dictionary& url, float timeout){
    for(unsigned int i = 0; i < subscriptions.size(); i++){
        if(subscriptions[i].callback == callback && subscriptions[i].identifier != 0){
            Array params;
            params.append(subscriptions[i].identifier);
            Dictionary request_body = SolanaClient::make_rpc_dict(method_names[i], params);
            request_queue.push_front(RequestData{request_body, url, timeout, request_body["id"], Callable()});

            remove_subscription(i);
            i--;
        }
    }
}

void WsRpcCall::connect_ws(const String& url){
    if(get_ready_state() == WebSocketPeer::STATE_CLOSED){
        connect_to_url(url);
    }
}

void WsRpcCall::add_subscription(unsigned int id, unsigned int sub_id){
    unsigned int request_index = request_index_from_id(id);
    if(request_index == request_queue.size()){
        std::cout << "err" << std::endl;
        return;
    }
    const Dictionary &request_body = request_queue[request_index].request;
    std::cout << "sub " << (int)request_body["id"] << std::endl;
    subscriptions.push_back(SubscriptionData{sub_id, request_body["method"], request_queue.front().parsed_url, request_queue.front().callback});
    method_names.push_back(request_body["method"]);
}

void WsRpcCall::remove_subscription(unsigned int id){
    subscriptions.erase(subscriptions.begin() + id);
    method_names.erase(method_names.begin() + id);
}

void WsRpcCall::call_subscription_callback(unsigned int id, const Dictionary& params){
    for(unsigned int i = 0; i < subscriptions.size(); i++){
        std::cout << "is " << (unsigned int) subscriptions[i].identifier << " == " << id << std::endl;
        if(subscriptions[i].identifier == id){
            Array args;
            args.append(params);
            subscriptions[i].callback.callv(args);
        }
    }
}

void WsRpcCall::finalize_request(unsigned int id){
    remove_request_with_id(id);
    pending_request = false;
    close_if_done();
}

void WsRpcCall::close_if_done(){
    if(subscriptions.size() == 0){
        close();
    }
}

void WsRpcCall::remove_request_with_id(unsigned int id){
    for(unsigned int i = 0; i < request_queue.size(); i++){
        if(request_queue[i].request_identifier == id){
            request_queue.erase(request_queue.begin() + i);
        }
    }
}

unsigned int WsRpcCall::request_index_from_id(unsigned int id){
    for(unsigned int i = 0; i < request_queue.size(); i++){
        if(request_queue[i].request_identifier == id){
            return i;
        }
    }
    return request_queue.size();
}

};