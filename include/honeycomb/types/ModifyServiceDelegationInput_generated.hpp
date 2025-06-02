#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ModifyServiceDelegationInput.
 */
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
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ModifyServiceDelegationInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the HiveControl property.
	 * @param val New value for HiveControl.
	 */
	void set_HiveControl(const Variant &val);
	/**
	 * @brief Get the HiveControl property.
	 * @return Current value of HiveControl.
	 */
	Variant get_HiveControl() const;

	/**
	 * @brief Set the CharacterManager property.
	 * @param val New value for CharacterManager.
	 */
	void set_CharacterManager(const Variant &val);
	/**
	 * @brief Get the CharacterManager property.
	 * @return Current value of CharacterManager.
	 */
	Variant get_CharacterManager() const;

	/**
	 * @brief Set the ResourceManager property.
	 * @param val New value for ResourceManager.
	 */
	void set_ResourceManager(const Variant &val);
	/**
	 * @brief Get the ResourceManager property.
	 * @return Current value of ResourceManager.
	 */
	Variant get_ResourceManager() const;

	/**
	 * @brief Set the NectarStaking property.
	 * @param val New value for NectarStaking.
	 */
	void set_NectarStaking(const Variant &val);
	/**
	 * @brief Get the NectarStaking property.
	 * @return Current value of NectarStaking.
	 */
	Variant get_NectarStaking() const;

	/**
	 * @brief Set the NectarMissions property.
	 * @param val New value for NectarMissions.
	 */
	void set_NectarMissions(const Variant &val);
	/**
	 * @brief Get the NectarMissions property.
	 * @return Current value of NectarMissions.
	 */
	Variant get_NectarMissions() const;

	/**
	 * @brief Set the BuzzGuild property.
	 * @param val New value for BuzzGuild.
	 */
	void set_BuzzGuild(const Variant &val);
	/**
	 * @brief Get the BuzzGuild property.
	 * @return Current value of BuzzGuild.
	 */
	Variant get_BuzzGuild() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MODIFYSERVICEDELEGATIONINPUT
