#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONREWARD
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONREWARD

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MissionReward.
 */
class MissionReward : public Resource {
	GDCLASS(MissionReward, Resource)

private:
	String kind;
	int64_t min;
	int64_t max;
	String resource;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MissionReward.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the kind property.
	 * @param val New value for kind.
	 */
	void set_kind(const String &val);
	/**
	 * @brief Get the kind property.
	 * @return Current value of kind.
	 */
	String get_kind() const;

	/**
	 * @brief Set the min property.
	 * @param val New value for min.
	 */
	void set_min(const int64_t &val);
	/**
	 * @brief Get the min property.
	 * @return Current value of min.
	 */
	int64_t get_min() const;

	/**
	 * @brief Set the max property.
	 * @param val New value for max.
	 */
	void set_max(const int64_t &val);
	/**
	 * @brief Get the max property.
	 * @return Current value of max.
	 */
	int64_t get_max() const;

	/**
	 * @brief Set the resource property.
	 * @param val New value for resource.
	 */
	void set_resource(const String &val);
	/**
	 * @brief Get the resource property.
	 * @return Current value of resource.
	 */
	String get_resource() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONREWARD
