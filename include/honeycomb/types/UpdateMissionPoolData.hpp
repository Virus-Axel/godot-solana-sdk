#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONPOOLDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONPOOLDATA
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class UpdateMissionPoolData : public Resource{
GDCLASS(UpdateMissionPoolData, Resource)
private:
String project;
String missionPool;
String characterModel;
String authority;
String delegateAuthority;
String payer;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_project(const String& val);
String get_project() const;
void set_missionPool(const String& val);
String get_missionPool() const;
void set_characterModel(const String& val);
String get_characterModel() const;
void set_authority(const String& val);
String get_authority() const;
void set_delegateAuthority(const String& val);
String get_delegateAuthority() const;
void set_payer(const String& val);
String get_payer() const;
};
} // honeycomb_resource
} // godot
#endif
