#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WRAPPED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WRAPPED
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums.hpp"

namespace godot{
namespace honeycomb_resource{

class Wrapped : public Resource{
GDCLASS(Wrapped, Resource)
private:
Variant mint;
Variant criteria;
String kind;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_mint(const Variant& val);
Variant get_mint() const;
void set_criteria(const Variant& val);
Variant get_criteria() const;
void set_kind(const String& val);
String get_kind() const;
};
} // honeycomb_resource
} // godot
#endif
