#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGWRAPPED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGWRAPPED
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterConfigWrapped : public Resource{
GDCLASS(CharacterConfigWrapped, Resource)
private:
String kind;
Array criterias;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_kind(const String& val);
String get_kind() const;
void set_criterias(const Array& val);
Array get_criterias() const;
};
} // honeycomb_resource
} // godot
#endif
