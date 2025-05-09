#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WRAPPED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WRAPPED

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Wrapped.
 */
class Wrapped : public Resource {
	GDCLASS(Wrapped, Resource)

private:
	Variant mint;
	Variant criteria;
	String kind;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Wrapped.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const Variant &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	Variant get_mint() const;

	/**
	 * @brief Set the criteria property.
	 * @param val New value for criteria.
	 */
	void set_criteria(const Variant &val);
	/**
	 * @brief Get the criteria property.
	 * @return Current value of criteria.
	 */
	Variant get_criteria() const;

	/**
	 * @brief Set the kind property.
	 * @param val New value for kind.
	 */
	void set_kind(const String &val);
	/**
	 * @brief Get the kind property.
	 * @return Current value of kind.
	 */
	String get_kind() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WRAPPED
