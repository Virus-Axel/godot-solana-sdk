#include "rpc_single_ws_request_client.hpp"

#include <godot_cpp/classes/json.hpp>
#include <solana_client.hpp>

namespace godot{

void WsRpcCall::enqueue_ws_request(const Dictionary& request_body, const Callable& callback, const Callable& confirmation_callback, const Dictionary& url, float timeout){
    request_queue.push_back(WsRequestData{request_body, url, timeout, request_body["id"], callback, confirmation_callback});
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
            if(!request_queue.empty() && !is_pending()){
                pending_request = true;
                connecting = false;
                send_text(JSON::stringify(request_queue.front().request));
            }
            else{
                process_timeouts(delta);
            }

            while(get_available_packet_count()){
                const PackedByteArray packet_data = get_packet();
                process_package(packet_data);
            }
        break;

        case WebSocketPeer::STATE_CLOSED:
            if(!request_queue.empty() && !connecting){
                connect_ws(SolanaClient::assemble_url(request_queue.front().parsed_url));
            }
            else if(connecting){
                process_timeouts(delta);
            }
        break;

        default:
            process_timeouts(delta);
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
        call_subscription_callback(subscription_id, ((Dictionary)json)["params"]);

        return;
    }
    else if(result.get_type() == Variant::BOOL){

        return;
    }

    // Check for subscribe confirmations.
    if(result.get_type() == Variant::FLOAT){
        unsigned int rpc_id = ((Dictionary)json)["id"];
        unsigned int subscription_id = result;

        add_subscription(rpc_id, subscription_id);
        finalize_request(rpc_id, json);
    }
    else{
        finalize_request(json["id"], Dictionary());
        ERR_FAIL_EDMSG("Web socket failed");
    }
}

void WsRpcCall::unsubscribe_all(const Callable &callback, const Dictionary& url, float timeout){
    
    for(unsigned int i = 0; i < subscriptions.size(); i++){
        std::cout <<"GOOOO" << std::endl;
        if(subscriptions[i].callback == callback){
            std::cout << "HIT " << i << std::endl;
            Array params;
            params.append(subscriptions[i].identifier);
            String unsubscribe_method_name = subscriptions[i].method_name.replace("Subscribe", "Unsubscribe");
            Dictionary request_body = SolanaClient::make_rpc_dict(unsubscribe_method_name, params);
            request_queue.push_back(WsRequestData{request_body, url, timeout, request_body["id"], Callable(), Callable()});

            remove_subscription(i);
            i--;
        }
    }
}

void WsRpcCall::connect_ws(const String& url){
    if(get_ready_state() == WebSocketPeer::STATE_CLOSED && !connecting){
        Error err = connect_to_url(url);
        if(err != OK){
            finalize_request(0, Dictionary());
            ERR_FAIL_EDMSG("Ws failed to connect");
        }
        connecting = true;
    }
}

void WsRpcCall::add_subscription(unsigned int id, unsigned int sub_id){
    unsigned int request_index = request_index_from_id(id);
    if(request_index == request_queue.size()){
        return;
    }
    const Dictionary &request_body = request_queue[request_index].request;
    subscriptions.push_back(SubscriptionData{sub_id, request_body["method"], request_queue.front().parsed_url, request_queue.front().callback});
}

void WsRpcCall::remove_subscription(unsigned int index){
    subscriptions.erase(subscriptions.begin() + index);
}

void WsRpcCall::call_subscription_callback(unsigned int id, const Dictionary& params){
    for(unsigned int i = 0; i < subscriptions.size(); i++){
        if(subscriptions[i].identifier == id){
            Array args;
            args.append(params);
            subscriptions[i].callback.callv(args);
        }
    }
}

void WsRpcCall::call_confirmation_callback(unsigned int id, const Dictionary &params){
    std::cout << "trying Callback " << request_queue.size() << std::endl;
    for(unsigned int i = 0; i < request_queue.size(); i++){
        if(request_queue[i].request_identifier == id){
            Array args;
            args.push_back(params);
            if(request_queue[i].confirmation_callback.is_valid()){
                std::cout << "Callback" << std::endl;
                request_queue[i].confirmation_callback.callv(args);
            }
        }
    }
}

void WsRpcCall::finalize_request(unsigned int id, const Dictionary& result){
    call_confirmation_callback(id, result);
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

void WsRpcCall::process_timeouts(float delta){
    for(unsigned int i = 0; i < request_queue.size(); i++){
        request_queue[i].timeout -= delta;
        if(request_queue[i].timeout < 0.0F){
            connecting = false;
            finalize_request(request_queue[i].request_identifier, Dictionary());
            ERR_FAIL_EDMSG("Ws request timed out.");
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