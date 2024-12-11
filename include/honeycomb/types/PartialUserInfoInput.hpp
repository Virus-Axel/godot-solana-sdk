#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class PartialUserInfoInput : public Resource{
GDCLASS(PartialUserInfoInput, Resource)
private:
String username;
String name;
String bio;
String pfp;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_username(const String& val);
String get_username();
void set_name(const String& val);
String get_name();
void set_bio(const String& val);
String get_bio();
void set_pfp(const String& val);
String get_pfp();
};
}
}
#endif
