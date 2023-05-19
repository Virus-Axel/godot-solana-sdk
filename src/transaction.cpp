#include "transaction.hpp"
#include "instruction.hpp"
#include "keypair.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

using internal::gde_interface;

void Transaction::_update_pointer(){
    void* instruction_pointers[instructions.size()];

    // Write instruction pointer array.
    if (!array_to_pointer_array<Instruction>(instructions, instruction_pointers)){
        gde_interface->print_warning("Bad Instruction array", "_update_pointer", "transaction.cpp", 17, false);
        return;
    }

    // Get payer pointer.
    void *payer_ptr = variant_to_type<Pubkey>(payer);
    if(payer_ptr == nullptr){
        gde_interface->print_warning("Bad transaction payer", "_update_pointer", "transaction.cpp", 23, false);
        return;
    }

    data_pointer = create_transaction_unsigned_with_payer(instruction_pointers, instructions.size(), payer_ptr);
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

    ClassDB::bind_method(D_METHOD("create_signed_with_payer", "instructions", "payer", "signers", "latest_blockhash"), &Transaction::create_signed_with_payer);
    ClassDB::bind_method(D_METHOD("serialize"), &Transaction::serialize);
    ClassDB::bind_method(D_METHOD("sign", "latest_blockhash"), &Transaction::sign);
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
    p_list->push_back(PropertyInfo(Variant::OBJECT, "payer", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey"));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "instructions", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("Instruction")));
    p_list->push_back(PropertyInfo(Variant::ARRAY, "signers", PROPERTY_HINT_NONE, MAKE_RESOURCE_TYPE_HINT("Keypair")));
}

Transaction::Transaction() {
}

void Transaction::create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash){
    void* instruction_pointers[instructions.size()];
    void* signer_pointers[signers.size()];

    array_to_pointer_array<Instruction>(instructions, instruction_pointers);
    array_to_pointer_array<Keypair>(signers, signer_pointers);

    void *latest_blockhash_ptr =  variant_to_type<Pubkey>(latest_blockhash);
    void *payer_ptr = variant_to_type<Pubkey>(payer);

    create_transaction_signed_with_payer(instruction_pointers, instructions.size(), payer_ptr, signer_pointers, signers.size(), latest_blockhash_ptr);
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
    // Get and verify transaction pointer.
    void* tx = to_ptr();
    if (tx == nullptr){
        return PackedByteArray();
    }

    // Write the serialized transaction into a buffer.
    unsigned char buffer[MAXIMUM_SERIALIZED_BUFFER + 1];
    int written_bytes = serialize_transaction(tx, buffer, MAXIMUM_SERIALIZED_BUFFER);

    // Return the amount of bytes written.
    if (written_bytes <= 0)
        return PackedByteArray();
    else{
        PackedByteArray ret;
        ret.resize(written_bytes);
        for(int i = 0; i < written_bytes; i++){
            ret[i] = buffer[i];
        }
        return ret;
    }
}

Error Transaction::sign(const Variant& latest_blockhash){
    // Check Variant type.
    if (latest_blockhash.get_type() != Variant::OBJECT){
        gde_interface->print_warning("Latest Blockhash must be a Hash object", "sign", "transaction.cpp", 170, false);
        return Error::ERR_INVALID_PARAMETER;
    }

    // Get Hash and validate it.
    void* latest_blockhash_ptr = variant_to_type<Hash>(latest_blockhash);
    if(latest_blockhash_ptr == nullptr){
        gde_interface->print_warning("Provided hash is invalid.", "sign", "transaction.cpp", 179, false);
        return Error::ERR_INVALID_PARAMETER;
    }

    // Get transaction pointer and validate is.
    void* tx = to_ptr();
    if (tx == nullptr){
        return Error::ERR_INVALID_DATA;
    }

    // Get array of pointers to signers
    void* signer_pointers[signers.size()];
    if(!array_to_pointer_array<Keypair>(signers, signer_pointers)){
        gde_interface->print_warning("Bad signer array", "sign", "transaction.cpp", 192, false);
        return Error::ERR_INVALID_DATA;
    }

    int status = sign_transaction(tx, signer_pointers, signers.size(), latest_blockhash_ptr);

    // Check status from rust library.
    if (status != 0){
        gde_interface->print_warning("Unknown signing error", "sign", "transaction.cpp", 198, false);
        return Error::ERR_INVALID_DATA;
    }

    return OK;
}

Error Transaction::partially_sign(const Variant& latest_blockhash){
    // Check type of latest blockhash.
    if (latest_blockhash.get_type() != Variant::OBJECT){
        gde_interface->print_warning("Latest Blockhash must be a Hash object", "sign", "transaction.cpp", 209, false);
        return Error::ERR_INVALID_PARAMETER;
    }

    // Check if blockhash is valid.
    void* latest_blockhash_ptr = variant_to_type<Hash>(latest_blockhash);
    if(latest_blockhash_ptr == nullptr){
        gde_interface->print_warning("Provided hash is invalid.", "sign", "transaction.cpp", 216, false);
        return Error::ERR_INVALID_PARAMETER;
    }

    // Get pointer to transaction.
    void* tx = to_ptr();
    if (tx == nullptr){
        return Error::ERR_INVALID_DATA;
    }

    // Write an array of pointers to signers.
    void* signer_pointers[signers.size()];
    if(!array_to_pointer_array<Keypair>(signers, signer_pointers)){
        return Error::ERR_INVALID_DATA;
    }

    int status = partially_sign_transaction(tx, signer_pointers, signers.size(), latest_blockhash_ptr);

    // Check status of rust library function.
    if (status != 0){
        gde_interface->print_warning("Unknown signing error", "sign", "transaction.cpp", 236, false);
        return Error::ERR_INVALID_DATA;
    }

    return OK;
}

Transaction::~Transaction(){
    _free_pointer_if_not_null();
}
}