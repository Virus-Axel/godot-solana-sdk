#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEBALANCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEBALANCE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

class ResourceBalance : public Resource {
	GDCLASS(ResourceBalance, Resource)
private:
	PackedByteArray address;
	String amount;
	Variant mint;
	String wallet;
	String resourceStorage;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_amount(const String &val);
	String get_amount() const;
	void set_mint(const Variant &val);
	Variant get_mint() const;
	void set_wallet(const String &val);
	String get_wallet() const;
	void set_resourceStorage(const String &val);
	String get_resourceStorage() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
