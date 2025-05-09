#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONDATA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type NewMissionData.
 */
class NewMissionData : public Resource {
	GDCLASS(NewMissionData, Resource)

private:
	String project;
	String missionPool;
	String authority;
	String delegateAuthority;
	String payer;
	String name;
	int64_t minXp;
	Variant cost;
	int64_t duration;
	Array rewards;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this NewMissionData.
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
	 * @brief Set the rewards property.
	 * @param val New value for rewards.
	 */
	void set_rewards(const Array &val);
	/**
	 * @brief Get the rewards property.
	 * @return Current value of rewards.
	 */
	Array get_rewards() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONDATA
