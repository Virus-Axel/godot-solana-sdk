#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECALLFROMMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECALLFROMMISSIONDATA
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot{
namespace honeycomb_resource{

class RecallFromMissionData : public Resource{
GDCLASS(RecallFromMissionData, Resource)
private:
String mission;
PackedStringArray characterAddresses;
String authority;
String payer;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_mission(const String& val);
String& get_mission();
void set_characterAddresses(const PackedStringArray& val);
PackedStringArray& get_characterAddresses();
void set_authority(const String& val);
String& get_authority();
void set_payer(const String& val);
String& get_payer();
};
} // honeycomb_resource
} // godot
#endif