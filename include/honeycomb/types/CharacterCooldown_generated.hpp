#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWN
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWN
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterCooldown : public Resource{
GDCLASS(CharacterCooldown, Resource)
private:
int32_t ejection;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_ejection(const int32_t& val);
int32_t get_ejection() const;
};
} // honeycomb_resource
} // godot
#endif
