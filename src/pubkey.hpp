#ifndef SOLANA_SDK_PUBKEY_HPP
#define SOLANA_SDK_PUBKEY_HPP

#include <godot_cpp/classes/node.hpp>
#include "../wrapper/wrapper.h"

namespace godot{
class Pubkey : public Node {
    GDCLASS(Pubkey, Node)

private:
    bool unique = true;
    String value = "";
    PackedByteArray bytes;
    void *data_pointer;

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    Pubkey();
    const void* to_ptr() const;

    ~Pubkey();
};
}

#endif