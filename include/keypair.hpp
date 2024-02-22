#ifndef SOLANA_SDK_KEYPAIR_HPP
#define SOLANA_SDK_KEYPAIR_HPP


#include <godot_cpp/classes/resource.hpp>
#include "utils.hpp"
#include "ed25519.h"
#include "../ed25519/src/sha512.h"
#include "../ed25519/src/ge.h"

namespace godot{
class Keypair : public Resource {
    GDCLASS(Keypair, Resource)

private:
    //Ref<CryptoPP::ed25519::Signer> signer;
    //Ref<CryptoPP::ed25519::Verifier> verifier;
    //GDExtensionObjectPtr signer;
    //GDExtensionObjectPtr verifier;


    const int KEY_LENGTH = 32;
    const int SIGNATURE_LENGTH = 64;

    bool unique = true;
    String public_value = "";
    PackedByteArray public_bytes;

    String private_value = "";
    PackedByteArray private_bytes;

    PackedByteArray seed;

    void from_private_bytes(const PackedByteArray &byte_array);

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Keypair();
    Keypair(const PackedByteArray &seed);
    
    static Variant new_from_seed(const String &seed);
    static Variant new_from_seed(const PackedByteArray &seed);
    static Variant new_from_bytes(const PackedByteArray &bytes);
    static Variant new_from_file(const String &filename);
    static Variant new_from_private_key(const Variant &private_key);
    static Variant new_random();
    static bool is_keypair(const Variant& object);

    void set_public_value(const String& p_value);
    String get_public_value();

    void set_public_bytes(const PackedByteArray& p_value);
    PackedByteArray get_public_bytes() const;

    void set_private_value(const String& p_value);
    String get_private_value();

    void set_private_bytes(const PackedByteArray& p_value);
    PackedByteArray get_private_bytes();

    PackedByteArray sign_message(const PackedByteArray& message);
    bool verify_signature(const PackedByteArray& signature, const PackedByteArray& message);

    void set_unique(const bool p_value);
    bool get_unique();

    void set_seed(const PackedByteArray &p_value);
    PackedByteArray get_seed();

    void random();
    void from_seed();

    void save_to_file(const String &filename);

    ~Keypair();
};
}

#endif