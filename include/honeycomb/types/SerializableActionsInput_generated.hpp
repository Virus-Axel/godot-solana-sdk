#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERIALIZABLEACTIONSINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERIALIZABLEACTIONSINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class SerializableActionsInput : public Resource {
	GDCLASS(SerializableActionsInput, Resource)
private:
	String kind;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kind(const String &val);
	String get_kind() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
