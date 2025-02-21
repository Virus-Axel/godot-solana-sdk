#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAM
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAM
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class AssociatedProgram : public Resource{
GDCLASS(AssociatedProgram, Resource)
private:
Variant address;
Array trustedActions;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const Variant& val);
Variant get_address() const;
void set_trustedActions(const Array& val);
Array get_trustedActions() const;
};
} // honeycomb_resource
} // godot
#endif
