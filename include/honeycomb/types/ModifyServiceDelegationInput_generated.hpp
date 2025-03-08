#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ModifyServiceDelegationInput : public Resource {
	GDCLASS(ModifyServiceDelegationInput, Resource)
private:
	Variant HiveControl;
	Variant CharacterManager;
	Variant ResourceManager;
	Variant NectarStaking;
	Variant NectarMissions;
	Variant BuzzGuild;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_HiveControl(const Variant &val);
	Variant get_HiveControl() const;
	void set_CharacterManager(const Variant &val);
	Variant get_CharacterManager() const;
	void set_ResourceManager(const Variant &val);
	Variant get_ResourceManager() const;
	void set_NectarStaking(const Variant &val);
	Variant get_NectarStaking() const;
	void set_NectarMissions(const Variant &val);
	Variant get_NectarMissions() const;
	void set_BuzzGuild(const Variant &val);
	Variant get_BuzzGuild() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
