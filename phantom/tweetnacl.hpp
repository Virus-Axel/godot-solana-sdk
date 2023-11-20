#ifndef SOLANA_SDK_PHANTOM
#define SOLANA_SDK_PHANTOM

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot{
class TweetNacl : public Node{
    GDCLASS(TweetNacl, Node)
private:
    bool connected = false;
    uint32_t active_signer_index = 0;
    enum State{
        IDLE = 0,
        CONNECTING = 1,
        SIGNING = 2,
    };

    PackedByteArray connected_key;
    State phantom_state = State::IDLE;

    void clear_state();
    void store_encoded_message(const PackedByteArray &store_serialized_message);

protected:
    static void _bind_methods();
public:
    void _process(double delta) override;
    PhantomController();
    void deeplink_connect();
    void connect_phantom();
    bool is_connected();
    bool is_idle();
    void poll_connection();
    void poll_message_signing();
    PackedByteArray get_connected_key();
    PackedByteArray get_message_signature();
    void sign_message(const PackedByteArray &serialized_message, const uint32_t index);

    uint32_t get_active_signer_index();
    ~PhantomController();
};
}

#endif