#include "instruction.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

CompiledKeys::CompiledKeys(){

}

CompiledKeys::CompiledKeys(TypedArray<Instruction> instructions, Pubkey* payer, Hash &latest_blockhash){
    TypedArray<Pubkey> writable_signer_keys;
    TypedArray<Pubkey> readonly_signer_keys;
    TypedArray<Pubkey> writable_non_signer_keys;
    TypedArray<Pubkey> readonly_non_signer_keys;

    PackedByteArray writable_signer_indicies;
    PackedByteArray readonly_signer_indicies;
    PackedByteArray writable_non_signer_indicies;
    PackedByteArray readonly_non_signer_indicies;

    this->latest_blockhash = &latest_blockhash; 

    for(unsigned int i = 0; i < instructions.size(); i++){
        Instruction *element = Object::cast_to<Instruction>((Object*) instructions[i]);
        Pubkey *program_id = Object::cast_to<Pubkey>((Object*) element->get_program_id());

        readonly_non_signer_keys.append(element->get_program_id());
        const TypedArray<AccountMeta> &account_metas = element->get_accounts();

        for(unsigned int j = 0; j < account_metas.size(); j++){
            AccountMeta *account_meta = Object::cast_to<AccountMeta>((Object*)account_metas[i]);
            if(account_meta->get_is_signer() && account_meta->get_writeable()){
                writable_signer_keys.append(account_meta->get_pubkey());
            }
            else if(!account_meta->get_is_signer() && account_meta->get_writeable()){
                writable_non_signer_keys.append(account_meta->get_pubkey());
            }
            else if(!account_meta->get_is_signer() && !account_meta->get_writeable()){
                readonly_non_signer_keys.append(account_meta->get_pubkey());
            }
            else{
                readonly_signer_keys.append(account_meta->get_pubkey());
            }
        }
    }

    // Loop again to figure out indices.
    for(unsigned int i = 0; i < instructions.size(); i++){
        Instruction *element = Object::cast_to<Instruction>((Object*) instructions[i]);
        const TypedArray<AccountMeta> &account_metas = element->get_accounts();

        CompiledInstruction compiled_instruction;
        compiled_instruction.data = element->get_data();
        
        compiled_instruction.program_id_index =
            writable_signer_keys.size() +
            readonly_signer_keys.size() +
            writable_non_signer_keys.size() +
            readonly_non_signer_indicies.size();

        for(unsigned int j = 0; j < account_metas.size(); j++){

            AccountMeta *account_meta = Object::cast_to<AccountMeta>((Object*)account_metas[i]);
            if(account_meta->get_is_signer() && account_meta->get_writeable()){
                std::cout << "Pushing (1) " << 
                    writable_signer_indicies.size()
                    << std::endl;
                writable_signer_indicies.push_back(
                    writable_signer_indicies.size()
                    );
                compiled_instruction.accounts.push_back(*writable_signer_indicies.end());
            }
            else if(!account_meta->get_is_signer() && account_meta->get_writeable()){
                std::cout << "Pushing (2) " << 
                writable_signer_keys.size() +
                    readonly_signer_keys.size() +
                    writable_non_signer_keys.size() +
                    readonly_non_signer_indicies.size()
                    << std::endl;
                writable_non_signer_indicies.push_back(
                    writable_signer_keys.size() +
                    readonly_signer_keys.size() +
                    writable_non_signer_indicies.size()
                );
                compiled_instruction.accounts.push_back(*writable_non_signer_indicies.end());
            }
            else if(!account_meta->get_is_signer() && !account_meta->get_writeable()){
                std::cout << "Pushing (3) " << 
                writable_signer_keys.size() +
                    readonly_signer_keys.size() +
                    writable_non_signer_keys.size() +
                    readonly_non_signer_indicies.size()
                    << std::endl;
                readonly_non_signer_indicies.push_back(
                    writable_signer_keys.size() +
                    readonly_signer_keys.size() +
                    writable_non_signer_keys.size() +
                    readonly_non_signer_indicies.size());
                compiled_instruction.accounts.push_back(*readonly_non_signer_indicies.end());
            }
            else{
                std::cout << "Pushing (4) " << 
                writable_signer_keys.size() +
                    readonly_signer_keys.size() +
                    writable_non_signer_keys.size() +
                    readonly_non_signer_indicies.size()
                    << std::endl;
                readonly_signer_indicies.push_back(
                    writable_signer_keys.size() +
                    readonly_signer_indicies.size()
                );
                compiled_instruction.accounts.push_back(*readonly_signer_indicies.end());
            }
        }
        compiled_instructions.push_back(compiled_instruction);
    }

    account_keys.append_array(writable_signer_keys);
    account_keys.append_array(readonly_signer_keys);
    account_keys.append_array(writable_non_signer_keys);
    account_keys.append_array(readonly_non_signer_keys);

    num_required_signatures = writable_signer_keys.size() + readonly_signer_keys.size();
    num_readonly_unsigned_accounts = readonly_non_signer_keys.size();
    num_readonly_signed_accounts = readonly_signer_keys.size();
}

PackedByteArray CompiledKeys::serialize(){
    PackedByteArray result;
    result.append(num_required_signatures);
    result.append(num_readonly_signed_accounts);
    result.append(num_readonly_unsigned_accounts);

    result.append(account_keys.size());
    for(unsigned int i = 0; i < account_keys.size(); i++){
        Pubkey *account_key = Object::cast_to<Pubkey>(account_keys[i]);
        result.append_array(account_key->get_bytes());
    }

    result.append(1);
    //Hash *latest_blockhash_pubkey = Object::cast_to<Hash>(latest_blockhash);
    result.append_array(latest_blockhash->get_bytes());

    result.append(compiled_instructions.size());
    for(unsigned int i = 0; i < compiled_instructions.size(); i++){
        result.append_array(compiled_instructions[i].serialize());
    }
    return result;
}

CompiledKeys::~CompiledKeys(){
}

void Instruction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_program_id"), &Instruction::get_program_id);
    ClassDB::bind_method(D_METHOD("set_program_id", "p_value"), &Instruction::set_program_id);
    ClassDB::bind_method(D_METHOD("get_data"), &Instruction::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "p_value"), &Instruction::set_data);
    ClassDB::bind_method(D_METHOD("get_accounts"), &Instruction::get_accounts);
    ClassDB::bind_method(D_METHOD("set_accounts", "p_value"), &Instruction::set_accounts);
    ClassDB::add_property("Instruction", PropertyInfo(Variant::OBJECT, "program_id", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_DEFAULT), "set_program_id", "get_program_id");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::ARRAY, "accounts", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("AccountMeta")), "set_accounts", "get_accounts");}

Instruction::Instruction() {
}

void Instruction::set_program_id(const Variant& p_value){
    program_id = p_value;
}
Variant Instruction::get_program_id(){
    return program_id;
}

void Instruction::set_data(const PackedByteArray& p_value){
    data = p_value;
}
PackedByteArray Instruction::get_data(){
    return data;
}

void Instruction::set_accounts(const TypedArray<AccountMeta>& p_value){
    accounts = p_value;
}

TypedArray<AccountMeta> Instruction::get_accounts(){
    return accounts;
}



Instruction::~Instruction() {
}

CompiledInstruction::CompiledInstruction(){

}

void CompiledInstruction::_bind_methods(){

}

PackedByteArray CompiledInstruction::serialize(){
    PackedByteArray result;

    result.append((int64_t) 0);
    result.append(accounts.size());
    for(unsigned int i = 0; i < accounts.size(); i++){
        result.append(accounts[i]);
    }

    result.append(data.size());
    for(unsigned int i = 0; i < data.size(); i++){
        result.append(data[i]);
    }

    return result;
}

CompiledInstruction& CompiledInstruction::operator=(const CompiledInstruction& other){
    this->accounts = other.accounts;
    this->data = other.data;
    this->program_id_index = other.program_id_index;

    return *this;
}

CompiledInstruction::~CompiledInstruction(){

}

}