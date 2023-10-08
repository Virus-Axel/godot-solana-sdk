#ifndef SOLANA_SDK_KEYPAIR_HPP
#define SOLANA_SDK_KEYPAIR_HPP


#include <godot_cpp/classes/resource.hpp>
#include "utils.hpp"
#include "xed25519.h"

namespace godot{
class Keypair : public Resource {
    GDCLASS(Keypair, Resource)

private:
    CryptoPP::ed25519::Signer signer;
    CryptoPP::ed25519::Verifier verifier;

    const int KEY_LENGTH = 32;

    bool unique = true;
    String public_value = "";
    PackedByteArray public_bytes;

    String private_value = "";
    PackedByteArray private_bytes;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Keypair();
    
    void set_public_value(const String& p_value);
    String get_public_value();

    void set_public_bytes(const PackedByteArray& p_value);
    PackedByteArray get_public_bytes();

    void set_private_value(const String& p_value);
    String get_private_value();

    void set_private_bytes(const PackedByteArray& p_value);
    PackedByteArray get_private_bytes();

    PackedByteArray sign_message(const PackedByteArray& message);

    void set_unique(const bool p_value);
    bool get_unique();

    ~Keypair();
};
}

#endif