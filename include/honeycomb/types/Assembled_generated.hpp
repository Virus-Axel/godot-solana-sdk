#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLED
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Assembled : public Resource{
GDCLASS(Assembled, Resource)
private:
Variant hash;
Variant mint;
String uri;
Array attributes;
Variant assemblerConfig;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_hash(const Variant& val);
Variant get_hash() const;
void set_mint(const Variant& val);
Variant get_mint() const;
void set_uri(const String& val);
String get_uri() const;
void set_attributes(const Array& val);
Array get_attributes() const;
void set_assemblerConfig(const Variant& val);
Variant get_assemblerConfig() const;
};
} // honeycomb_resource
} // godot
#endif
