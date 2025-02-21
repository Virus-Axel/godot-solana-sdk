#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBUSERINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBUSERINFO
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class HoneycombUserInfo : public Resource{
GDCLASS(HoneycombUserInfo, Resource)
private:
String username;
String name;
String bio;
String pfp;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_username(const String& val);
String get_username() const;
void set_name(const String& val);
String get_name() const;
void set_bio(const String& val);
String get_bio() const;
void set_pfp(const String& val);
String get_pfp() const;
};
} // honeycomb_resource
} // godot
#endif
