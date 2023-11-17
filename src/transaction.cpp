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
#include <phantom.hpp>
#include <message.hpp>

//#include <emscripten.h>

namespace godot{

using internal::gdextension_interface_print_warning;


void Transaction::_bind_methods() {
    ClassDB::add_signal("Transaction", MethodInfo("fully_signed"));

    ClassDB::bind_method(D_METHOD("get_instructions"), &Transaction::get_instructions);
    ClassDB::bind_method(D_METHOD("set_instructions", "p_value"), &Transaction::set_instructions);
    ClassDB::bind_method(D_METHOD("get_payer"), &Transaction::get_payer);
    ClassDB::bind_method(D_METHOD("set_payer", "p_value"), &Transaction::set_payer);
    ClassDB::bind_method(D_METHOD("get_signers"), &Transaction::get_signers);
    ClassDB::bind_method(D_METHOD("set_signers", "p_value"), &Transaction::set_signers);

    ClassDB::bind_method(D_METHOD("_payer_signed", "signature"), &Transaction::_payer_signed);

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
    ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);
    ClassDB::bind_method(D_METHOD("update_latest_blockhash", "custom_hash"), &Transaction::update_latest_blockhash);
    ClassDB::bind_method(D_METHOD("sign"), &Transaction::sign);
    ClassDB::bind_method(D_METHOD("sign_and_send"), &Transaction::sign_and_send);
    ClassDB::bind_method(D_METHOD("create_message"), &Transaction::create_message);
    ClassDB::bind_method(D_METHOD("send"), &Transaction::send);
    ClassDB::bind_method(D_METHOD("partially_sign", "latest_blockhash"), &Transaction::partially_sign);
}

void Transaction::_payer_signed(PackedByteArray signature){
    PhantomController *controller = Object::cast_to<PhantomController>(payer);
    controller->disconnect("message_signed", Callable(this, "_payer_signed"));

    signatures[0] = signature;

    emit_signal("fully_signed");
}

bool Transaction::is_phantom_payer() const{
    return payer.has_method("get_connected_key");
}

void Transaction::create_message(){
    // Free existing memory.
    message.clear();
    if(instructions.is_empty() || (payer.get_type() == Variant::NIL)){
        signatures.clear();
        return;
    }

    message = memnew(Message(instructions, payer));
    Object::cast_to<Message>(message)->set_latest_blockhash(latest_blockhash_string);

    const int amount_of_signers = Object::cast_to<Message>(message)->get_amount_signers();
    
    if(signers.size() == amount_of_signers){
        return;
    }

    signatures.resize(amount_of_signers);
    for(unsigned int i = 0; i < signatures.size(); i++){
        PackedByteArray temp;
        temp.resize(64);
        signatures[i] = temp;
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
	return false;
}

void Transaction::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::BOOL, "use_phantom_payer", PROPERTY_HINT_NONE, "false"));
    if(!use_phantom_payer){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
    }
    else{
        p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_NODE_TYPE, "PhantomController"));
    }
	p_list->push_back(PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Instruction")));
    p_list->push_back(PropertyInfo(Variant::ARRAY, "signers", PROPERTY_HINT_NONE, MAKE_RESOURCE_TYPE_HINT("Keypair")));
}

Transaction::Transaction() {
}

void Transaction::create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash){
}

void Transaction::set_instructions(const Array& p_value){
    instructions = p_value;
}

Array Transaction::get_instructions(){
    return instructions;
}

void Transaction::set_payer(const Variant& p_value){
    payer = p_value;
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
    create_message();
    PackedByteArray msg = serialize();

    TypedArray<Keypair> &signers = Object::cast_to<Message>(message)->get_signers();

    for (unsigned int i = 0; i < signers.size(); i++){
        Keypair *kp = Object::cast_to<Keypair>(signers[i]);
        PackedByteArray signature = kp->sign_message(serialize_message());
        signatures[1 + i] = signature;
    }

    if(is_phantom_payer()){
        PhantomController *controller = Object::cast_to<PhantomController>(payer);

        controller->connect("message_signed", Callable(this, "_payer_signed"));
        controller->sign_message(serialize());
    }

    return OK;
}

Dictionary Transaction::send(){
    PackedByteArray serialized_bytes = serialize();
    return SolanaClient::send_transaction(SolanaSDK::bs64_encode(serialized_bytes));
}

Error Transaction::sign(){
    create_message();
    PackedByteArray msg = serialize();

    TypedArray<Keypair> &signers = Object::cast_to<Message>(message)->get_signers();

    for (unsigned int i = 0; i < signers.size(); i++){
        Keypair *kp = Object::cast_to<Keypair>(signers[i]);
        PackedByteArray signature = kp->sign_message(serialize_message());
        signatures[1 + i] = signature;
    }

    if(is_phantom_payer()){
        PhantomController *controller = Object::cast_to<PhantomController>(payer);

        controller->connect("message_signed", Callable(this, "_payer_signed"));
        controller->sign_message(serialize());
    }

    return OK;
}

Error Transaction::partially_sign(const Variant& latest_blockhash){

    return OK;
}

Transaction::~Transaction(){
}
}