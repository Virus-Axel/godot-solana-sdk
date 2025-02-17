#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFO
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums.hpp"

namespace godot{
namespace honeycomb_resource{

class CivicInfo : public Resource{
GDCLASS(CivicInfo, Resource)
private:
String gatekeeperNetwork;
String expiry;
int32_t walletIndex;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_gatekeeperNetwork(const String& val);
String get_gatekeeperNetwork() const;
void set_expiry(const String& val);
String get_expiry() const;
void set_walletIndex(const int32_t& val);
int32_t get_walletIndex() const;
};
} // honeycomb_resource
} // godot
#endif
