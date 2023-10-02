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

class Instruction;

class Message{
private:
public:
    Message();
    ~Message();
};

class CompiledKeyMeta{
public:
    bool is_writer = false;
    bool is_signer = false;
    bool is_invoked = false;
};

class CompiledKeys{
private:
    unsigned int num_required_signatures = 0;
    unsigned int num_readonly_signed_accounts = 0;
    unsigned int num_readonly_unsigned_accounts = 0;
    Pubkey* payer = nullptr;
    TypedArray<Pubkey> account_keys;
public:
    CompiledKeys();
    CompiledKeys(TypedArray<Instruction> instructions, Pubkey* payer, const Hash &latest_blockhash);
    ~CompiledKeys();
};

class CompiledInstruction{
private:
    unsigned int program_id_index = 0;
    PackedInt32Array accounts;
    PackedByteArray data;
public:
    CompiledInstruction();
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