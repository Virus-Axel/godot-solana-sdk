#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CUSTOMDATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CUSTOMDATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CustomDataInput.
 */
class CustomDataInput : public Resource {
	GDCLASS(CustomDataInput, Resource)

private:
	Dictionary add;
	PackedStringArray remove;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CustomDataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the add property.
	 * @param val New value for add.
	 */
	void set_add(const Dictionary &val);
	/**
	 * @brief Get the add property.
	 * @return Current value of add.
	 */
	Dictionary get_add() const;

	/**
	 * @brief Set the remove property.
	 * @param val New value for remove.
	 */
	void set_remove(const PackedStringArray &val);
	/**
	 * @brief Get the remove property.
	 * @return Current value of remove.
	 */
	PackedStringArray get_remove() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CUSTOMDATAINPUT
