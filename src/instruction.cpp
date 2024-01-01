#include "instruction.hpp"
#include "keypair.hpp"
#include "solana_sdk.hpp"

#include "utils.hpp"

#include <vector>
#include <algorithm>

#include <godot_cpp/core/class_db.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

bool Instruction::is_serializable(){
    if(program_id.get_type() != Variant::OBJECT){
        return false;
    }
    for(unsigned int i = 0; i < accounts.size(); i++){
        if(accounts[i].get_type() != Variant::OBJECT){
            return false;
        }
    }
    return true;
}

void Instruction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_program_id"), &Instruction::get_program_id);
    ClassDB::bind_method(D_METHOD("set_program_id", "p_value"), &Instruction::set_program_id);
    ClassDB::bind_method(D_METHOD("get_data"), &Instruction::get_data);
    ClassDB::bind_method(D_METHOD("set_data", "p_value"), &Instruction::set_data);
    ClassDB::bind_method(D_METHOD("get_accounts"), &Instruction::get_accounts);
    ClassDB::bind_method(D_METHOD("set_accounts", "p_value"), &Instruction::set_accounts);
    ClassDB::bind_method(D_METHOD("serialize"), &Instruction::serialize);

    ClassDB::add_property("Instruction", PropertyInfo(Variant::OBJECT, "program_id", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", PROPERTY_USAGE_DEFAULT), "set_program_id", "get_program_id");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"), "set_data", "get_data");
    ClassDB::add_property("Instruction", PropertyInfo(Variant::ARRAY, "accounts", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("AccountMeta")), "set_accounts", "get_accounts");
}

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

void Instruction::append_meta(const AccountMeta& meta){
    accounts.append(memnew(AccountMeta(meta)));
}

PackedByteArray Instruction::serialize(){
    PackedByteArray result;

    if(program_id.has_method("get_bytes")){
        result.append_array(Object::cast_to<Pubkey>(program_id)->get_bytes());
    }
    result.append_array(data);
    for(unsigned int i = 0; i < accounts.size(); i++){
        const Pubkey key = accounts[i];

        result.append_array(key.get_bytes());
    }

    return result;
}

Instruction::~Instruction() {
}

CompiledInstruction::CompiledInstruction(){
    accounts = PackedByteArray();
    data = PackedByteArray();
}

void CompiledInstruction::_bind_methods(){
}

PackedByteArray CompiledInstruction::serialize(){
    PackedByteArray result;

    result.append(program_id_index);
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

CompiledInstruction::~CompiledInstruction(){

}

}