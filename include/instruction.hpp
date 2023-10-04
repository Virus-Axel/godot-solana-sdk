#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "../wrapper/wrapper.h"
#include "account_meta.hpp"
#include "pubkey.hpp"
#include "utils.hpp"
#include "solana_resource.hpp"
#include "hash.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

// Predefine some classes.
class Instruction;
class CompiledInstruction;

class Message{
private:
public:
    Message();
    ~Message();
};


class CompiledKeys: public Resource{ // Message

private:
    uint8_t num_required_signatures = 0;
    uint8_t num_readonly_signed_accounts = 0;
    uint8_t num_readonly_unsigned_accounts = 0;
    TypedArray<Pubkey> account_keys;
    Variant latest_blockhash;
    TypedArray<CompiledInstruction> compiled_instructions;

public:
    CompiledKeys();
    CompiledKeys(TypedArray<Instruction> instructions, Pubkey* payer, const Variant &latest_blockhash);
    PackedByteArray serialize();
    ~CompiledKeys();
};

class CompiledInstruction: public Resource{
    GDCLASS(CompiledInstruction, Resource)

protected:
    static void _bind_methods();

public:
    uint8_t program_id_index = 0;
    PackedByteArray accounts;
    PackedByteArray data;

    CompiledInstruction();

    //CompiledInstruction& operator=(const CompiledInstruction& other);
    PackedByteArray serialize();

    ~CompiledInstruction();
};

class Instruction : public SolanaResource {
    GDCLASS(Instruction, Resource)

private:
    Variant program_id;
    PackedByteArray data;
    TypedArray<AccountMeta> accounts;
    void _update_pointer() override;
    void _free_pointer() override;

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
}

#endif