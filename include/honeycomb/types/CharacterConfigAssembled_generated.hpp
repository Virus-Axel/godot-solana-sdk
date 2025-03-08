#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGASSEMBLED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGASSEMBLED
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharacterConfigAssembled : public Resource {
	GDCLASS(CharacterConfigAssembled, Resource)
private:
	String kind;
	Variant assemblerConfig;
	String name;
	String symbol;
	String description;
	Array creators;
	int32_t sellerFeeBasisPoints;
	String collectionName;
	Variant mintAs;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
	void set_assemblerConfig(const Variant &val);
	Variant get_assemblerConfig() const;
	void set_name(const String &val);
	String get_name() const;
	void set_symbol(const String &val);
	String get_symbol() const;
	void set_description(const String &val);
	String get_description() const;
	void set_creators(const Array &val);
	Array get_creators() const;
	void set_sellerFeeBasisPoints(const int32_t &val);
	int32_t get_sellerFeeBasisPoints() const;
	void set_collectionName(const String &val);
	String get_collectionName() const;
	void set_mintAs(const Variant &val);
	Variant get_mintAs() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
