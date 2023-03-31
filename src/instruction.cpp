#include "instruction.hpp"

#include <godot_cpp/core/class_db.hpp>

namespace godot{

void Instruction::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_new", "program_id", "data", "account_metas"), &Instruction::create_new);
}

Instruction::Instruction() {
    data_pointer = nullptr;
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