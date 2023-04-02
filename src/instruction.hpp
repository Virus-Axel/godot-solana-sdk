#ifndef SOLANA_SDK_INSTRUCTION_HPP
#define SOLANA_SDK_INSTRUCTION_HPP

#include "../wrapper/wrapper.h"
#include "account_meta.hpp"
#include "pubkey.hpp"
#include "utils.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class Instruction : public Resource {
    GDCLASS(Instruction, Resource)

private:
    void *data_pointer;
    Variant program_id;
    PackedByteArray data;
    TypedArray<AccountMeta> accounts;
    bool _is_accounts_valid();
    void _update_pointer();
    void _free_pointer_if_not_null();

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

    void *to_ptr() const;
    void create_new(const Variant& program_id, PackedByteArray data, Array account_metas);

    ~Instruction();
};
}

#endif