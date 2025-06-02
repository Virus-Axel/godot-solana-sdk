#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEOUTPUTRESOURCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEOUTPUTRESOURCE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type RecipeOutputResource.
 */
class RecipeOutputResource : public Resource {
	GDCLASS(RecipeOutputResource, Resource)

private:
	String resource;
	int32_t amount;
	Dictionary characteristics;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this RecipeOutputResource.
	 */
	Dictionary to_dict();

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

	/**
	 * @brief Set the amount property.
	 * @param val New value for amount.
	 */
	void set_amount(const int32_t &val);
	/**
	 * @brief Get the amount property.
	 * @return Current value of amount.
	 */
	int32_t get_amount() const;

	/**
	 * @brief Set the characteristics property.
	 * @param val New value for characteristics.
	 */
	void set_characteristics(const Dictionary &val);
	/**
	 * @brief Get the characteristics property.
	 * @return Current value of characteristics.
	 */
	Dictionary get_characteristics() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPEOUTPUTRESOURCE
