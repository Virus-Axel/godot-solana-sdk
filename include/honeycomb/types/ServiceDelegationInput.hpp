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
Array ResourceManager;
Array NectarStaking;
Array NectarMissions;
Array BuzzGuild;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_HiveControl(const Array& val);
Array get_HiveControl() const;
void set_AssetAssembler(const Array& val);
Array get_AssetAssembler() const;
void set_AssetManager(const Array& val);
Array get_AssetManager() const;
void set_ResourceManager(const Array& val);
Array get_ResourceManager() const;
void set_NectarStaking(const Array& val);
Array get_NectarStaking() const;
void set_NectarMissions(const Array& val);
Array get_NectarMissions() const;
void set_BuzzGuild(const Array& val);
Array get_BuzzGuild() const;
};
} // honeycomb_resource
} // godot
#endif
