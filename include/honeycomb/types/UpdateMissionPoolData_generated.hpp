#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONPOOLDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONPOOLDATA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UpdateMissionPoolData.
 */
class UpdateMissionPoolData : public Resource {
	GDCLASS(UpdateMissionPoolData, Resource)

private:
	String project;
	String missionPool;
	String characterModel;
	String authority;
	String delegateAuthority;
	String payer;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UpdateMissionPoolData.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const String &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	String get_project() const;

	/**
	 * @brief Set the missionPool property.
	 * @param val New value for missionPool.
	 */
	void set_missionPool(const String &val);
	/**
	 * @brief Get the missionPool property.
	 * @return Current value of missionPool.
	 */
	String get_missionPool() const;

	/**
	 * @brief Set the characterModel property.
	 * @param val New value for characterModel.
	 */
	void set_characterModel(const String &val);
	/**
	 * @brief Get the characterModel property.
	 * @return Current value of characterModel.
	 */
	String get_characterModel() const;

	/**
	 * @brief Set the authority property.
	 * @param val New value for authority.
	 */
	void set_authority(const String &val);
	/**
	 * @brief Get the authority property.
	 * @return Current value of authority.
	 */
	String get_authority() const;

	/**
	 * @brief Set the delegateAuthority property.
	 * @param val New value for delegateAuthority.
	 */
	void set_delegateAuthority(const String &val);
	/**
	 * @brief Get the delegateAuthority property.
	 * @return Current value of delegateAuthority.
	 */
	String get_delegateAuthority() const;

	/**
	 * @brief Set the payer property.
	 * @param val New value for payer.
	 */
	void set_payer(const String &val);
	/**
	 * @brief Get the payer property.
	 * @return Current value of payer.
	 */
	String get_payer() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONPOOLDATA
