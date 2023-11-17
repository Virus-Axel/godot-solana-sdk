#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "account_meta.hpp"
#include "pubkey.hpp"
#include "utils.hpp"
#include "hash.hpp"
#include <vector>

#include <godot_cpp/classes/node.hpp>

namespace godot{

MAKE_TYPED_ARRAY(Pubkey, Variant::OBJECT)
MAKE_TYPED_ARRAY(AccountMeta, Variant::OBJECT)
MAKE_TYPED_ARRAY(Resource, Variant::OBJECT)

class CompiledInstruction;

class Instruction : public Resource {
    GDCLASS(Instruction, Resource)

private:
    Variant program_id;
    PackedByteArray data;
    TypedArray<AccountMeta> accounts;

protected:
    static void _bind_methods();

public:
    Instruction();

    void set_program_id(const Variant& p_value);
    Variant get_program_id();

    void set_data(const PackedByteArray& p_value);
    PackedByteArray get_data();

    void set_accounts(const TypedArray<AccountMeta>& p_value);
    TypedArray<AccountMeta> get_accounts();

    PackedByteArray serialize();
    
    ~Instruction();
};

class CompiledInstruction : public Object{
    GDCLASS(CompiledInstruction, Object)

protected:
    static void _bind_methods();

public:
    uint8_t program_id_index = 0;
    PackedByteArray accounts;
    PackedByteArray data;

    CompiledInstruction();
    //CompiledInstruction(godot::Variant& other);

    //const CompiledInstruction& operator=(const CompiledInstruction& other);
    PackedByteArray serialize();

    ~CompiledInstruction();
};

}

#endif