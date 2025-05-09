#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AssociatedProgramInput.
 */
class AssociatedProgramInput : public Resource {
	GDCLASS(AssociatedProgramInput, Resource)

private:
	String address;
	Array trustedActions;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AssociatedProgramInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const String &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	String get_address() const;

	/**
	 * @brief Set the trustedActions property.
	 * @param val New value for trustedActions.
	 */
	void set_trustedActions(const Array &val);
	/**
	 * @brief Get the trustedActions property.
	 * @return Current value of trustedActions.
	 */
	Array get_trustedActions() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSOCIATEDPROGRAMINPUT
