#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDMULTIPLIERMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDMULTIPLIERMETADATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AddMultiplierMetadataInput.
 */
class AddMultiplierMetadataInput : public Resource {
	GDCLASS(AddMultiplierMetadataInput, Resource)

private:
	int64_t value;
	Variant type;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AddMultiplierMetadataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the value property.
	 * @param val New value for value.
	 */
	void set_value(const int64_t &val);
	/**
	 * @brief Get the value property.
	 * @return Current value of value.
	 */
	int64_t get_value() const;

	/**
	 * @brief Set the type property.
	 * @param val New value for type.
	 */
	void set_type(const Variant &val);
	/**
	 * @brief Get the type property.
	 * @return Current value of type.
	 */
	Variant get_type() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADDMULTIPLIERMETADATAINPUT
