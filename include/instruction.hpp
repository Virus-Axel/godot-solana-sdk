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

MAKE_TYPED_ARRAY(CompiledInstruction, Variant::OBJECT)
MAKE_TYPED_ARRAY(Instruction, Variant::OBJECT)

class CompiledKeys: public Resource{ // Message
    GDCLASS(CompiledKeys, Resource)
private:
    uint8_t num_required_signatures = 0;
    uint8_t num_readonly_signed_accounts = 0;
    uint8_t num_readonly_unsigned_accounts = 0;
    TypedArray<Resource> account_keys;
    Variant latest_blockhash;
    TypedArray<CompiledInstruction> compiled_instructions;
    TypedArray<Resource> signers;

    int locate_account_meta(const TypedArray<Resource>& arr, const AccountMeta &input);

protected:
    static void _bind_methods();

public:
    CompiledKeys();
    CompiledKeys(TypedArray<Instruction> instructions, Variant &payer, Variant &latest_blockhash);
    PackedByteArray serialize();
    TypedArray<Resource> &get_signers();
    ~CompiledKeys();
};

}

#endif