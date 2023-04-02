#include "instruction.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

bool Instruction::_is_accounts_valid(){
    for(int i = 0; i < accounts.size(); i++){

        if(accounts[i].get_type() != Variant::OBJECT){
            return false;
        }

        AccountMeta *account_ref = variant_to_type<AccountMeta>(accounts[i]);
        if(!account_ref->is_valid()){
            return false;
        }
    }
    return true;
}

void Instruction::_free_pointer_if_not_null(){
    if(data_pointer != nullptr){
        free_instruction(data_pointer);
        data_pointer = nullptr;
    }
}

void Instruction::_update_pointer(){
    _free_pointer_if_not_null();
    if(program_id.get_type() != Variant::OBJECT){
        return;
    }
    Pubkey *program_id_ref = variant_to_type<Pubkey>(program_id);
    if(program_id_ref->is_null()){
        return;
    }
    if(!_is_accounts_valid()){
        return;
    }
    
    // Rust wants to deallocate its memory, so we allocate new to avoid double free
    void** account_pointers = new void*[accounts.size()];
    unsigned char* allocated_data = new unsigned char[data.size()]; 

    array_to_pointer_array<AccountMeta>(accounts, account_pointers);

    Object *program_id_cast = program_id;
    Pubkey *program_id_ptr = Object::cast_to<Pubkey>(program_id_cast);

    memcpy(allocated_data, data.get_string_from_utf8().utf8().get_data(), data.size());

    data_pointer = create_instruction_with_bytes(program_id_ptr->to_ptr(), allocated_data, data.size(), account_pointers, accounts.size());
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
    ClassDB::add_property("Instruction", PropertyInfo(Variant::ARRAY, "accounts", PROPERTY_HINT_ARRAY_TYPE, "AccountMeta"), "set_accounts", "get_accounts");

    ClassDB::bind_method(D_METHOD("create_new", "program_id", "data", "account_metas"), &Instruction::create_new);
}

Instruction::Instruction() {
    data_pointer = nullptr;
}

void Instruction::set_program_id(const Variant& p_value){
    program_id = p_value;
    _update_pointer();
}
Variant Instruction::get_program_id(){
    return program_id;
}

void Instruction::set_data(const PackedByteArray& p_value){
    data = p_value;
    _update_pointer();
}
PackedByteArray Instruction::get_data(){
    return data;
}

void Instruction::set_accounts(const TypedArray<AccountMeta>& p_value){
    accounts = p_value;
    for(int i = 0; i < accounts.size(); i++){
        if(accounts[i].get_type() == Variant::NIL){
            Ref<AccountMeta> resource = memnew(AccountMeta);
            accounts[i] = Variant(resource);
        }
    }
    _update_pointer();
}
TypedArray<AccountMeta> Instruction::get_accounts(){
    return accounts;
}

void *Instruction::to_ptr() const{
    return data_pointer;
}

void Instruction::create_new(const Variant& program_id, PackedByteArray data, Array account_metas){
    // Rust wants to deallocate its memory, so we allocate new to avoid double free
    void** account_pointers = new void*[account_metas.size()];
    unsigned char* allocated_data = new unsigned char[data.size()]; 

    for(int i = 0; i < account_metas.size(); i++){
        Object *object_cast = account_metas[i];
        AccountMeta *element = Object::cast_to<AccountMeta>(object_cast);
        account_pointers[i] = element->to_ptr();
    }

    Object *program_id_cast = program_id;
    Pubkey *program_id_ptr = Object::cast_to<Pubkey>(program_id_cast);

    memcpy(allocated_data, data.get_string_from_utf8().utf8().get_data(), data.size());

    data_pointer = create_instruction_with_bytes(program_id_ptr->to_ptr(), allocated_data, data.size(), account_pointers, account_metas.size());
}

Instruction::~Instruction() {
    if(data_pointer != nullptr){
        free_instruction(data_pointer);
    }
}
}