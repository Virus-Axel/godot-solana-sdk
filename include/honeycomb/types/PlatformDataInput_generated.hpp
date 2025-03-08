#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATAINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class PlatformDataInput : public Resource {
	GDCLASS(PlatformDataInput, Resource)
private:
	Variant custom;
	int64_t addXp;
	PackedInt32Array addAchievements;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_custom(const Variant &val);
	Variant get_custom() const;
	void set_addXp(const int64_t &val);
	int64_t get_addXp() const;
	void set_addAchievements(const PackedInt32Array &val);
	PackedInt32Array get_addAchievements() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
