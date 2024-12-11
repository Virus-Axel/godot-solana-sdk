#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONNECTARSTAKING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONNECTARSTAKING
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class ServiceDelegationNectarStaking : public Resource{
GDCLASS(ServiceDelegationNectarStaking, Resource)
private:
int32_t index;
int32_t permission;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_index(const int32_t& val);
int32_t get_index();
void set_permission(const int32_t& val);
int32_t get_permission();
};
}
}
#endif
