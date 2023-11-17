#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include "hash.hpp"
#include "pubkey.hpp"
#include "account.hpp"
#include "instruction.hpp"

#include <godot_cpp/classes/node.hpp>

namespace godot{

class Transaction : public Node {
    GDCLASS(Transaction, Node)

private:
    Variant message;

    Array instructions;
    Variant payer;
    Array signers;
    TypedArray<PackedByteArray> signatures;

    String latest_blockhash_string = "";

    bool has_cumpute_budget_instructions = false;
    bool use_phantom_payer = false;

    void _get_property_list(List<PropertyInfo> *p_list) const;
    void _payer_signed(PackedByteArray signature);

    bool is_phantom_payer() const;
    void create_message();

protected:
    static void _bind_methods();

public:
    Transaction();

    void set_instructions(const Array& p_value);
    Array get_instructions();

    void set_payer(const Variant& p_value);
    Variant get_payer();

    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

    void set_signers(const Array& p_value);
    Array get_signers();

    void set_use_phantom_payer(bool p_value);
    bool get_use_phantom_payer();

    void update_latest_blockhash(const String &custom_hash = "");

    PackedByteArray serialize();
    PackedByteArray serialize_message();
    PackedByteArray serialize_signers();
    Error sign();
    Dictionary send();
    Variant sign_and_send();
    Error partially_sign(const Variant& latest_blockhash);

    void create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash);

    ~Transaction();
};
}

#endif