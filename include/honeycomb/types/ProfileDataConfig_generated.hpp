#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIG
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ProfileDataConfig : public Resource {
	GDCLASS(ProfileDataConfig, Resource)
private:
	PackedStringArray achievements;
	PackedStringArray customDataFields;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_achievements(const PackedStringArray &val);
	PackedStringArray get_achievements() const;
	void set_customDataFields(const PackedStringArray &val);
	PackedStringArray get_customDataFields() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
