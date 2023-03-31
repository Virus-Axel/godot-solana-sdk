#ifndef SOLANA_SDK_ACCOUNT_META_HPP
#define SOLANA_SDK_ACCOUNT_META_HPP

#include "../wrapper/wrapper.h"

#include "pubkey.hpp"
#include "account_meta.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class AccountMeta : public Node {
    GDCLASS(AccountMeta, Node)

private:
    void *data_pointer;

protected:
    static void _bind_methods();

public:
    AccountMeta();

    void *to_ptr();
    void create_new(const Variant& account_key, bool is_signer, bool writeable);

    ~AccountMeta();
};
}

#endif