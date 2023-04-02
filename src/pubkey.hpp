#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

#include "solana_resource.hpp"
#include "../wrapper/wrapper.h"

namespace godot{
class Pubkey : public SolanaResource {
    GDCLASS(Pubkey, Resource)

private:
    const int PUBKEY_LENGTH = 32;

    bool unique = true;
    String value = "";
    PackedByteArray bytes;

    void _free_pointer() override;
    void _update_pointer() override;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Pubkey();

    void set_value(const String& p_value);
    String get_value();

    void set_bytes(const PackedByteArray& p_value);
    PackedByteArray get_bytes();

    void set_unique(const bool p_value);
    bool get_unique();

    ~Pubkey();
};
}

#endif