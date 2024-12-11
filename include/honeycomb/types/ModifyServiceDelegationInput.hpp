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
Variant CurrencyManager;
Variant NectarStaking;
Variant NectarMissions;
Variant BuzzGuild;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_HiveControl(const Variant& val);
Variant get_HiveControl();
void set_AssetAssembler(const Variant& val);
Variant get_AssetAssembler();
void set_AssetManager(const Variant& val);
Variant get_AssetManager();
void set_CurrencyManager(const Variant& val);
Variant get_CurrencyManager();
void set_NectarStaking(const Variant& val);
Variant get_NectarStaking();
void set_NectarMissions(const Variant& val);
Variant get_NectarMissions();
void set_BuzzGuild(const Variant& val);
Variant get_BuzzGuild();
};
}
}
#endif
