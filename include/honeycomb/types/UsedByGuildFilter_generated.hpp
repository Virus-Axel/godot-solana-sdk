#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYGUILDFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYGUILDFILTER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class UsedByGuildFilter : public Resource {
	GDCLASS(UsedByGuildFilter, Resource)
private:
	Variant id;
	Variant role;
	int32_t order;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_id(const Variant &val);
	Variant get_id() const;
	void set_role(const Variant &val);
	Variant get_role() const;
	void set_order(const int32_t &val);
	int32_t get_order() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
