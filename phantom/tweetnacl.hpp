#ifndef SOLANA_SDK_PHANTOM
#define SOLANA_SDK_PHANTOM

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/wrapped.hpp>

namespace godot{
class TweetNacl : public Node{
    GDCLASS(TweetNacl, Node)
private:
    static PackedByteArray _0();
    static PackedByteArray _9();

    static PackedInt64Array gf0();
    static PackedInt64Array gf1();
    static PackedInt64Array _121665();
    static PackedInt64Array D();
    static PackedInt64Array D2();
    static PackedInt64Array X();
    static PackedInt64Array Y();
    static PackedInt64Array I();

    static PackedByteArray sigma();
    static PackedInt32Array minusp();

    static void crypto_scalarmult(PackedByteArray &q, const PackedByteArray &n, const PackedByteArray &p);

    static void crypto_box_keypair(PackedByteArray& y, PackedByteArray &x);
    static void crypto_scalarmult_base(PackedByteArray &q, const PackedByteArray &n);
protected:
    static void _bind_methods();
public:
    static TypedArray<PackedByteArray> box_keypair();
};
}

#endif