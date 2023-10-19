#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include <solana_sdk.hpp>
#include <phantom.hpp>

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

    ClassDB::bind_method(D_METHOD("_transaction_response", "result", "response_code", "headers", "body"), &Transaction::_transaction_response);
    ClassDB::bind_method(D_METHOD("_payer_signed", "signature"), &Transaction::_payer_signed);

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
    ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);
    ClassDB::bind_method(D_METHOD("sign", "latest_blockhash"), &Transaction::sign);
    ClassDB::bind_method(D_METHOD("sign_and_send"), &Transaction::sign_and_send);
    ClassDB::bind_method(D_METHOD("send"), &Transaction::send);
    ClassDB::bind_method(D_METHOD("partially_sign", "latest_blockhash"), &Transaction::partially_sign);
}

void Transaction::_transaction_response(int result, int response_code, PackedStringArray headers, PackedByteArray body){
    Node *req = get_child(0);
    remove_child(req);
    req->queue_free();

    std::cout << "response is " << result << " & " << response_code << std::endl;
    std::cout << body.get_string_from_utf8().ascii() << std::endl;
}

void Transaction::_payer_signed(PackedByteArray signature){
    std::cout << "payer is signed" << std::endl;
    PhantomController *controller = Object::cast_to<PhantomController>(payer);
    controller->disconnect("message_signed", Callable(this, "_payer_signed"));
    signatures.append_array(controller->get_message_signature());
    std::cout << "# signatures size " << signatures.size() << std::endl;

    emit_signal("fully_signed");
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

void Transaction::set_signers(const Array& p_value){
    signers = p_value;
}

Array Transaction::get_signers(){
    return signers;
}

PackedByteArray Transaction::serialize(){
    Pubkey *payer_key = Object::cast_to<Pubkey>(payer);
    const String hash_string = "4uQeVj5tqViQh7yWWGStvkEG1Zmhx6uasJtWCJziofM";//SolanaSDK::get_latest_blockhash();
    Variant hash = memnew(Hash);
    Object::cast_to<Hash>(hash)->set_value(hash_string);

    message = memnew(CompiledKeys(instructions, payer_key, hash));
    //return PackedByteArray();
    return Object::cast_to<CompiledKeys>(message)->serialize();
}

Variant Transaction::sign_and_send(){
    const String hash_string = SolanaSDK::get_latest_blockhash();
    Hash hash;
    hash.set_value(hash_string);

    PackedByteArray serialized_bytes = serialize();

    return SolanaSDK::send_transaction(SolanaSDK::bs64_encode(serialized_bytes));

    return OK;
}

Error Transaction::send(){
    PackedByteArray serialized_bytes = serialize();
    serialized_bytes.append_array(signatures);

    // Set headers
	PackedStringArray http_headers;
	http_headers.append("Content-Type: application/json");
	http_headers.append("Accept-Encoding: json");

    HTTPRequest *request = memnew(HTTPRequest);
    request->connect("request_completed", Callable(this, "_transaction_response"));
    add_child(request);

    const godot::String REQUEST_DATA = "{\"id\":1,\"jsonrpc\":\"2.0\",\"method\":\"sendTransaction\",\"params\":[\"" + SolanaSDK::bs64_encode(serialized_bytes) + "\",{\"encoding\":\"base64\"}]}";

	request->request("https://api.devnet.solana.com", http_headers, HTTPClient::METHOD_POST, REQUEST_DATA);

    return OK;
}

Error Transaction::sign(const Variant& latest_blockhash){
    std::cout << "signing this ***" << std::endl;

    PackedByteArray msg = serialize();

    TypedArray<Resource> &signers = Object::cast_to<CompiledKeys>(message)->get_signers();

    for (unsigned int i = 0; i < signers.size(); i++){
        Keypair *kp = Object::cast_to<Keypair>(signers[i]);
        PackedByteArray signature = kp->sign_message(msg);
        signatures.append_array(signature);
    }
    //emscripten_run_script("alert('hi from emscripten')");

    if(use_phantom_payer){
        PhantomController *controller = Object::cast_to<PhantomController>(payer);

        controller->connect("message_signed", Callable(this, "_payer_signed"));
        controller->sign_message(msg);

        /*while(!controller->is_idle()){
            std::cout << "Y from phantom " << std::endl;
            OS::get_singleton()->delay_msec(100);
        }*/
    }

    std::cout << "# signatures size " << signatures.size() << std::endl;

    return OK;
}

Error Transaction::partially_sign(const Variant& latest_blockhash){

    return OK;
}

Transaction::~Transaction(){
}
}