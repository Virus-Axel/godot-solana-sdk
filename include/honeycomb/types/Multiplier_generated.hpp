#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Multiplier : public Resource{
GDCLASS(Multiplier, Resource)
private:
int64_t value;
Variant multiplierType;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_value(const int64_t& val);
int64_t get_value() const;
void set_multiplierType(const Variant& val);
Variant get_multiplierType() const;
};
} // honeycomb_resource
} // godot
#endif
