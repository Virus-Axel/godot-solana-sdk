#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_GLOBAL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_GLOBAL
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Global : public Resource {
	GDCLASS(Global, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Array config;
	Variant userTrees;
	int32_t totalUsers;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_config(const Array &val);
	Array get_config() const;
	void set_userTrees(const Variant &val);
	Variant get_userTrees() const;
	void set_totalUsers(const int32_t &val);
	int32_t get_totalUsers() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
