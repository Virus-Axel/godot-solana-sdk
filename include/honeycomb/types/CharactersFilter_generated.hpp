#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSFILTER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CharactersFilter : public Resource {
	GDCLASS(CharactersFilter, Resource)
private:
	Variant owner;
	Variant source;
	Variant usedBy;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_owner(const Variant &val);
	Variant get_owner() const;
	void set_source(const Variant &val);
	Variant get_source() const;
	void set_usedBy(const Variant &val);
	Variant get_usedBy() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
