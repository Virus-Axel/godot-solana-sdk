#ifndef GODOT_SOLANA_META_DATA_USES_HPP
#define GODOT_SOLANA_META_DATA_USES_HPP

#include <godot_cpp/classes/resource.hpp>


namespace godot{

/**
 * @brief Contains token metadata uses information.
 */
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

    /**
     * @bindmethods{MetaDataUses, Resource}
     */
    static void _bind_methods();

public:
    /**
     * @setter{use_method, p_value}
     */
    void set_use_method(const uint32_t p_value);

    /**
     * @getter{use_method, uint32_t}
     */
    uint32_t get_use_method();

    /**
     * @setter{total, p_value}
     */
    void set_total(const uint64_t p_value);

    /**
     * @getter{total, uint64_t}
     */
    uint64_t get_total();

    /**
     * @setter{remaining, p_value}
     */
    void set_remaining(const uint64_t p_value);

    /**
     * @getter{remaining, uint64_t}
     */
    uint64_t get_remaining();

    /**
     * @serializer{MetaDataUses}
     */
    PackedByteArray serialize() const;
};

}

#endif