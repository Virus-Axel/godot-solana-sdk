#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDMULTIPLIERMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDMULTIPLIERMETADATAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class AddMultiplierMetadataInput : public Resource{
GDCLASS(AddMultiplierMetadataInput, Resource)
private:
int64_t value;
Variant type;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_value(const int64_t& val);
int64_t get_value() const;
void set_type(const Variant& val);
Variant get_type() const;
};
} // honeycomb_resource
} // godot
#endif
