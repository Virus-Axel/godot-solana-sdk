#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class AssociatedProgramInput : public Resource {
	GDCLASS(AssociatedProgramInput, Resource)
private:
	String address;
	Array trustedActions;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const String &val);
	String get_address() const;
	void set_trustedActions(const Array &val);
	Array get_trustedActions() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
