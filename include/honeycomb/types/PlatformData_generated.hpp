#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATA
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class PlatformData : public Resource {
	GDCLASS(PlatformData, Resource)
private:
	Array custom;
	int64_t xp;
	PackedInt32Array achievements;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_custom(const Array &val);
	Array get_custom() const;
	void set_xp(const int64_t &val);
	int64_t get_xp() const;
	void set_achievements(const PackedInt32Array &val);
	PackedInt32Array get_achievements() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
