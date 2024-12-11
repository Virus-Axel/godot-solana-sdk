#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTICIPATEONMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTICIPATEONMISSIONDATA
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class ParticipateOnMissionData : public Resource{
GDCLASS(ParticipateOnMissionData, Resource)
private:
String mission;
PackedStringArray characterAddresses;
String authority;
String payer;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_mission(const String& val);
String get_mission();
void set_characterAddresses(const PackedStringArray& val);
PackedStringArray get_characterAddresses();
void set_authority(const String& val);
String get_authority();
void set_payer(const String& val);
String get_payer();
};
}
}
#endif
