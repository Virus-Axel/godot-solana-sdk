#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceDelegationInput.
 */
class ServiceDelegationInput : public Resource {
	GDCLASS(ServiceDelegationInput, Resource)

private:
	Array HiveControl;
	Array CharacterManager;
	Array ResourceManager;
	Array NectarStaking;
	Array NectarMissions;
	Array BuzzGuild;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceDelegationInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the HiveControl property.
	 * @param val New value for HiveControl.
	 */
	void set_HiveControl(const Array &val);
	/**
	 * @brief Get the HiveControl property.
	 * @return Current value of HiveControl.
	 */
	Array get_HiveControl() const;

	/**
	 * @brief Set the CharacterManager property.
	 * @param val New value for CharacterManager.
	 */
	void set_CharacterManager(const Array &val);
	/**
	 * @brief Get the CharacterManager property.
	 * @return Current value of CharacterManager.
	 */
	Array get_CharacterManager() const;

	/**
	 * @brief Set the ResourceManager property.
	 * @param val New value for ResourceManager.
	 */
	void set_ResourceManager(const Array &val);
	/**
	 * @brief Get the ResourceManager property.
	 * @return Current value of ResourceManager.
	 */
	Array get_ResourceManager() const;

	/**
	 * @brief Set the NectarStaking property.
	 * @param val New value for NectarStaking.
	 */
	void set_NectarStaking(const Array &val);
	/**
	 * @brief Get the NectarStaking property.
	 * @return Current value of NectarStaking.
	 */
	Array get_NectarStaking() const;

	/**
	 * @brief Set the NectarMissions property.
	 * @param val New value for NectarMissions.
	 */
	void set_NectarMissions(const Array &val);
	/**
	 * @brief Get the NectarMissions property.
	 * @return Current value of NectarMissions.
	 */
	Array get_NectarMissions() const;

	/**
	 * @brief Set the BuzzGuild property.
	 * @param val New value for BuzzGuild.
	 */
	void set_BuzzGuild(const Array &val);
	/**
	 * @brief Get the BuzzGuild property.
	 * @return Current value of BuzzGuild.
	 */
	Array get_BuzzGuild() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEDELEGATIONINPUT
