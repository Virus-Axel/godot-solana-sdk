#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSION

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Mission.
 */
class Mission : public Resource {
	GDCLASS(Mission, Resource)

private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	int32_t bump;
	Variant project;
	Variant missionPool;
	String name;
	int64_t minXp;
	Variant cost;
	Dictionary requirement;
	Array rewards;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Mission.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the program_id property.
	 * @param val New value for program_id.
	 */
	void set_program_id(const PackedByteArray &val);
	/**
	 * @brief Get the program_id property.
	 * @return Current value of program_id.
	 */
	PackedByteArray get_program_id() const;

	/**
	 * @brief Set the discriminator property.
	 * @param val New value for discriminator.
	 */
	void set_discriminator(const PackedByteArray &val);
	/**
	 * @brief Get the discriminator property.
	 * @return Current value of discriminator.
	 */
	PackedByteArray get_discriminator() const;

	/**
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the missionPool property.
	 * @param val New value for missionPool.
	 */
	void set_missionPool(const Variant &val);
	/**
	 * @brief Get the missionPool property.
	 * @return Current value of missionPool.
	 */
	Variant get_missionPool() const;

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
	 * @brief Set the requirement property.
	 * @param val New value for requirement.
	 */
	void set_requirement(const Dictionary &val);
	/**
	 * @brief Get the requirement property.
	 * @return Current value of requirement.
	 */
	Dictionary get_requirement() const;

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

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSION
