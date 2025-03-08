#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSHPLFUNGIBLE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSHPLFUNGIBLE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ResourceKindParamsHplFungible : public Resource {
	GDCLASS(ResourceKindParamsHplFungible, Resource)
private:
	int32_t decimals;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_decimals(const int32_t &val);
	int32_t get_decimals() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
