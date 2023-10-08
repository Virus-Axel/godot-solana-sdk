#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"
#include "pubkey.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <solana_sdk.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;


void Transaction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_instructions"), &Transaction::get_instructions);
    ClassDB::bind_method(D_METHOD("set_instructions", "p_value"), &Transaction::set_instructions);
    ClassDB::bind_method(D_METHOD("get_payer"), &Transaction::get_payer);
    ClassDB::bind_method(D_METHOD("set_payer", "p_value"), &Transaction::set_payer);
    ClassDB::bind_method(D_METHOD("get_signers"), &Transaction::get_signers);
    ClassDB::bind_method(D_METHOD("set_signers", "p_value"), &Transaction::set_signers);

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
    ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);
    ClassDB::bind_method(D_METHOD("sign", "latest_blockhash"), &Transaction::sign);
    ClassDB::bind_method(D_METHOD("sign_and_send"), &Transaction::sign_and_send);
    ClassDB::bind_method(D_METHOD("partially_sign", "latest_blockhash"), &Transaction::partially_sign);
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
    p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair"));
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

void Transaction::set_signers(const Array& p_value){
    signers = p_value;
}

Array Transaction::get_signers(){
    return signers;
}

PackedByteArray Transaction::serialize(){
    Pubkey *payer_key = Object::cast_to<Pubkey>(payer);
    const String hash_string = "4uQeVj5tqViQh7yWWGStvkEG1Zmhx6uasJtWCJziofM";//SolanaSDK::get_latest_blockhash();
    Hash hash;
    hash.set_value(hash_string);

    message = memnew(CompiledKeys(instructions, payer_key, hash));
    //return PackedByteArray();
    return Object::cast_to<CompiledKeys>(message)->serialize();
}

Variant Transaction::sign_and_send(){
    const String hash_string = SolanaSDK::get_latest_blockhash();
    Hash hash;
    hash.set_value(hash_string);

    PackedByteArray serialized_bytes = serialize();

    return SolanaSDK::send_transaction(encode64(serialized_bytes));

    return OK;
}

Error Transaction::sign(const Variant& latest_blockhash){

    PackedByteArray msg = serialize();

    TypedArray<Resource> &signers = Object::cast_to<CompiledKeys>(message)->get_signers();
    for (unsigned int i = 0; i < signers.size(); i++){
        Keypair *kp = Object::cast_to<Keypair>(signers[i]);
        PackedByteArray signature = kp->sign_message(msg);
        signatures.append_array(signature);
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