#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATION
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceDelegation : public Resource {
	GDCLASS(ServiceDelegation, Resource)
private:
	String kind;
	int32_t index;
	String permission;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_index(const int32_t &val);
	int32_t get_index() const;
	void set_permission(const String &val);
	String get_permission() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
