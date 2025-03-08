#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYEJECTED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYEJECTED
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class UsedByEjected : public Resource {
	GDCLASS(UsedByEjected, Resource)
private:
	Variant mint;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_mint(const Variant &val);
	Variant get_mint() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
