#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHCONFIRMED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHCONFIRMED
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class AuthConfirmed : public Resource{
GDCLASS(AuthConfirmed, Resource)
private:
String accessToken;
Variant user;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_accessToken(const String& val);
String get_accessToken() const;
void set_user(const Variant& val);
Variant get_user() const;
};
} // honeycomb_resource
} // godot
#endif
