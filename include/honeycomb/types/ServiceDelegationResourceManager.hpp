#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONRESOURCEMANAGER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONRESOURCEMANAGER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums.hpp"

namespace godot{
namespace honeycomb_resource{

class ServiceDelegationResourceManager : public Resource{
GDCLASS(ServiceDelegationResourceManager, Resource)
private:
String permission;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_permission(const String& val);
String get_permission() const;
};
} // honeycomb_resource
} // godot
#endif
