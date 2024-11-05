#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIGINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class AssemblerConfigInput : public Resource{
GDCLASS(AssemblerConfigInput, Resource)
private:
String assemblerConfig;
String name;
String symbol;
String description;
Array creators;
int32_t sellerFeeBasisPoints;
String collectionName;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_assemblerConfig(const String& val);
String get_assemblerConfig();
void set_name(const String& val);
String get_name();
void set_symbol(const String& val);
String get_symbol();
void set_description(const String& val);
String get_description();
void set_creators(const Array& val);
Array get_creators();
void set_sellerFeeBasisPoints(const int32_t& val);
int32_t get_sellerFeeBasisPoints();
void set_collectionName(const String& val);
String get_collectionName();
};
} // honeycomb_resource
} // godot
#endif