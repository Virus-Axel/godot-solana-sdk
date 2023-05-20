#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include <godot_cpp/classes/node.hpp>
#include <hash.hpp>

namespace godot {

class SolanaSDK : public Node {
    GDCLASS(SolanaSDK, Node)

private:

protected:
    static void _bind_methods();

public:
    SolanaSDK();
    static PackedByteArray bs58_decode(String str);
    static String bs58_encode(PackedByteArray input);
    static String get_latest_blockhash();
    static Variant send_transaction(const String& transaction);
    ~SolanaSDK();
};
}

#endif
