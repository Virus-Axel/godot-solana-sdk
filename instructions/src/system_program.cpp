#include "system_program.hpp"

#include "instruction.hpp"

namespace godot{

const std::string SystemProgram::ID = "11111111111111111111111111111111";

void SystemProgram::_bind_methods(){
    ClassDB::bind_static_method("SystemProgram", D_METHOD("transfer", "sender_pubkey", "reciever_pubkey", "lamports"), &SystemProgram::transfer);
}

Variant SystemProgram::transfer(const Variant &sender, const Variant& reciever, const uint32_t lamports){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    data.resize(TRANSFER_DATA_SIZE);

    data[0] = 2;
    data.encode_u64(4, lamports);

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(sender, true, true));
    result->append_meta(AccountMeta(reciever, false, true));

    return result;
}

}