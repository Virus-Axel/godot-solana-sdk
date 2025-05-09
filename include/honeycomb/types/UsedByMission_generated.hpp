#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYMISSION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYMISSION

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UsedByMission.
 */
class UsedByMission : public Resource {
	GDCLASS(UsedByMission, Resource)

private:
	Variant missionId;
	String participationId;
	Array rewards;
	int32_t endTime;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UsedByMission.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the missionId property.
	 * @param val New value for missionId.
	 */
	void set_missionId(const Variant &val);
	/**
	 * @brief Get the missionId property.
	 * @return Current value of missionId.
	 */
	Variant get_missionId() const;

	/**
	 * @brief Set the participationId property.
	 * @param val New value for participationId.
	 */
	void set_participationId(const String &val);
	/**
	 * @brief Get the participationId property.
	 * @return Current value of participationId.
	 */
	String get_participationId() const;

	/**
	 * @brief Set the rewards property.
	 * @param val New value for rewards.
	 */
	void set_rewards(const Array &val);
	/**
	 * @brief Get the rewards property.
	 * @return Current value of rewards.
	 */
	Array get_rewards() const;

	/**
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int32_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int32_t get_endTime() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYMISSION
