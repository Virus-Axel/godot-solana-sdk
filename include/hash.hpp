#ifndef SOLANA_SDK_HASH_HPP
#define SOLANA_SDK_HASH_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>


namespace godot{

class Hash : public Resource {
    GDCLASS(Hash, Resource)

private:
    const int HASH_LENGTH = 32;

    bool unique = true;
    String value = "";
    PackedByteArray bytes;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Hash();

    void set_value(const String& p_value);
    String get_value();

    void set_bytes(const PackedByteArray& p_value);
    PackedByteArray get_bytes();

    void set_unique(const bool p_value);
    bool get_unique();

    ~Hash();
};
}

#endif