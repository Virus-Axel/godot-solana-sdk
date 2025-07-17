#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWNINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWNINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterCooldownInput.
 */
class CharacterCooldownInput : public Resource {
	GDCLASS(CharacterCooldownInput, Resource)

private:
	int32_t ejection;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterCooldownInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the ejection property.
	 * @param val New value for ejection.
	 */
	void set_ejection(const int32_t &val);
	/**
	 * @brief Get the ejection property.
	 * @return Current value of ejection.
	 */
	int32_t get_ejection() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCOOLDOWNINPUT
