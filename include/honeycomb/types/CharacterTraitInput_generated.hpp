#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAITINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAITINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterTraitInput.
 */
class CharacterTraitInput : public Resource {
	GDCLASS(CharacterTraitInput, Resource)

private:
	String layer;
	String name;
	String uri;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterTraitInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the layer property.
	 * @param val New value for layer.
	 */
	void set_layer(const String &val);
	/**
	 * @brief Get the layer property.
	 * @return Current value of layer.
	 */
	String get_layer() const;

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

	/**
	 * @brief Set the uri property.
	 * @param val New value for uri.
	 */
	void set_uri(const String &val);
	/**
	 * @brief Get the uri property.
	 * @return Current value of uri.
	 */
	String get_uri() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAITINPUT
