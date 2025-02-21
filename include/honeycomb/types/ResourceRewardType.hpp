#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEREWARDTYPE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ResourceRewardType : public Resource{
GDCLASS(ResourceRewardType, Resource)
private:
String kind;
Variant params;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_kind(const String& val);
String get_kind() const;
void set_params(const Variant& val);
Variant get_params() const;
};
} // honeycomb_resource
} // godot
#endif
