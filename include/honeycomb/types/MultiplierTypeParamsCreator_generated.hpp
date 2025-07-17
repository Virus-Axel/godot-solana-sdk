#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCREATOR
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCREATOR

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MultiplierTypeParamsCreator.
 */
class MultiplierTypeParamsCreator : public Resource {
	GDCLASS(MultiplierTypeParamsCreator, Resource)

private:
	Variant creator;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MultiplierTypeParamsCreator.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the creator property.
	 * @param val New value for creator.
	 */
	void set_creator(const Variant &val);
	/**
	 * @brief Get the creator property.
	 * @return Current value of creator.
	 */
	Variant get_creator() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCREATOR
