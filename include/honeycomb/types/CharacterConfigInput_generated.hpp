#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharacterConfigInput : public Resource {
	GDCLASS(CharacterConfigInput, Resource)
private:
	String kind;
	Array criterias;
	Variant assemblerConfigInput;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_criterias(const Array &val);
	Array get_criterias() const;
	void set_assemblerConfigInput(const Variant &val);
	Variant get_assemblerConfigInput() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
