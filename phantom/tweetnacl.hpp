#ifndef SOLANA_SDK_TWEETNACL
#define SOLANA_SDK_TWEETNACL

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

    static void M(PackedInt64Array &o, const PackedInt64Array &a, const PackedInt64Array &b);
    static void S(PackedInt64Array &o, const PackedInt64Array &a);
    static PackedByteArray sigma();
    static PackedInt64Array minusp();
    static void inv25519(PackedInt64Array &o, const PackedInt64Array &i);
    static void sel25519(PackedInt64Array &p, PackedInt64Array &q, int64_t b);
    static void car25519(PackedInt64Array &o);

    static PackedInt64Array gf(const PackedInt64Array &list);

    static int64_t signed_shift_left(const int64_t num, const int32_t bits);
    static int64_t signed_shift_right(const int64_t num, const int64_t bits);

    static bool checkLengths(const PackedByteArray &k, const PackedByteArray &n);
    static bool checkBoxLengths(const PackedByteArray &pk, const PackedByteArray &sk);

    static void st32(PackedByteArray &x, const int64_t j, int64_t u);
    static int64_t L32(const int64_t x, const int64_t c, const bool is_32 = false);
    static void pack25519(PackedByteArray &o, const PackedInt64Array &n);

    static void crypto_stream_xor(PackedByteArray &c, const int64_t cpos, const PackedByteArray &m, const int64_t mpos, const int64_t d, const PackedByteArray &n, const PackedByteArray &k);

    static void core(PackedByteArray &out, const PackedByteArray inp, const PackedByteArray &k, const PackedByteArray &c, const bool h);

    static int64_t crypto_onetimeauth_verify(const PackedByteArray &h, const int64_t hpos, const PackedByteArray &m, const int64_t mpos, const int64_t n, const PackedByteArray &k);
    static void crypto_scalarmult(PackedByteArray &q, const PackedByteArray &n, const PackedByteArray &p);

    static void crypto_stream_salsa20(PackedByteArray &c, const int64_t cpos, const int64_t d,const PackedByteArray &n, const PackedByteArray &k);
    static void crypto_box_keypair(PackedByteArray& y, PackedByteArray &x);
    static void crypto_scalarmult_base(PackedByteArray &q, const PackedByteArray &n);
    static void crypto_box_beforenm(PackedByteArray &k, const PackedByteArray &y, const PackedByteArray &x);

    static void crypto_stream(PackedByteArray &c, const int64_t cpos, const int64_t d, const PackedByteArray &n, const PackedByteArray &k);
    static void crypto_core_salsa20(PackedByteArray &out, const PackedByteArray &inp, const PackedByteArray &k, const PackedByteArray &c);
    static void crypto_core_hsalsa20(PackedByteArray &out, const PackedByteArray &inp, const PackedByteArray &k, const PackedByteArray &c);
    static void crypto_stream_salsa20_xor(PackedByteArray &c, int64_t cpos, const PackedByteArray &m, int64_t mpos, int64_t b, const PackedByteArray &n, const PackedByteArray &k);

    static PackedByteArray secretbox(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &key);

    static bool crypto_secretbox(PackedByteArray &c, const PackedByteArray &m, int64_t d, const PackedByteArray &n, const PackedByteArray &k);
    static void crypto_onetimeauth(PackedByteArray &out, int64_t outpos, const PackedByteArray &m, int64_t mpos, int64_t n, const PackedByteArray &k);
    static bool crypto_secretbox_open(PackedByteArray &m, const PackedByteArray &c, int64_t d, const PackedByteArray &n, const PackedByteArray &k);

    static PackedByteArray box_before(const PackedByteArray &publicKey, const PackedByteArray &secretKey);
    static PackedByteArray secretbox_open(const PackedByteArray &box, const PackedByteArray &nonce, const PackedByteArray &key);
protected:
    static void _bind_methods();
public:
    static Array box_keypair();
    static PackedByteArray box(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &publicKey, const PackedByteArray &secretKey);
    static PackedByteArray box_open(const PackedByteArray &msg, const PackedByteArray &nonce, const PackedByteArray &publicKey, const PackedByteArray &secretKey);
};
}

#endif