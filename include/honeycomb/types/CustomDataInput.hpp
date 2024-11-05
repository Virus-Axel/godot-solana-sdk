#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CUSTOMDATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CUSTOMDATAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot{
namespace honeycomb_resource{

class CustomDataInput : public Resource{
GDCLASS(CustomDataInput, Resource)
private:
Dictionary add;
PackedStringArray remove;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_add(const Dictionary& val);
Dictionary& get_add();
void set_remove(const PackedStringArray& val);
PackedStringArray& get_remove();
};
} // honeycomb_resource
} // godot
#endif