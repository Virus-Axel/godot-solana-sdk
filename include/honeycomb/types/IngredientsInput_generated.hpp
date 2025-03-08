#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INGREDIENTSINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INGREDIENTSINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class IngredientsInput : public Resource {
	GDCLASS(IngredientsInput, Resource)
private:
	String resourceAddress;
	int64_t amount;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_resourceAddress(const String &val);
	String get_resourceAddress() const;
	void set_amount(const int64_t &val);
	int64_t get_amount() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
