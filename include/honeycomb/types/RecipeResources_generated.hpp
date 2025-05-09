#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPERESOURCES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPERESOURCES

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type RecipeResources.
 */
class RecipeResources : public Resource {
	GDCLASS(RecipeResources, Resource)

private:
	Variant input;
	Variant output;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this RecipeResources.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the input property.
	 * @param val New value for input.
	 */
	void set_input(const Variant &val);
	/**
	 * @brief Get the input property.
	 * @return Current value of input.
	 */
	Variant get_input() const;

	/**
	 * @brief Set the output property.
	 * @param val New value for output.
	 */
	void set_output(const Variant &val);
	/**
	 * @brief Get the output property.
	 * @return Current value of output.
	 */
	Variant get_output() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPERESOURCES
