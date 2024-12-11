#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITSTAKINGMULTIPLIERMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITSTAKINGMULTIPLIERMETADATAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class InitStakingMultiplierMetadataInput : public Resource{
GDCLASS(InitStakingMultiplierMetadataInput, Resource)
private:
int32_t decimals;
Array multipliers;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_decimals(const int32_t& val);
int32_t get_decimals();
void set_multipliers(const Array& val);
Array get_multipliers();
};
}
}
#endif
