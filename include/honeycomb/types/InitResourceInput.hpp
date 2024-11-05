#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot{
namespace honeycomb_resource{

class InitResourceInput : public Resource{
GDCLASS(InitResourceInput, Resource)
private:
String name;
String symbol;
String uri;
int32_t decimals;
int32_t storage;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_name(const String& val);
String& get_name();
void set_symbol(const String& val);
String& get_symbol();
void set_uri(const String& val);
String& get_uri();
void set_decimals(const int32_t& val);
int32_t& get_decimals();
void set_storage(const int32_t& val);
int32_t& get_storage();
};
} // honeycomb_resource
} // godot
#endif