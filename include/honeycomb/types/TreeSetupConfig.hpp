#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREESETUPCONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREESETUPCONFIG
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class TreeSetupConfig : public Resource{
GDCLASS(TreeSetupConfig, Resource)
private:
Variant basic;
Variant advanced;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_basic(const Variant& val);
Variant get_basic();
void set_advanced(const Variant& val);
Variant get_advanced();
};
} // honeycomb_resource
} // godot
#endif