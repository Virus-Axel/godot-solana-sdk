#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot{
namespace honeycomb_resource{

class ImportResourceInput : public Resource{
GDCLASS(ImportResourceInput, Resource)
private:
int32_t decimals;
String project;
String mint;
String authority;
String storage;
PackedStringArray tags;
Variant custody;
String payer;
String delegateAuthority;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_decimals(const int32_t& val);
int32_t get_decimals() const;
void set_project(const String& val);
String get_project() const;
void set_mint(const String& val);
String get_mint() const;
void set_authority(const String& val);
String get_authority() const;
void set_storage(const String& val);
String get_storage() const;
void set_tags(const PackedStringArray& val);
PackedStringArray get_tags() const;
void set_custody(const Variant& val);
Variant get_custody() const;
void set_payer(const String& val);
String get_payer() const;
void set_delegateAuthority(const String& val);
String get_delegateAuthority() const;
};
} // honeycomb_resource
} // godot
#endif
