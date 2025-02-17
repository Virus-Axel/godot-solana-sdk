#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Multipliers : public Resource{
GDCLASS(Multipliers, Resource)
private:
PackedByteArray address;
int32_t bump;
Variant stakingPool;
int32_t decimals;
Array durationMultipliers;
Array countMultipliers;
Array creatorMultipliers;
Array collectionMultipliers;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_bump(const int32_t& val);
int32_t get_bump() const;
void set_stakingPool(const Variant& val);
Variant get_stakingPool() const;
void set_decimals(const int32_t& val);
int32_t get_decimals() const;
void set_durationMultipliers(const Array& val);
Array get_durationMultipliers() const;
void set_countMultipliers(const Array& val);
Array get_countMultipliers() const;
void set_creatorMultipliers(const Array& val);
Array get_creatorMultipliers() const;
void set_collectionMultipliers(const Array& val);
Array get_collectionMultipliers() const;
};
} // honeycomb_resource
} // godot
#endif
