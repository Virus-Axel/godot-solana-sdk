#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCECUSTODYPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCECUSTODYPARAMS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ResourceCustodyParams : public Resource{
GDCLASS(ResourceCustodyParams, Resource)
private:
Variant burnerDestination;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_burnerDestination(const Variant& val);
Variant get_burnerDestination() const;
};
} // honeycomb_resource
} // godot
#endif
