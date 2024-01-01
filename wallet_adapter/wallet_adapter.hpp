#ifndef SOLANA_SDK_WALLET_ADAPTER_HPP
#define SOLANA_SDK_WALLET_ADAPTER_HPP

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot{
class WalletAdapter : public Node{
    GDCLASS(WalletAdapter, Node)
private:
    bool connected = false;
    uint32_t active_signer_index = 0;
    enum State{
        IDLE = 0,
        CONNECTING = 1,
        SIGNING = 2,
    };
    enum WalletType{
        PHANTOM = 0,
        SOLFLARE = 1,
        BACKPACK = 2,
        MAX_TYPES = 3,
    };

    PackedByteArray connected_key;
    State wallet_state = State::IDLE;
    WalletType wallet_type = PHANTOM;

    void clear_state();
    void store_serialized_message(const PackedByteArray &serialized_message);
    void store_encoded_message(const PackedByteArray &store_serialized_message);
    String wallet_name_from_type(WalletType wallet_type);
    String wallet_check_name_from_type(WalletType wallet_type);

    String get_sign_transaction_script();
    String get_sign_message_script();
    String get_connect_script();

protected:
    static void _bind_methods();
public:
    void _process(double delta) override;
    WalletAdapter();

    void connect_wallet();
    
    bool is_connected();
    bool is_idle();
    bool has_multiple_wallets();

    void poll_connection();
    void poll_message_signing();
    PackedByteArray get_connected_key();
    PackedByteArray get_message_signature();

    Array get_available_wallets();

    void set_wallet_type(int wallet_type);
    int get_wallet_type();

    void sign_message(const PackedByteArray &serialized_message, const uint32_t index);
    void sign_text_message(const String& message);

    uint32_t get_active_signer_index();
    ~WalletAdapter();
};
}

#endif