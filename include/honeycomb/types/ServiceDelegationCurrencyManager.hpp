#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONCURRENCYMANAGER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONCURRENCYMANAGER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ServiceDelegationCurrencyManager : public Resource{
GDCLASS(ServiceDelegationCurrencyManager, Resource)
private:
int32_t permission;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_permission(const int32_t& val);
int32_t get_permission();
};
} // honeycomb_resource
} // godot
#endif