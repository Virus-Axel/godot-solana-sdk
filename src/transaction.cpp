#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <solana_client.hpp>
#include <solana_sdk.hpp>
#include <wallet_adapter.hpp>
#include <message.hpp>

//#include <emscripten.h>

namespace godot{

using internal::gdextension_interface_print_warning;


void Transaction::_bind_methods() {
    ClassDB::add_signal("Transaction", MethodInfo("fully_signed"));
    ClassDB::add_signal("Transaction", MethodInfo("sign_error", PropertyInfo(Variant::INT, "signer_index")));
    ClassDB::add_signal("Transaction", MethodInfo("transaction_response", PropertyInfo(Variant::DICTIONARY, "result")));

    ClassDB::bind_method(D_METHOD("get_instructions"), &Transaction::get_instructions);
    ClassDB::bind_method(D_METHOD("set_instructions", "p_value"), &Transaction::set_instructions);
    ClassDB::bind_method(D_METHOD("get_payer"), &Transaction::get_payer);
    ClassDB::bind_method(D_METHOD("set_payer", "p_value"), &Transaction::set_payer);
    ClassDB::bind_method(D_METHOD("get_signers"), &Transaction::get_signers);
    ClassDB::bind_method(D_METHOD("set_signers", "p_value"), &Transaction::set_signers);
    ClassDB::bind_method(D_METHOD("send_callback", "params"), &Transaction::send_callback);

    ClassDB::bind_method(D_METHOD("_signer_signed", "signature"), &Transaction::_signer_signed);
    ClassDB::bind_method(D_METHOD("_signer_failed"), &Transaction::_signer_failed);

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
    ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);

    ClassDB::bind_method(D_METHOD("add_instruction", "instruction"), &Transaction::add_instruction);
    ClassDB::bind_method(D_METHOD("update_latest_blockhash", "custom_hash"), &Transaction::update_latest_blockhash, DEFVAL(""));
    
    ClassDB::bind_method(D_METHOD("sign"), &Transaction::sign);
    ClassDB::bind_method(D_METHOD("sign_and_send"), &Transaction::sign_and_send);
    ClassDB::bind_method(D_METHOD("create_message"), &Transaction::create_message);
    ClassDB::bind_method(D_METHOD("send"), &Transaction::send);
    ClassDB::bind_method(D_METHOD("send_and_disconnect"), &Transaction::send_and_disconnect);
    ClassDB::bind_method(D_METHOD("partially_sign", "latest_blockhash"), &Transaction::partially_sign);

    ClassDB::bind_method(D_METHOD("is_confirmed"), &Transaction::is_confirmed);
    ClassDB::bind_method(D_METHOD("is_finalized"), &Transaction::is_finalized);
}

void Transaction::_signer_signed(PackedByteArray signature){
    WalletAdapter *controller = Object::cast_to<WalletAdapter>(payer);
    controller->disconnect("message_signed", Callable(this, "_signer_signed"));
    controller->disconnect("signing_error", Callable(this, "_signer_failed"));

    const uint32_t index = controller->get_active_signer_index();

    signatures[index] = signature;
    ready_signature_amount++;

    check_fully_signed();
}

void Transaction::_signer_failed(){
    WalletAdapter *controller = Object::cast_to<WalletAdapter>(payer);
    controller->disconnect("message_signed", Callable(this, "_signer_signed"));
    controller->disconnect("signing_error", Callable(this, "_signer_failed"));

    emit_signal("sign_error", controller->get_active_signer_index());
}

bool Transaction::is_phantom_payer() const{
    return payer.has_method("get_connected_key");
}

bool Transaction::is_message_valid(){
    return (message.get_type() == Variant::OBJECT);
}

void Transaction::create_message(){
    // Free existing memory.
    message.clear();
    if(instructions.is_empty() || (payer.get_type() == Variant::NIL)){
        signatures.clear();
        return;
    }
    for(unsigned int i = 0; i < instructions.size(); i++){
        if(instructions[i].get_type() != Variant::OBJECT){
            signatures.clear();
            return;
        }
        if(!Object::cast_to<Instruction>(instructions[i])->is_serializable()){
            signatures.clear();
            return;
        }
    }
    message = memnew(Message(instructions, payer));
    Object::cast_to<Message>(message)->set_latest_blockhash(latest_blockhash_string);

    const int amount_of_signers = Object::cast_to<Message>(message)->get_amount_signers();

    if(signers.size() == amount_of_signers){
        return;
    }

    ready_signature_amount = 0;
    signatures.resize(amount_of_signers);
    for(unsigned int i = 0; i < signatures.size(); i++){
        PackedByteArray temp;
        temp.resize(64);
        signatures[i] = temp;
    }
}

void Transaction::check_fully_signed(){
    if(ready_signature_amount == signers.size()){
        emit_signal("fully_signed");
    }
}

void Transaction::sign_at_index(const uint32_t index){
    if(signers[index].get_type() != Variant::Type::OBJECT){
        Array params;
        params.append(signers[index].get_type());
        internal::gdextension_interface_print_warning(String("Signer is not an object. It is a {0}").format(params).utf8(), "sign_at_index", __FILE__, __LINE__, false);
    }
    else if(signers[index].has_method("verify_signature")){
        Keypair *kp = Object::cast_to<Keypair>(signers[index]);

        PackedByteArray signature = kp->sign_message(serialize_message());
        signatures[index] = signature;
        ready_signature_amount++;
        check_fully_signed();
    }
    else if(signers[index].has_method("sign_message")){
        WalletAdapter* controller = Object::cast_to<WalletAdapter>(signers[index]);

        controller->connect("message_signed", Callable(this, "_signer_signed"));
        controller->connect("signing_error", Callable(this, "_signer_failed"));
        controller->sign_message(serialize(), index);
    }
}

bool Transaction::_set(const StringName &p_name, const Variant &p_value){
    String name = p_name;
	if (name == "instructions") {
		set_instructions(p_value);
		return true;
	}
    else if(name == "payer"){
        set_payer(p_value);
		return true;
    }
    else if(name == "signers"){
        set_signers(p_value);
        return true;
    }
    else if(name == "use_phantom_payer"){
        set_use_phantom_payer(p_value);
        return true;
    }
	return false;
}

bool Transaction::_get(const StringName &p_name, Variant &r_ret) const{
    String name = p_name;
	if (name == "instructions") {
		r_ret = instructions;
		return true;
	}
    else if(name == "payer"){
        r_ret = payer;
		return true;
    }
    else if(name == "signers"){
        r_ret = signers;
        return true;
    }
    else if(name == "use_phantom_payer"){
        r_ret = use_phantom_payer;
        return true;
    }
	return false;
}

void Transaction::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::BOOL, "use_phantom_payer", PROPERTY_HINT_NONE, "false"));
    if(!use_phantom_payer){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_NODE_TYPE, "WalletAdapter"));
    }
	p_list->push_back(PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Instruction")));
    p_list->push_back(PropertyInfo(Variant::ARRAY, "signers", PROPERTY_HINT_NONE, MAKE_RESOURCE_TYPE_HINT("Keypair")));
}

Transaction::Transaction() {
}

void Transaction::create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash){
}

bool Transaction::is_confirmed(){
    if(latest_commitment == "confirmed" || latest_commitment == "finalized"){
        return true;
    }

    PackedStringArray arr;
    arr.append(result_signature);
    Dictionary rpc_result = SolanaClient::get_signature_statuses(arr);

    Dictionary transaction_info = rpc_result.get("result", nullptr);
    Array transaction_value = transaction_info.get("value", nullptr);

    if(transaction_value[0].get_type() != Variant::DICTIONARY){
        return false;
    }

    String status = transaction_value[0].get("confirmationStatus", nullptr);
    latest_commitment = status;
    return (latest_commitment == "confirmed" || latest_commitment == "finalized");
}

bool Transaction::is_finalized(){
    if(latest_commitment == "finalized"){
        return true;
    }

    PackedStringArray arr;
    arr.append(result_signature);
    Dictionary rpc_result = SolanaClient::get_signature_statuses(arr);

    Dictionary transaction_info = rpc_result.get("result", nullptr);
    Array transaction_value = transaction_info.get("value", nullptr);

    if(transaction_value[0].get_type() != Variant::DICTIONARY){
        return false;
    }
    
    String status = transaction_value[0].get("confirmationStatus");
    latest_commitment = status;
    return (latest_commitment == "finalized");
}

void Transaction::set_instructions(const Array& p_value){
    instructions = p_value;
    create_message();
}

Array Transaction::get_instructions(){
    return instructions;
}

void Transaction::set_payer(const Variant& p_value){
    payer = p_value;
    create_message();
}

Variant Transaction::get_payer(){
    return payer;
}

void Transaction::set_use_phantom_payer(bool p_value){
    use_phantom_payer = p_value;
    notify_property_list_changed();
}

bool Transaction::get_use_phantom_payer(){
    return use_phantom_payer;
}

void Transaction::update_latest_blockhash(const String &custom_hash){
    ERR_FAIL_COND_EDMSG(instructions.size() == 0, "No instructions added to instruction.");
    ERR_FAIL_COND_EDMSG(!is_message_valid(), "Failed to compile transaction message.");

    if(custom_hash.is_empty()){
        const Dictionary latest_blockhash = SolanaClient::get_latest_blockhash();
        const Dictionary blockhash_result = latest_blockhash["result"];
        const Dictionary blockhash_value = blockhash_result["value"];
        latest_blockhash_string = blockhash_value["blockhash"];
        Object::cast_to<Message>(message)->set_latest_blockhash(latest_blockhash_string);
    }
    else{
        latest_blockhash_string = custom_hash;
        Object::cast_to<Message>(message)->set_latest_blockhash(custom_hash);
    }
}

void Transaction::add_instruction(const Variant &instruction){
    instructions.append(instruction);
    create_message();
}

void Transaction::set_signers(const Array& p_value){
    signers = p_value;
}

Array Transaction::get_signers(){
    return signers;
}

PackedByteArray Transaction::serialize(){
    PackedByteArray serialized_bytes;

    serialized_bytes.append_array(serialize_signers());
    serialized_bytes.append_array(serialize_message());

    return serialized_bytes;
}

PackedByteArray Transaction::serialize_message(){
    ERR_FAIL_COND_V_EDMSG(!is_message_valid(), PackedByteArray(), "Invalid message.");
    return Object::cast_to<Message>(message)->serialize();
}

PackedByteArray Transaction::serialize_signers(){
    PackedByteArray serialized_bytes;
    serialized_bytes.append(signatures.size());
    for(unsigned int i = 0; i < signatures.size(); i++){
        serialized_bytes.append_array(signatures[i]);
    }
    return serialized_bytes;
}

Variant Transaction::sign_and_send(){
    connect("fully_signed", Callable(this, "send_and_disconnect"));

    sign();

    return OK;
}

void Transaction::send_callback(Dictionary params){
    if(params.has("result")){
        result_signature = params["result"];
    }
    emit_signal("transaction_response", params);
}

Dictionary Transaction::send(){
    PackedByteArray serialized_bytes = serialize();

    Callable callback(this, "send_callback");

    SolanaClient::set_http_callback(callback);
    Dictionary rpc_result = SolanaClient::send_transaction(SolanaSDK::bs64_encode(serialized_bytes));

    return rpc_result;
}

void Transaction::send_and_disconnect(){
    disconnect("fully_signed", Callable(this, "send_and_disconnect"));

    send();
}

Error Transaction::sign(){
    ERR_FAIL_COND_V_EDMSG(!is_message_valid(), Error::ERR_INVALID_DATA, "Invalid message.");
    PackedByteArray msg = serialize();

    signers = Object::cast_to<Message>(message)->get_signers();

    for (unsigned int i = 0; i < signers.size(); i++){
        sign_at_index(i);
    }

    return OK;
}

Error Transaction::partially_sign(const Variant& latest_blockhash){

    return OK;
}

Transaction::~Transaction(){
}
}