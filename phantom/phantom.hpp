#ifndef SOLANA_SDK_PHANTOM
#define SOLANA_SDK_PHANTOM

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot{
class PhantomController : public Control{
    GDCLASS(PhantomController, Control)
private:
    bool connected = false;
    enum State{
        IDLE = 0,
        CONNECTING = 1,
    };

    PackedByteArray connected_key;
    State phantom_state = State::IDLE;

    void poll_connection();

protected:
    static void _bind_methods();
public:
    static int data;
    static void set_data();
    void _process(double delta) override;

    PhantomController();
    void connect_phantom();
    void sign_and_send_transaction();
    ~PhantomController();
};
}

#endif