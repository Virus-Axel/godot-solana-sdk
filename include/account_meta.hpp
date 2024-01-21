#ifndef SOLANA_SDK_ACCOUNT_META_HPP
#define SOLANA_SDK_ACCOUNT_META_HPP

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

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

public:
    AccountMeta();
    AccountMeta(const Variant& pid, bool signer, bool writeable);
    AccountMeta(const Variant& other);

    void set_pubkey(const Variant &p_value);
    Variant get_pubkey() const;

    Variant get_signer() const;

    void set_is_signer(const bool p_value);
    bool get_is_signer() const;

    void set_writeable(const bool p_value);
    bool get_writeable() const;

    void create_new(const Variant& account_key, bool is_signer, bool writeable);
    static Variant new_account_meta(const Variant& account_key, bool is_signer, bool writeable);

    bool operator==(const AccountMeta& other) const;

    ~AccountMeta();
};
}

#endif