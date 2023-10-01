#include "instruction.hpp"

#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

using internal::gdextension_interface_print_warning;

void Instruction::_free_pointer(){
    ;//free_instruction(data_pointer);
}

void Instruction::_update_pointer(){
    // Allocate buffers.
    void* account_pointers[accounts.size()];
    unsigned char allocated_data[data.size()];

    // Write account pointer array.
    if (!array_to_pointer_array<AccountMeta>(accounts, account_pointers)){
        gdextension_interface_print_warning("Bad accounts", "_update_pointer", "instruction.cpp", 20, false);
        return;
    }

    // Get program ID object.
    void *program_id_ptr = variant_to_type<Pubkey>(program_id);
    if(program_id_ptr == nullptr){
        gdextension_interface_print_warning("Bad program ID", "_update_pointer", "instruction.cpp", 26, false);
        return;
    }

    // Rust requires a mutable pointer so we make a copy.
    memcpy(allocated_data, data.get_string_from_utf8().utf8().get_data(), data.size());

    data_pointer = nullptr;//create_instruction_with_bytes(program_id_ptr, allocated_data, data.size(), account_pointers, accounts.size());
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
    _free_pointer_if_not_null();
}
}