#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ServiceDelegationInput : public Resource{
GDCLASS(ServiceDelegationInput, Resource)
private:
Array HiveControl;
Array AssetAssembler;
Array AssetManager;
Array CurrencyManager;
Array NectarStaking;
Array NectarMissions;
Array BuzzGuild;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_HiveControl(const Array& val);
Array get_HiveControl();
void set_AssetAssembler(const Array& val);
Array get_AssetAssembler();
void set_AssetManager(const Array& val);
Array get_AssetManager();
void set_CurrencyManager(const Array& val);
Array get_CurrencyManager();
void set_NectarStaking(const Array& val);
Array get_NectarStaking();
void set_NectarMissions(const Array& val);
Array get_NectarMissions();
void set_BuzzGuild(const Array& val);
Array get_BuzzGuild();
};
} // honeycomb_resource
} // godot
#endif