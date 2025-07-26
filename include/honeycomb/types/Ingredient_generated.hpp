#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INGREDIENT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INGREDIENT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Ingredient.
 */
class Ingredient : public Resource {
	GDCLASS(Ingredient, Resource)

private:
	Variant resourceAddress;
	int64_t amount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Ingredient.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the resourceAddress property.
	 * @param val New value for resourceAddress.
	 */
	void set_resourceAddress(const Variant &val);
	/**
	 * @brief Get the resourceAddress property.
	 * @return Current value of resourceAddress.
	 */
	Variant get_resourceAddress() const;

	/**
	 * @brief Set the amount property.
	 * @param val New value for amount.
	 */
	void set_amount(const int64_t &val);
	/**
	 * @brief Get the amount property.
	 * @return Current value of amount.
	 */
	int64_t get_amount() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INGREDIENT
