#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDFUNGIBLE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDFUNGIBLE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ResourceKindParamsWrappedFungible : public Resource {
	GDCLASS(ResourceKindParamsWrappedFungible, Resource)
private:
	int32_t decimals;
	Variant custody;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_decimals(const int32_t &val);
	int32_t get_decimals() const;
	void set_custody(const Variant &val);
	Variant get_custody() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
