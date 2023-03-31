#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"

#include "utils.hpp"

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

    array_to_pointer_array<Instruction>(instructions, instruction_pointers);
    array_to_pointer_array<Keypair>(signers, signer_pointers);

    Pubkey *latest_blockhash_ptr =  variant_to_type<Pubkey>(latest_blockhash);
    Pubkey *payer_ptr = variant_to_type<Pubkey>(payer);

    create_transaction_signed_with_payer(instruction_pointers, instructions.size(), payer_ptr, signer_pointers, signers.size(), latest_blockhash_ptr);
}

Transaction::~Transaction() {
    if(data_pointer != nullptr){
        free_account(data_pointer);
    }
}
}