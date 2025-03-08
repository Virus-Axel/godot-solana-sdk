#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAITINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAITINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharacterTraitInput : public Resource {
	GDCLASS(CharacterTraitInput, Resource)
private:
	String layer;
	String name;
	String uri;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_layer(const String &val);
	String get_layer() const;
	void set_name(const String &val);
	String get_name() const;
	void set_uri(const String &val);
	String get_uri() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
