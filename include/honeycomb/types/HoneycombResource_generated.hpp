#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBRESOURCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBRESOURCE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class HoneycombResource : public Resource {
	GDCLASS(HoneycombResource, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant mint;
	Variant storage;
	Variant kind;
	PackedStringArray tags;

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
	void set_mint(const Variant &val);
	Variant get_mint() const;
	void set_storage(const Variant &val);
	Variant get_storage() const;
	void set_kind(const Variant &val);
	Variant get_kind() const;
	void set_tags(const PackedStringArray &val);
	PackedStringArray get_tags() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
