#ifndef GODOT_SOLANA_META_DATA_COLLECTION_HPP
#define GODOT_SOLANA_META_DATA_COLLECTION_HPP

#include <godot_cpp/classes/resource.hpp>


namespace godot{

/**
 * @brief Contains token metadata collection information.
 */
class MetaDataCollection: public Resource{
    GDCLASS(MetaDataCollection, Resource)

private:
    Variant key;
    bool verified = false;

protected:
    /**
     * @bindmethods{MetaDataCollection, Resource}
     */
    static void _bind_methods();

public:

    /**
     * @setter{key, p_value}
     */
    void set_key(const Variant& p_value);

    /**
     * @getter{key, Variant}
     */
    Variant get_key();

    /**
     * @setter{verified, p_value}
     */
    void set_verified(const bool p_value);

    /**
     * @getter{verified, bool}
     */
    bool get_verified();

    /**
     * @brief Serialize MetaDataCollection.
     * 
     * @return PackedByteArray Serialized bytes.
     */
    PackedByteArray serialize() const;
};

}

#endif