#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIGINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ProfileDataConfigInput.
 */
class ProfileDataConfigInput : public Resource {
	GDCLASS(ProfileDataConfigInput, Resource)

private:
	PackedStringArray achievements;
	PackedStringArray customDataFields;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ProfileDataConfigInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the achievements property.
	 * @param val New value for achievements.
	 */
	void set_achievements(const PackedStringArray &val);
	/**
	 * @brief Get the achievements property.
	 * @return Current value of achievements.
	 */
	PackedStringArray get_achievements() const;

	/**
	 * @brief Set the customDataFields property.
	 * @param val New value for customDataFields.
	 */
	void set_customDataFields(const PackedStringArray &val);
	/**
	 * @brief Get the customDataFields property.
	 * @return Current value of customDataFields.
	 */
	PackedStringArray get_customDataFields() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILEDATACONFIGINPUT
