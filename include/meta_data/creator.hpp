#ifndef GODOT_SOLANA_META_DATA_CREATOR_HPP
#define GODOT_SOLANA_META_DATA_CREATOR_HPP

#include <godot_cpp/classes/resource.hpp>

namespace godot{

/**
 * @brief Contains token metadata creator information.
 */
class MetaDataCreator: public Resource{
    GDCLASS(MetaDataCreator, Resource)
private:
    Variant address;
    bool verified = false;
    uint16_t share = 0;

protected:

    /**
     * @bindmethods{MetaDataCreator, Resource}
     */
    static void _bind_methods();
public:
    /**
     * @setter{address, p_value}
     */
    void set_address(const Variant& p_value);

    /**
     * @getter{address, Variant}
     */
    Variant get_address();

    /**
     * @setter{verified, p_value}
     */
    void set_verified(const bool p_value);

    /**
     * @getter{verified, bool}
     */
    bool get_verified();

    /**
     * @setter{share, p_value}
     */
    void set_share(const uint32_t p_value);

    /**
     * @getter{share, uint32_t}
     */
    uint32_t get_share();

    /**
     * @brief Serialize MetaDataCreator.
     * 
     * @return PackedByteArray Serialized bytes.
     */
    PackedByteArray serialize() const;
};

}

#endif