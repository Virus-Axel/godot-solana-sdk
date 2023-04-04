#include "instruction.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Instruction::_free_pointer(){
    free_instruction(data_pointer);
}

void Instruction::_update_pointer(){
    // Rust wants to deallocate its memory, so we allocate new to avoid double free
    void** account_pointers = new void*[accounts.size()];
    unsigned char* allocated_data = new unsigned char[data.size()];

    if (!array_to_pointer_array<AccountMeta>(accounts, account_pointers)){
        std::cout << "accounts are bad" << std::endl;
        return;
    }

    void *program_id_ptr = variant_to_type<Pubkey>(program_id);
    if(program_id_ptr == nullptr){
        std::cout << "program id is bad" << std::endl;
        return;
    }

    memcpy(allocated_data, data.get_string_from_utf8().utf8().get_data(), data.size());

    data_pointer = create_instruction_with_bytes(program_id_ptr, allocated_data, data.size(), account_pointers, accounts.size());
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
    for(int i = 0; i < accounts.size(); i++){
        if(accounts[i].get_type() == Variant::NIL){
            Ref<AccountMeta> resource = memnew(AccountMeta);
            accounts[i] = Variant(resource);
        }
    }
}

TypedArray<AccountMeta> Instruction::get_accounts(){
    return accounts;
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
    _free_pointer_if_not_null();
}
}