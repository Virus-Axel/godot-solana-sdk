#ifndef GODOT_SOLANA_META_DATA_USES_HPP
#define GODOT_SOLANA_META_DATA_USES_HPP

#include <godot_cpp/classes/resource.hpp>


namespace godot{

class MetaDataUses: public Resource{
    GDCLASS(MetaDataUses, Resource)

private:
    enum UseMethod{
        Burn = 0,
        Multiple = 1,
        Single = 2,
    };
    UseMethod use_method = UseMethod::Burn;
    uint64_t total = 0;
    uint64_t remaining = 0;

protected:
    static void _bind_methods();

public:
    void set_use_method(const uint32_t p_value);
    uint32_t get_use_method();

    void set_total(const uint64_t p_value);
    uint64_t get_total();

    void set_remaining(const uint64_t p_value);
    uint64_t get_remaining();

    PackedByteArray serialize() const;
};

}

#endif