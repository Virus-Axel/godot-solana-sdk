#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERIALIZABLEACTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERIALIZABLEACTIONS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class SerializableActions : public Resource{
GDCLASS(SerializableActions, Resource)
private:
String kind;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_kind(const String& val);
String get_kind() const;
};
} // honeycomb_resource
} // godot
#endif
