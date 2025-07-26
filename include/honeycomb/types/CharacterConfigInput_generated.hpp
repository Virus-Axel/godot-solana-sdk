#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterConfigInput.
 */
class CharacterConfigInput : public Resource {
	GDCLASS(CharacterConfigInput, Resource)

private:
	String kind;
	Array criterias;
	Variant assemblerConfigInput;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterConfigInput.
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
	 * @brief Set the criterias property.
	 * @param val New value for criterias.
	 */
	void set_criterias(const Array &val);
	/**
	 * @brief Get the criterias property.
	 * @return Current value of criterias.
	 */
	Array get_criterias() const;

	/**
	 * @brief Set the assemblerConfigInput property.
	 * @param val New value for assemblerConfigInput.
	 */
	void set_assemblerConfigInput(const Variant &val);
	/**
	 * @brief Get the assemblerConfigInput property.
	 * @return Current value of assemblerConfigInput.
	 */
	Variant get_assemblerConfigInput() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGINPUT
