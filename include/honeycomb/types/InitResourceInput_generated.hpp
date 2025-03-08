#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class InitResourceInput : public Resource {
	GDCLASS(InitResourceInput, Resource)
private:
	String name;
	String symbol;
	String uri;
	int32_t decimals;
	PackedStringArray tags;
	String storage;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_name(const String &val);
	String get_name() const;
	void set_symbol(const String &val);
	String get_symbol() const;
	void set_uri(const String &val);
	String get_uri() const;
	void set_decimals(const int32_t &val);
	int32_t get_decimals() const;
	void set_tags(const PackedStringArray &val);
	PackedStringArray get_tags() const;
	void set_storage(const String &val);
	String get_storage() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
