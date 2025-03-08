#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONHIVECONTROL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONHIVECONTROL
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceDelegationHiveControl : public Resource {
	GDCLASS(ServiceDelegationHiveControl, Resource)
private:
	String permission;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_permission(const String &val);
	String get_permission() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
