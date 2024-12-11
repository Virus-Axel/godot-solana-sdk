#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class AssociatedProgramInput : public Resource{
GDCLASS(AssociatedProgramInput, Resource)
private:
String address;
Array trustedActions;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_address(const String& val);
String get_address();
void set_trustedActions(const Array& val);
Array get_trustedActions();
};
}
}
#endif
