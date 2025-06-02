#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSOURCEPARAMSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSOURCEPARAMSFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterSourceParamsFilter.
 */
class CharacterSourceParamsFilter : public Resource {
	GDCLASS(CharacterSourceParamsFilter, Resource)

private:
	Variant mint;
	Variant criteria;
	bool is_compressed;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterSourceParamsFilter.
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
	 * @brief Set the is_compressed property.
	 * @param val New value for is_compressed.
	 */
	void set_is_compressed(const bool &val);
	/**
	 * @brief Get the is_compressed property.
	 * @return Current value of is_compressed.
	 */
	bool get_is_compressed() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSOURCEPARAMSFILTER
