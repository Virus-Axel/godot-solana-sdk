#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MEAL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MEAL
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Meal : public Resource{
GDCLASS(Meal, Resource)
private:
Variant resourceAddress;
int64_t amount;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_resourceAddress(const Variant& val);
Variant get_resourceAddress() const;
void set_amount(const int64_t& val);
int64_t get_amount() const;
};
} // honeycomb_resource
} // godot
#endif
