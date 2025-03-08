#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPEPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPEPARAMS
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ResourceRewardTypeParams : public Resource {
	GDCLASS(ResourceRewardTypeParams, Resource)
private:
	Variant address;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const Variant &val);
	Variant get_address() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
