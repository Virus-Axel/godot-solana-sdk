#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type PlatformData.
 */
class PlatformData : public Resource {
	GDCLASS(PlatformData, Resource)

private:
	Array custom;
	int64_t xp;
	PackedInt32Array achievements;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this PlatformData.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the custom property.
	 * @param val New value for custom.
	 */
	void set_custom(const Array &val);
	/**
	 * @brief Get the custom property.
	 * @return Current value of custom.
	 */
	Array get_custom() const;

	/**
	 * @brief Set the xp property.
	 * @param val New value for xp.
	 */
	void set_xp(const int64_t &val);
	/**
	 * @brief Get the xp property.
	 * @return Current value of xp.
	 */
	int64_t get_xp() const;

	/**
	 * @brief Set the achievements property.
	 * @param val New value for achievements.
	 */
	void set_achievements(const PackedInt32Array &val);
	/**
	 * @brief Get the achievements property.
	 * @return Current value of achievements.
	 */
	PackedInt32Array get_achievements() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PLATFORMDATA
