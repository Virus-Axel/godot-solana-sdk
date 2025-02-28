#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONBUZZGUILD
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONBUZZGUILD
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot{
namespace honeycomb_resource{

class ServiceDelegationBuzzGuild : public Resource{
GDCLASS(ServiceDelegationBuzzGuild, Resource)
private:
int32_t index;
String permission;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_index(const int32_t& val);
int32_t get_index() const;
void set_permission(const String& val);
String get_permission() const;
};
} // honeycomb_resource
} // godot
#endif
