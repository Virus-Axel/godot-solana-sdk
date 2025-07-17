#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONDATA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UpdateMissionData.
 */
class UpdateMissionData : public Resource {
	GDCLASS(UpdateMissionData, Resource)

private:
	String mission;
	String authority;
	String delegateAuthority;
	String payer;
	String name;
	int64_t minXp;
	Variant cost;
	int64_t duration;
	bool removeAllRewards;
	Array addRewards;
	PackedInt32Array removeRewardIndices;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UpdateMissionData.
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

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

	/**
	 * @brief Set the minXp property.
	 * @param val New value for minXp.
	 */
	void set_minXp(const int64_t &val);
	/**
	 * @brief Get the minXp property.
	 * @return Current value of minXp.
	 */
	int64_t get_minXp() const;

	/**
	 * @brief Set the cost property.
	 * @param val New value for cost.
	 */
	void set_cost(const Variant &val);
	/**
	 * @brief Get the cost property.
	 * @return Current value of cost.
	 */
	Variant get_cost() const;

	/**
	 * @brief Set the duration property.
	 * @param val New value for duration.
	 */
	void set_duration(const int64_t &val);
	/**
	 * @brief Get the duration property.
	 * @return Current value of duration.
	 */
	int64_t get_duration() const;

	/**
	 * @brief Set the removeAllRewards property.
	 * @param val New value for removeAllRewards.
	 */
	void set_removeAllRewards(const bool &val);
	/**
	 * @brief Get the removeAllRewards property.
	 * @return Current value of removeAllRewards.
	 */
	bool get_removeAllRewards() const;

	/**
	 * @brief Set the addRewards property.
	 * @param val New value for addRewards.
	 */
	void set_addRewards(const Array &val);
	/**
	 * @brief Get the addRewards property.
	 * @return Current value of addRewards.
	 */
	Array get_addRewards() const;

	/**
	 * @brief Set the removeRewardIndices property.
	 * @param val New value for removeRewardIndices.
	 */
	void set_removeRewardIndices(const PackedInt32Array &val);
	/**
	 * @brief Get the removeRewardIndices property.
	 * @return Current value of removeRewardIndices.
	 */
	PackedInt32Array get_removeRewardIndices() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONDATA
