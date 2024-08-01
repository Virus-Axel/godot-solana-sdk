#ifndef SOLANA_SDK_MESSAGE_HPP
#define SOLANA_SDK_MESSAGE_HPP

#include <instruction.hpp>
#include <godot_cpp/classes/node.hpp>

#include "keypair.hpp"

namespace godot{

class Message: public Resource{ // Message
    GDCLASS(Message, Resource)
private:
    uint8_t payer_index = 0;
    uint8_t num_required_signatures = 0;
    uint8_t num_readonly_signed_accounts = 0;
    uint8_t num_readonly_unsigned_accounts = 0;
    TypedArray<Pubkey> account_keys;
    String latest_blockhash;
    TypedArray<CompiledInstruction> compiled_instructions;
    Array signers;

    TypedArray<AccountMeta> merged_metas;

    void compile_instruction(Variant instruction);
    void recalculate_headers();
    void merge_account_meta(const AccountMeta& account_meta);
    void merge_signer(const Variant& signer);

    int locate_account_meta(const TypedArray<AccountMeta>& arr, const AccountMeta &input);

protected:
    static void _bind_methods();

public:
    Message();
    Message(TypedArray<Instruction> instructions, Variant &payer, uint32_t unit_limit, uint32_t unit_price);
    Message(const PackedByteArray& bytes);
    void set_latest_blockhash(const String& blockhash);
    PackedByteArray serialize();
    PackedByteArray serialize_blockhash();
    int get_amount_signers();
    void set_signers(const Array& signers);
    Array &get_signers();
    ~Message();
};
}

#endif