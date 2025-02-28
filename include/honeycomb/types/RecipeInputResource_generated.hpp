#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class RecipeInputResource : public Resource{
GDCLASS(RecipeInputResource, Resource)
private:
String resource;
int32_t amount;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_resource(const String& val);
String get_resource() const;
void set_amount(const int32_t& val);
int32_t get_amount() const;
};
} // honeycomb_resource
} // godot
#endif
