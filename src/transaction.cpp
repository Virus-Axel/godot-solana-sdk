#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Transaction::_update_pointer(){
    
}
void Transaction::_free_pointer(){
    free_transaction(data_pointer);
}

void Transaction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_instructions"), &Transaction::get_instructions);
    ClassDB::bind_method(D_METHOD("set_instructions", "p_value"), &Transaction::set_instructions);
    ClassDB::bind_method(D_METHOD("get_payer"), &Transaction::get_payer);
    ClassDB::bind_method(D_METHOD("set_payer", "p_value"), &Transaction::set_payer);
    ClassDB::bind_method(D_METHOD("get_signers"), &Transaction::get_signers);
    ClassDB::bind_method(D_METHOD("set_signers", "p_value"), &Transaction::set_signers);
    ClassDB::bind_method(D_METHOD("get_signed_transaction"), &Transaction::get_signed_transaction);
    ClassDB::bind_method(D_METHOD("set_signed_transaction", "p_value"), &Transaction::set_signed_transaction);
    //ClassDB::add_property("Transaction", PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"), "set_payer", "get_payer");
    //ClassDB::add_property("Transaction", PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_RESOURCE_TYPE, "Instruction"), "set_instructions", "get_instructions");

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
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
    else if(name == "signed_transaction"){
        set_signed_transaction(p_value);
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
    else if(name == "signed_transaction"){
        r_ret = signed_transaction;
        return true;
    }
    else if(name == "signers"){
        r_ret = signers;
        return true;
    }
	return false;
}

void Transaction::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags visibility = PROPERTY_USAGE_NO_EDITOR;
    if(signed_transaction){
        visibility = PROPERTY_USAGE_DEFAULT;
    }
    p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_RESOURCE_TYPE, "Instruction"));
	p_list->push_back(PropertyInfo(Variant::BOOL, "signed_transaction", PROPERTY_HINT_RESOURCE_TYPE, "Instruction"));
    p_list->push_back(PropertyInfo(Variant::ARRAY, "signers", PROPERTY_HINT_NONE, "", visibility, ""));
}

Transaction::Transaction() {
}

void Transaction::create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash){
    // Rust wants to deallocate its memory, so we allocate new to avoid double free
    void** instruction_pointers = new void*[instructions.size()];
    void** signer_pointers = new void*[signers.size()];

    array_to_pointer_array<Instruction>(instructions, instruction_pointers);
    array_to_pointer_array<Keypair>(signers, signer_pointers);

    void *latest_blockhash_ptr =  variant_to_type<Pubkey>(latest_blockhash);
    void *payer_ptr = variant_to_type<Pubkey>(payer);

    create_transaction_signed_with_payer(instruction_pointers, instructions.size(), payer_ptr, signer_pointers, signers.size(), latest_blockhash_ptr);
}

void Transaction::set_instructions(const Array& p_value){
    instructions = p_value;
    for(int i = 0; i < instructions.size(); i++){
        if(instructions[i].get_type() == Variant::NIL){
            Ref<Instruction> resource = memnew(Instruction);
            instructions[i] = Variant(resource);
        }
    }
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
    for(int i = 0; i < signers.size(); i++){
        if(signers[i].get_type() == Variant::NIL){
            Ref<Keypair> resource = memnew(Keypair);
            signers[i] = Variant(resource);
        }
    }
}
Array Transaction::get_signers(){
    return signers;
}

void Transaction::set_signed_transaction(const bool p_value){
    signed_transaction = p_value;
    notify_property_list_changed();
}
bool Transaction::get_signed_transaction(){
    return signed_transaction;
}

Error Transaction::serialize(){
    void* tx = to_ptr();
    if (tx == nullptr){
        return Error::ERR_CANT_CREATE;
    }
    return Error::OK;
}

Transaction::~Transaction(){
    _free_pointer_if_not_null();
}
}