#ifndef SOLANA_SDK_MESSAGE_HPP
#define SOLANA_SDK_MESSAGE_HPP

#include <instruction.hpp>
#include <godot_cpp/classes/node.hpp>

namespace godot{

MAKE_TYPED_ARRAY(CompiledInstruction, Variant::OBJECT)
MAKE_TYPED_ARRAY(Instruction, Variant::OBJECT)

class Message: public Resource{ // Message
    GDCLASS(Message, Resource)
private:
    uint8_t payer_index = 0;
    uint8_t num_required_signatures = 0;
    uint8_t num_readonly_signed_accounts = 0;
    uint8_t num_readonly_unsigned_accounts = 0;
    TypedArray<Resource> account_keys;
    Variant latest_blockhash;
    TypedArray<CompiledInstruction> compiled_instructions;
    TypedArray<Resource> signers;

    TypedArray<Resource> merged_metas;

    void compile_instruction(Variant instruction);
    void recalculate_headers();

    int locate_account_meta(const TypedArray<Resource>& arr, const AccountMeta &input);

protected:
    static void _bind_methods();

public:
    Message();
    Message(TypedArray<Instruction> instructions, Variant &payer);
    void set_latest_blockhash(const String& blockhash);
    PackedByteArray serialize();
    PackedByteArray serialize_blockhash();
    int get_amount_signers();
    TypedArray<Resource> &get_signers();
    ~Message();
};
}

#endif