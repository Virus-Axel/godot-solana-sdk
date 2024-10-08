#include "honeycomb.hpp"

#include "pubkey.hpp"
#include "transaction.hpp"
#include "solana_utils.hpp"
#include "godot_cpp/classes/json.hpp"

namespace godot{

void InitResourceInput::_bind_methods(){
    
}

void InitResourceInput::set_resource_name(const String& resource_name){
    this->resource_name = resource_name;
}
String InitResourceInput::get_resource_name(){
    return resource_name;
}

void InitResourceInput::set_symbol(const String& symbol){
    this->symbol = symbol;
}
String InitResourceInput::get_symbol(){
    return symbol;
}

void InitResourceInput::set_uri(const String& uri){
    this->uri = uri;
}
String InitResourceInput::get_uri(){
    return uri;
}

void InitResourceInput::set_decimals(int32_t decimals){
    this->decimals = decimals;
}
int32_t InitResourceInput::get_decimals(){
    return decimals;
}

void InitResourceInput::set_storage(int32_t storage){
    this->storage = (ResourceStorageEnum)storage;
}
int32_t InitResourceInput::get_storage(){
    return storage;
}

Dictionary InitResourceInput::to_dict(){
    Dictionary result;
    String resource_name = "";
    String symbol = "";
    String uri = "";
    int32_t decimals = 0;
    ResourceStorageEnum storage = AccountState;
    result["name"] = resource_name;
    result["symbol"] = symbol;
    result["uri"] = uri;
    result["decimals"] = decimals;
    if(storage == 0){
        result["storage"] = "AccountState";
    }
    else{
        result["storage"] = "LedgerState";
    }
    return result;
}

void UserInfoInput::_bind_methods(){
    
}

Dictionary UserInfoInput::to_dict(){
    Dictionary result;

    return result;
}

void BasicTreeConfig::_bind_methods(){
    
}

void AdvancedTreeConfig::_bind_methods(){
    
}

Dictionary AdvancedTreeConfig::to_dict(){
    return Dictionary();
}

void TreeSetupConfig::_bind_methods(){
    
}

Dictionary TreeSetupConfig::to_dict(){
    return Dictionary();
}

void HoneyComb::query_response_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body){
    Dictionary response = JSON::parse_string(body.get_string_from_ascii());

    Dictionary method_response = ((Dictionary)response["data"])[method_name];
    String encoded_transaction = "";
    if (method_response.has("tx")){
        encoded_transaction = ((Dictionary)method_response["tx"])["transaction"];
    }
    else{
        encoded_transaction = method_response["transaction"];
    }
    //String encoded_transaction = response.find_key("transaction");

    if(encoded_transaction.is_empty()){
        pending = false;
        ERR_FAIL_EDMSG("transaction is empty.");
    }

    if(!result_tx->is_inside_tree()){
        memfree(result_tx);
    }
    else{
        remove_child(result_tx);
    }
    PackedByteArray decoded_tx = SolanaUtils::bs58_decode(encoded_transaction);
    result_tx = (Transaction*)(Object*)Transaction::new_from_bytes(decoded_tx);
    add_child(result_tx, false, INTERNAL_MODE_BACK);
    //result_tx->update_latest_blockhash("");

    result_tx->set_signers(signers);

    Callable callback = Callable(this, "transaction_response_callback");
    result_tx->connect("transaction_response_received", callback, CONNECT_ONE_SHOT);

    for(unsigned int i = 0; i < signers.size(); i++){
        if(signers[i].get_type() == Variant::OBJECT){
            if(((Object*)signers[i])->get_class() == "Keypair"){
                Array signer_array;
                signer_array.append(signers[i]);
                result_tx->partially_sign(signer_array);
            }
        }
    }
    
    //result_tx->sign();
    result_tx->send();
}

void HoneyComb::transaction_response_callback(const Dictionary& response){

    PackedByteArray bt = result_tx->serialize();

    pending = false;
    remove_child(child);
    emit_signal("transaction_response_received", response);
}


void HoneyComb::send_query(){
    const String HONEYCOMB_URL = "https://edge.test.honeycombprotocol.com/";

    Callable callback = Callable(this, "query_response_callback");
    api_request->connect("request_completed", callback);

    PackedStringArray headers;
    headers.append("content-type: application/json");
    add_child(api_request);
    child = api_request;
    pending = true;
    api_request->request(HONEYCOMB_URL, headers, HTTPClient::METHOD_POST, build());
}

String HoneyComb::build(){
    String args_type_list = "";
    String args_list = "";
    String args_values = "";
    for(unsigned int i = 0; i < args.size(); i++){
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
        if(String(arg["value"]).begins_with("{")){
            args_values += String("\"{0}\": {1},").format(format_params);
        }
        else{
            args_values += String("\"{0}\": \"{1}\",").format(format_params);
        }
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
    format_params.append(query_fields);

    const String query_template = "{ \"query\":\"query {0}({1}) { {0}({2}) { {4} } }\", \"variables\":{{3}}}"; 
    return query_template.format(format_params);
}

void HoneyComb::bind_method_from_ref(const String ref){

}

void HoneyComb::add_arg(const String& name, const String& type_name, const Variant& value, bool optional){
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

Variant HoneyComb::create_project(const Variant& authority, const String& name){
    signers.append(authority);
    method_name = "createCreateProjectTransaction";

    add_arg("authority", "String", Pubkey::string_from_variant(authority));
    add_arg("name", "String", name);

    query_fields = "tx { transaction blockhash lastValidBlockHeight } project";

    send_query();

    return nullptr;
}

void HoneyComb::create_user(const Variant& user_wallet_key){
    signers.append(user_wallet_key);
    signers.append(Pubkey::new_random());
    method_name = "createNewUserTransaction";

    add_arg("wallet", "String", Pubkey::string_from_variant(user_wallet_key));
    //String user_info = "{\"username\": \"xyz789\",\"name\": \"abc123\",\"bio\": \"abc123\",\"pfp\": \"abc123\"}";
    Dictionary user_info;
    user_info["username"] = "axel";
    user_info["name"] = "axel";
    user_info["bio"] = "axel";
    user_info["pfp"] = "axel";
    add_arg("info", "UserInfoInput", user_info, true);

    query_fields = "transaction blockhash lastValidBlockHeight";

    send_query();
}

void HoneyComb::create_profile(const Variant& project, const Variant& payer){
    signers.append(payer);
    method_name = "createNewProfileTransaction";

    add_arg("project", "String", Pubkey::string_from_variant(project));
    add_arg("payer", "String", Pubkey::string_from_variant(payer));

    query_fields = "transaction blockhash lastValidBlockHeight";

    send_query();
}

void HoneyComb::create_resource(const Variant& project, const Variant& authority, const String& name, const String& uri, const String& symbol, uint32_t decimals){
    signers.append(authority);
    method_name = "createCreateNewResourceTransaction";

    add_arg("project", "String", Pubkey::string_from_variant(project));
    add_arg("authority", "String", Pubkey::string_from_variant(authority));
    String resource_info = "{\"name\": \"{0}\",\"symbol\": \"{1}\",\"uri\": \"{2}\",\"decimals\": {3}, \"storage\": \"{4}\"}";
    Array params;
    params.append(name);
    params.append(symbol);
    params.append(uri);
    params.append(decimals);
    params.append("AccountState");
    add_arg("params", "InitResourceInput", resource_info.format(params));

    query_fields = "transaction blockhash lastValidBlockHeight";

    send_query();
}

void HoneyComb::bind_non_changing_methods(){
    ClassDB::add_signal("HoneyComb", MethodInfo("transaction_response_received", PropertyInfo(Variant::DICTIONARY, "response")));

    ClassDB::bind_method(D_METHOD("create_project", "authority", "name"), &HoneyComb::create_project);
    ClassDB::bind_method(D_METHOD("create_user", "user_wallet_key"), &HoneyComb::create_user);
    ClassDB::bind_method(D_METHOD("create_profile", "project", "payer"), &HoneyComb::create_profile);

    ClassDB::bind_method(D_METHOD("query_response_callback", "result", "response_code", "headers", "body"), &HoneyComb::query_response_callback);
    ClassDB::bind_method(D_METHOD("transaction_response_callback", "response"), &HoneyComb::transaction_response_callback);
}

HoneyComb::~HoneyComb(){
    /*if(!api_request->is_inside_tree()){
        memfree(api_request);
    }
    if(!result_tx->is_inside_tree()){
        memfree(result_tx);
    }*/
}

}