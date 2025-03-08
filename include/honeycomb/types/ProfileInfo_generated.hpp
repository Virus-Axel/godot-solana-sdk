#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEINFO
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ProfileInfo : public Resource{
GDCLASS(ProfileInfo, Resource)
private:
String name;
String bio;
String pfp;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
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
