#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCES

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type RecipeInputResources.
 */
class RecipeInputResources : public Resource {
	GDCLASS(RecipeInputResources, Resource)

private:
	Variant inputOne;
	Variant inputTwo;
	Variant inputThree;
	Variant inputFour;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this RecipeInputResources.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the inputOne property.
	 * @param val New value for inputOne.
	 */
	void set_inputOne(const Variant &val);
	/**
	 * @brief Get the inputOne property.
	 * @return Current value of inputOne.
	 */
	Variant get_inputOne() const;

	/**
	 * @brief Set the inputTwo property.
	 * @param val New value for inputTwo.
	 */
	void set_inputTwo(const Variant &val);
	/**
	 * @brief Get the inputTwo property.
	 * @return Current value of inputTwo.
	 */
	Variant get_inputTwo() const;

	/**
	 * @brief Set the inputThree property.
	 * @param val New value for inputThree.
	 */
	void set_inputThree(const Variant &val);
	/**
	 * @brief Get the inputThree property.
	 * @return Current value of inputThree.
	 */
	Variant get_inputThree() const;

	/**
	 * @brief Set the inputFour property.
	 * @param val New value for inputFour.
	 */
	void set_inputFour(const Variant &val);
	/**
	 * @brief Get the inputFour property.
	 * @return Current value of inputFour.
	 */
	Variant get_inputFour() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEINPUTRESOURCES
