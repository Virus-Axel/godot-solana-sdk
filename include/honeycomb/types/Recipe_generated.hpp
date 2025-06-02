#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Recipe.
 */
class Recipe : public Resource {
	GDCLASS(Recipe, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant key;
	int64_t xp;
	Array ingredients;
	Variant meal;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Recipe.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the key property.
	 * @param val New value for key.
	 */
	void set_key(const Variant &val);
	/**
	 * @brief Get the key property.
	 * @return Current value of key.
	 */
	Variant get_key() const;

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
	 * @brief Set the ingredients property.
	 * @param val New value for ingredients.
	 */
	void set_ingredients(const Array &val);
	/**
	 * @brief Get the ingredients property.
	 * @return Current value of ingredients.
	 */
	Array get_ingredients() const;

	/**
	 * @brief Set the meal property.
	 * @param val New value for meal.
	 */
	void set_meal(const Variant &val);
	/**
	 * @brief Get the meal property.
	 * @return Current value of meal.
	 */
	Variant get_meal() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RECIPE
