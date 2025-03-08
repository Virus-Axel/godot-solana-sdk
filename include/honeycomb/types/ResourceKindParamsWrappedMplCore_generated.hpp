#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDMPLCORE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEKINDPARAMSWRAPPEDMPLCORE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ResourceKindParamsWrappedMplCore : public Resource {
	GDCLASS(ResourceKindParamsWrappedMplCore, Resource)
private:
	PackedStringArray characteristics;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_characteristics(const PackedStringArray &val);
	PackedStringArray get_characteristics() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
