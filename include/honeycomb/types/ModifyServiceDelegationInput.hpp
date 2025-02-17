#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ModifyServiceDelegationInput : public Resource{
GDCLASS(ModifyServiceDelegationInput, Resource)
private:
Variant HiveControl;
Variant AssetAssembler;
Variant AssetManager;
Variant ResourceManager;
Variant NectarStaking;
Variant NectarMissions;
Variant BuzzGuild;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_HiveControl(const Variant& val);
Variant get_HiveControl() const;
void set_AssetAssembler(const Variant& val);
Variant get_AssetAssembler() const;
void set_AssetManager(const Variant& val);
Variant get_AssetManager() const;
void set_ResourceManager(const Variant& val);
Variant get_ResourceManager() const;
void set_NectarStaking(const Variant& val);
Variant get_NectarStaking() const;
void set_NectarMissions(const Variant& val);
Variant get_NectarMissions() const;
void set_BuzzGuild(const Variant& val);
Variant get_BuzzGuild() const;
};
} // honeycomb_resource
} // godot
#endif
