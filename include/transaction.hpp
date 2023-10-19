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
    PackedByteArray signatures;
    bool use_phantom_payer;

    void _get_property_list(List<PropertyInfo> *p_list) const;

    void _transaction_response(int result, int response_code, PackedStringArray headers, PackedByteArray body);
    void _payer_signed(PackedByteArray signature);

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

    PackedByteArray serialize();
    Error sign(const Variant &latest_blockhash);
    Error send();
    Variant sign_and_send();
    Error partially_sign(const Variant& latest_blockhash);

    void create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash);

    ~Transaction();
};
}

#endif