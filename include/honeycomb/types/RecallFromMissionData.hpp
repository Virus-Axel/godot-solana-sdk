#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECALLFROMMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECALLFROMMISSIONDATA
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class RecallFromMissionData : public Resource{
GDCLASS(RecallFromMissionData, Resource)
private:
String mission;
PackedStringArray characterAddresses;
String authority;
int32_t userId;
String payer;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_mission(const String& val);
String get_mission() const;
void set_characterAddresses(const PackedStringArray& val);
PackedStringArray get_characterAddresses() const;
void set_authority(const String& val);
String get_authority() const;
void set_userId(const int32_t& val);
int32_t get_userId() const;
void set_payer(const String& val);
String get_payer() const;
};
} // honeycomb_resource
} // godot
#endif
