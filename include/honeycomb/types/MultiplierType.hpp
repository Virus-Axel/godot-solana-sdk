#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class MultiplierType : public Resource{
GDCLASS(MultiplierType, Resource)
private:
String kind;
Dictionary params;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_kind(const String& val);
String get_kind() const;
void set_params(const Dictionary& val);
Dictionary get_params() const;
};
} // honeycomb_resource
} // godot
#endif
