#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSMISSIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSMISSIONS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ServiceParamsMissions : public Resource{
GDCLASS(ServiceParamsMissions, Resource)
private:
Variant poolId;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_poolId(const Variant& val);
Variant get_poolId() const;
};
} // honeycomb_resource
} // godot
#endif
