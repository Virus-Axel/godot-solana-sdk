#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterConfigInput : public Resource{
GDCLASS(CharacterConfigInput, Resource)
private:
String kind;
Array criterias;
Variant assemblerConfigInput;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_kind(const String& val);
String get_kind();
void set_criterias(const Array& val);
Array get_criterias();
void set_assemblerConfigInput(const Variant& val);
Variant get_assemblerConfigInput();
};
} // honeycomb_resource
} // godot
#endif