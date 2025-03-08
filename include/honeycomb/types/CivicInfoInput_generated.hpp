#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFOINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot{
namespace honeycomb_resource{

class CivicInfoInput : public Resource{
GDCLASS(CivicInfoInput, Resource)
private:
String gatekeeperNetwork;
String wallet;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_gatekeeperNetwork(const String& val);
String get_gatekeeperNetwork() const;
void set_wallet(const String& val);
String get_wallet() const;
};
} // honeycomb_resource
} // godot
#endif
