#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USERINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USERINFOINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class UserInfoInput : public Resource{
GDCLASS(UserInfoInput, Resource)
private:
String name;
String bio;
String pfp;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
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
