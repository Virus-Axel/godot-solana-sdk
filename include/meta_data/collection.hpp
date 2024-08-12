#ifndef GODOT_SOLANA_META_DATA_COLLECTION_HPP
#define GODOT_SOLANA_META_DATA_COLLECTION_HPP

#include <godot_cpp/classes/resource.hpp>


namespace godot{

class MetaDataCollection: public Resource{
    GDCLASS(MetaDataCollection, Resource)

private:
    Variant key;
    bool verified = false;

protected:
    static void _bind_methods();

public:
    void set_key(const Variant& p_value);
    Variant get_key();

    void set_verified(const bool p_value);
    bool get_verified();

    PackedByteArray serialize() const;
};

}

#endif