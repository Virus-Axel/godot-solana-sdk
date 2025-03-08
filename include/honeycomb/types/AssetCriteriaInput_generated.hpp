#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSETCRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSETCRITERIAINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class AssetCriteriaInput : public Resource {
	GDCLASS(AssetCriteriaInput, Resource)
private:
	String kind;
	PackedByteArray params;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_params(const PackedByteArray &val);
	PackedByteArray get_params() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
