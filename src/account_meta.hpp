#ifndef SOLANA_SDK_ACCOUNT_META_HPP
#define SOLANA_SDK_ACCOUNT_META_HPP

#include "../wrapper/wrapper.h"

#include "pubkey.hpp"
#include "account_meta.hpp"
#include "utils.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class AccountMeta : public Resource {
    GDCLASS(AccountMeta, Resource)

private:
    bool is_signer = false;
    bool writeable = false;

    Variant key;

    void *data_pointer;

    void _update_pointer();
    void _free_pointer_if_not_null();

protected:
    static void _bind_methods();

public:
    AccountMeta();

    void set_pubkey(const Variant &p_value);
    Variant get_pubkey() const;

    void set_is_signer(const bool p_value);
    bool get_is_signer() const;

    void set_writeable(const bool p_value);
    bool get_writeable() const;

    bool is_valid() const;
    void *to_ptr();
    void create_new(const Variant& account_key, bool is_signer, bool writeable);

    ~AccountMeta();
};
}

#endif