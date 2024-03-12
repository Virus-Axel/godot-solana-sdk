#ifndef SOLANA_SDK_TRANSACTION_HPP
#define SOLANA_SDK_TRANSACTION_HPP

#include "hash.hpp"
#include "pubkey.hpp"
#include "account.hpp"
#include "instruction.hpp"

#include <godot_cpp/classes/node.hpp>
#include <solana_client.hpp>

namespace godot{

class Transaction : public Node {
    GDCLASS(Transaction, Node)

private:
    uint32_t ready_signature_amount = 0;

    Variant message;

    bool is_fully_signed = false;
    Array instructions;
    Variant payer;
    Array signers;
    TypedArray<PackedByteArray> signatures;

    String latest_blockhash_string = "";
    String result_signature = "";
    String latest_commitment = "";
    String url = "https://api.devnet.solana.com";

    SolanaClient *send_client;
    SolanaClient *blockhash_client;
    SolanaClient *poll_client;

    bool has_cumpute_budget_instructions = false;
    bool external_payer = false;
    bool pending_blockhash = false;
    bool pending_send = false;
    bool pending_poll = false;

    void _get_property_list(List<PropertyInfo> *p_list) const;
    void _signer_signed(PackedByteArray signature);
    void _signer_failed();

    bool is_phantom_payer() const;
    bool is_message_valid();
    void create_message();
    void check_fully_signed();

    void sign_at_index(const uint32_t index);

protected:
    static void _bind_methods();

public:
    Transaction();
    void _ready() override;
    void _process(double delta) override;

    void set_latest_commitment(const Dictionary& value);

    void set_instructions(const Array& p_value);
    Array get_instructions();

    void set_payer(const Variant& p_value);
    Variant get_payer();

    void set_url(const String& p_value);

    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

    void set_signers(const Array& p_value);
    Array get_signers();

    void set_external_payer(bool p_value);
    bool get_external_payer();

    void update_latest_blockhash(const String &custom_hash = "");
    void add_instruction(const Variant &instruction);

    PackedByteArray serialize();
    PackedByteArray serialize_message();
    PackedByteArray serialize_signers();
    Error sign();
    Dictionary send();
    void send_and_disconnect();
    Variant sign_and_send();
    Error partially_sign(const Variant& latest_blockhash);

    void create_signed_with_payer(Array instructions, Variant payer, Array signers, Variant latest_blockhash);
    
    void send_callback(Dictionary params);
    void blockhash_callback(Dictionary params);

    bool is_confirmed();
    bool is_finalized();

    ~Transaction();
};
}

#endif