#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCOLLECTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCOLLECTION

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MultiplierTypeParamsCollection.
 */
class MultiplierTypeParamsCollection : public Resource {
	GDCLASS(MultiplierTypeParamsCollection, Resource)

private:
	Variant collection;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MultiplierTypeParamsCollection.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the collection property.
	 * @param val New value for collection.
	 */
	void set_collection(const Variant &val);
	/**
	 * @brief Get the collection property.
	 * @return Current value of collection.
	 */
	Variant get_collection() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSCOLLECTION
