#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Transaction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
}

Transaction::Transaction() {
    data_pointer = nullptr;
}

void Transaction::create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash){
    // Rust wants to deallocate its memory, so we allocate new to avoid double free
    void** instruction_pointers = new void*[instructions.size()];
    void** signer_pointers = new void*[signers.size()];

    for(int i = 0; i < instructions.size(); i++){
        Object *object_cast = instructions[i];
        Instruction *element = Object::cast_to<Instruction>(object_cast);
        instruction_pointers[i] = element->to_ptr();
    }

    for(int i = 0; i < signers.size(); i++){
        Object *object_cast = signers[i];
        Keypair *element = Object::cast_to<Keypair>(object_cast);
        signer_pointers[i] = element->to_ptr();
    }

    Object *latest_blockhash_cast = latest_blockhash;
    Pubkey *latest_blockhash_ptr = Object::cast_to<Pubkey>(latest_blockhash_cast);

    Object *payer_cast = payer;
    Pubkey *payer_ptr = Object::cast_to<Pubkey>(payer_cast);

    create_transaction_signed_with_payer(instruction_pointers, instructions.size(), payer_ptr, signer_pointers, signers.size(), latest_blockhash_ptr);
}

Transaction::~Transaction() {
    if(data_pointer != nullptr){
        free_account(data_pointer);
    }
}
}