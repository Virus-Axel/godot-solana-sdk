#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYCUSTOM
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYCUSTOM

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UsedByCustom.
 */
class UsedByCustom : public Resource {
	GDCLASS(UsedByCustom, Resource)

private:
	Variant user;
	Dictionary data;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UsedByCustom.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the user property.
	 * @param val New value for user.
	 */
	void set_user(const Variant &val);
	/**
	 * @brief Get the user property.
	 * @return Current value of user.
	 */
	Variant get_user() const;

	/**
	 * @brief Set the data property.
	 * @param val New value for data.
	 */
	void set_data(const Dictionary &val);
	/**
	 * @brief Get the data property.
	 * @return Current value of data.
	 */
	Dictionary get_data() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYCUSTOM
