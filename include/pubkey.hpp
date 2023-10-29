#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

namespace godot{
class Pubkey : public Resource {
    GDCLASS(Pubkey, Resource)

private:
    // Number of bytes in a pubkey
    const unsigned int PUBKEY_BYTES = 32;
    // maximum length of derived `Pubkey` seed
    const unsigned int MAX_SEED_LEN = 32;
    // Maximum number of seeds
    const unsigned int MAX_SEEDS = 16;
    // Maximum string length of a base58 encoded pubkey
    const unsigned int MAX_BASE58_LEN = 44;

    const unsigned char PDA_MARKER[22] = "ProgramDerivedAddress";

    String type = "UNIQUE";
    String seed = "";
    String value = "";
    PackedByteArray bytes;

    Variant base;
    Variant owner;

    Variant wallet_address;
    Variant token_mint_address;

    bool are_bytes_curve_point() const;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Pubkey();
    Pubkey(const String& from);

    void set_value(const String& p_value);
    String get_value();

    void set_seed(const String& p_value);
    String get_seed();

    void set_bytes(const PackedByteArray& p_value);
    PackedByteArray get_bytes() const;

    void set_type(const String p_value);
    String get_type();

    void set_base(const Variant p_value);
    Variant get_base();

    void set_owner(const Variant p_value);
    Variant get_owner();

    void set_wallet_address(const Variant p_value);
    Variant get_wallet_address();

    void set_token_mint_address(const Variant p_value);
    Variant get_token_mint_address();

    void create_from_string(const String& from);
    void create_from_array(const unsigned char* data);
    void create_with_seed(Variant basePubkey, String seed, Variant owner_pubkey);
    bool create_program_address(const PackedStringArray seeds, const Variant &program_id);
    bool get_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address);

    void operator=(const Variant& other);
    bool operator==(const Pubkey& other) const;

    ~Pubkey();
};
}

#endif