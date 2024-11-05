#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYDELEGATIONINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot{

class ModifyDelegationInput : public Resource{
GDCLASS(ModifyDelegationInput, Resource)
private:
Variant delegation;
int32_t index;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_delegation(const Variant& val);
Variant& get_delegation();
void set_index(const int32_t& val);
int32_t& get_index();
};
} // godot
#endif