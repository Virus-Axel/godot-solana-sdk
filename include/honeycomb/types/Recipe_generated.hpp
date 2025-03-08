#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Recipe : public Resource {
	GDCLASS(Recipe, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant key;
	int64_t xp;
	Array ingredients;
	Variant meal;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_project(const Variant &val);
	Variant get_project() const;
	void set_key(const Variant &val);
	Variant get_key() const;
	void set_xp(const int64_t &val);
	int64_t get_xp() const;
	void set_ingredients(const Array &val);
	Array get_ingredients() const;
	void set_meal(const Variant &val);
	Variant get_meal() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
