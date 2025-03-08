#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCES
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class RecipeInputResources : public Resource {
	GDCLASS(RecipeInputResources, Resource)
private:
	Variant inputOne;
	Variant inputTwo;
	Variant inputThree;
	Variant inputFour;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_inputOne(const Variant &val);
	Variant get_inputOne() const;
	void set_inputTwo(const Variant &val);
	Variant get_inputTwo() const;
	void set_inputThree(const Variant &val);
	Variant get_inputThree() const;
	void set_inputFour(const Variant &val);
	Variant get_inputFour() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
