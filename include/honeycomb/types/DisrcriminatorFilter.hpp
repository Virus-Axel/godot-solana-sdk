#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DISRCRIMINATORFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DISRCRIMINATORFILTER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class DisrcriminatorFilter : public Resource{
GDCLASS(DisrcriminatorFilter, Resource)
private:
String programId;
String accountName;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_programId(const String& val);
String get_programId() const;
void set_accountName(const String& val);
String get_accountName() const;
};
} // honeycomb_resource
} // godot
#endif
