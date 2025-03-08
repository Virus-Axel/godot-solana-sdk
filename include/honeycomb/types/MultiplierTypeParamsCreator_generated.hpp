#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCREATOR
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCREATOR
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class MultiplierTypeParamsCreator : public Resource {
	GDCLASS(MultiplierTypeParamsCreator, Resource)
private:
	Variant creator;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_creator(const Variant &val);
	Variant get_creator() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
