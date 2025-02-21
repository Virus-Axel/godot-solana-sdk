#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSOURCEPARAMSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSOURCEPARAMSFILTER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterSourceParamsFilter : public Resource{
GDCLASS(CharacterSourceParamsFilter, Resource)
private:
Variant mint;
Variant criteria;
bool is_compressed;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_mint(const Variant& val);
Variant get_mint() const;
void set_criteria(const Variant& val);
Variant get_criteria() const;
void set_is_compressed(const bool& val);
bool get_is_compressed() const;
};
} // honeycomb_resource
} // godot
#endif
