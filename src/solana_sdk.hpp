#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include "../wrapper/wrapper.h"
#include <godot_cpp/classes/node.hpp>

namespace godot {

class SolanaSDK : public Node {
    GDCLASS(SolanaSDK, Node)

private:

protected:
    static void _bind_methods();

public:
    SolanaSDK();
    Error append_account(String account_key);
    ~SolanaSDK();
};
}

#endif
