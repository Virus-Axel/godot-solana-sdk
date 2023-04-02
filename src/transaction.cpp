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
    ClassDB::add_property("Transaction", PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"), "set_payer", "get_payer");
    ClassDB::add_property("Transaction", PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_RESOURCE_TYPE, "Instruction"), "set_instructions", "get_instructions");

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
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

Transaction::~Transaction(){
    _free_pointer_if_not_null();
}
}