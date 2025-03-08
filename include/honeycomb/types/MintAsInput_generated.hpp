#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class MintAsInput : public Resource {
	GDCLASS(MintAsInput, Resource)
private:
	String kind;
	Variant mplBubblegum;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_mplBubblegum(const Variant &val);
	Variant get_mplBubblegum() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
