#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWNINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWNINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharacterCooldownInput : public Resource {
	GDCLASS(CharacterCooldownInput, Resource)
private:
	int32_t ejection;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_ejection(const int32_t &val);
	int32_t get_ejection() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
