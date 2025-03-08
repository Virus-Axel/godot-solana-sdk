#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPERESOURCES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPERESOURCES
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class RecipeResources : public Resource {
	GDCLASS(RecipeResources, Resource)
private:
	Variant input;
	Variant output;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_input(const Variant &val);
	Variant get_input() const;
	void set_output(const Variant &val);
	Variant get_output() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
