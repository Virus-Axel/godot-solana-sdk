#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTICIPATEONMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTICIPATEONMISSIONDATA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ParticipateOnMissionData.
 */
class ParticipateOnMissionData : public Resource {
	GDCLASS(ParticipateOnMissionData, Resource)

private:
	String mission;
	PackedStringArray characterAddresses;
	String authority;
	int32_t userId;
	String payer;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ParticipateOnMissionData.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the mission property.
	 * @param val New value for mission.
	 */
	void set_mission(const String &val);
	/**
	 * @brief Get the mission property.
	 * @return Current value of mission.
	 */
	String get_mission() const;

	/**
	 * @brief Set the characterAddresses property.
	 * @param val New value for characterAddresses.
	 */
	void set_characterAddresses(const PackedStringArray &val);
	/**
	 * @brief Get the characterAddresses property.
	 * @return Current value of characterAddresses.
	 */
	PackedStringArray get_characterAddresses() const;

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
	 * @brief Set the userId property.
	 * @param val New value for userId.
	 */
	void set_userId(const int32_t &val);
	/**
	 * @brief Get the userId property.
	 * @return Current value of userId.
	 */
	int32_t get_userId() const;

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

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTICIPATEONMISSIONDATA
