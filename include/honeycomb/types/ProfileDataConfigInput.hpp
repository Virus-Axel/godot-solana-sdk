#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIGINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ProfileDataConfigInput : public Resource{
GDCLASS(ProfileDataConfigInput, Resource)
private:
PackedStringArray achievements;
PackedStringArray customDataFields;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_achievements(const PackedStringArray& val);
PackedStringArray get_achievements();
void set_customDataFields(const PackedStringArray& val);
PackedStringArray get_customDataFields();
};
} // honeycomb_resource
} // godot
#endif