#ifndef GODOT_SOLANA_META_DATA_CREATOR_HPP
#define GODOT_SOLANA_META_DATA_CREATOR_HPP

#include <godot_cpp/classes/resource.hpp>

namespace godot{

class MetaDataCreator: public Resource{
    GDCLASS(MetaDataCreator, Resource)
private:
    Variant address;
    bool verified = false;
    uint16_t share = 0;

protected:
    static void _bind_methods();
public:
    void set_address(const Variant& p_value);
    Variant get_address();

    void set_verified(const bool p_value);
    bool get_verified();

    void set_share(const uint32_t p_value);
    uint32_t get_share();

    PackedByteArray serialize() const;
};

}

#endif