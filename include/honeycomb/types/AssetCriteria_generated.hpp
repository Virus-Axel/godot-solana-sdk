#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSETCRITERIA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSETCRITERIA
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class AssetCriteria : public Resource {
	GDCLASS(AssetCriteria, Resource)
private:
	String kind;
	Variant params;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_params(const Variant &val);
	Variant get_params() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
