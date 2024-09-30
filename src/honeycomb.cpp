#include "honeycomb.hpp"

#include "pubkey.hpp"
#include "transaction.hpp"
#include "solana_utils.hpp"
#include "godot_cpp/classes/json.hpp"

namespace godot{

void HoneyComb::query_response_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body){
    std::cout << body.get_string_from_ascii().ascii() << std::endl;
    Dictionary response = JSON::parse_string(body.get_string_from_ascii());

    String encoded_transaction = ((Dictionary)((Dictionary)((Dictionary)response["data"])[method_name])["tx"])["transaction"];

    ERR_FAIL_COND_EDMSG(encoded_transaction.is_empty(), "transaction is empty.");

    if(!result_tx->is_inside_tree()){
        memfree(result_tx);
    }
    else{
        remove_child(result_tx);
    }
    result_tx = (Transaction*)(Object*)Transaction::new_from_bytes(SolanaUtils::bs58_decode(encoded_transaction));
    add_child(result_tx, false, INTERNAL_MODE_BACK);
    std::cout << encoded_transaction.ascii() << std::endl;
    std::cout << "setting signers" << std::endl;
    result_tx->update_latest_blockhash();

    result_tx->set_signers(signers);
    std::cout << "signers set" << std::endl;

    Callable callback = Callable(this, "transaction_response_callback");
    result_tx->connect("transaction_response_received", callback, CONNECT_ONE_SHOT);

    std::cout << "trying to sign" << std::endl;
    result_tx->sign();
    std::cout << "trying to send" << std::endl;
    result_tx->send();
}

void HoneyComb::transaction_response_callback(const Dictionary& response){
    std::cout << "transaction response" << std::endl;

    PackedByteArray bt = result_tx->serialize();

    for(unsigned int i = 0; i < bt.size(); i++)
        std::cout << (int)bt[i]<< ", ";
    std::cout << std::endl;

    std::cout << JSON::stringify(response).ascii() << std::endl;
}


void HoneyComb::send_query(){
    const String HONEYCOMB_URL = "https://edge.test.honeycombprotocol.com/";

    Callable callback = Callable(this, "query_response_callback");
    api_request->connect("request_completed", callback);

    PackedStringArray headers;
    headers.append("content-type: application/json");
    add_child(api_request);
    api_request->request(HONEYCOMB_URL, headers, HTTPClient::METHOD_POST, build());
}

String HoneyComb::build(){
    String args_type_list = "";
    String args_list = "";
    String args_values = "";
    for(unsigned int i = 0; i < args.size(); i++){
        std::cout << JSON::stringify(args[i]).ascii() << std::endl;
        Array format_params;
        Dictionary arg = args[i];
        format_params.append(arg["name"]);
        format_params.append(arg["type"]);
        args_type_list += String("${0}: {1},").format(format_params);

        format_params.clear();
        format_params.append(arg["name"]);
        args_list += String("{0}: ${0},").format(format_params);

        format_params.clear();
        format_params.append(arg["name"]);
        format_params.append(arg["value"]);
        args_values += String("\"{0}\": \"{1}\",").format(format_params);
    }

    // pop last ,
    args_type_list = args_type_list.erase(args_type_list.length() -1);
    args_list = args_list.erase(args_list.length() -1);
    args_values = args_values.erase(args_values.length() -1);

    Array format_params;
    format_params.append(method_name);
    format_params.append(args_type_list);
    format_params.append(args_list);
    format_params.append(args_values);

    const String query_template = "{ \"query\":\"query {0}({1}) { {0}({2}) { tx { transaction blockhash lastValidBlockHeight } project } }\", \"variables\":{{3}}}"; 
    return query_template.format(format_params);
}

void HoneyComb::bind_method_from_ref(const String ref){

}

void HoneyComb::add_arg(const String& name, const String& type_name, const String& value, bool optional){
    Dictionary entry;
    entry["name"] = name;
    if(!optional){
        entry["type"] = type_name + String("!");
    }
    else{
        entry["type"] = type_name;
    }
    entry["value"] = value;
    args.append(entry);
}

HoneyComb::HoneyComb(){
    api_request = memnew(HTTPRequest);
    result_tx = memnew(Transaction);
}

Variant HoneyComb::create_project_transaction(const Variant& authority, const String& name){
    signers.append(authority);
    method_name = "createCreateProjectTransaction";

    add_arg("authority", "String", Pubkey::string_from_variant(authority));
    add_arg("name", "String", name);

    send_query();

    return nullptr;
}

void HoneyComb::_bind_methods(){
    ClassDB::bind_method(D_METHOD("create_project", "authority", "name"), &HoneyComb::create_project_transaction);

    ClassDB::bind_method(D_METHOD("query_response_callback", "result", "response_code", "headers", "body"), &HoneyComb::query_response_callback);
    ClassDB::bind_method(D_METHOD("transaction_response_callback", "response"), &HoneyComb::transaction_response_callback);
}

HoneyComb::~HoneyComb(){
    if(!api_request->is_inside_tree()){
        memfree(api_request);
    }
    if(!result_tx->is_inside_tree()){
        memfree(result_tx);
    }
}

}