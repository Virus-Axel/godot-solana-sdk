#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharactersFilter.
 */
class CharactersFilter : public Resource {
	GDCLASS(CharactersFilter, Resource)

private:
	Variant owner;
	Variant source;
	Variant usedBy;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharactersFilter.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the owner property.
	 * @param val New value for owner.
	 */
	void set_owner(const Variant &val);
	/**
	 * @brief Get the owner property.
	 * @return Current value of owner.
	 */
	Variant get_owner() const;

	/**
	 * @brief Set the source property.
	 * @param val New value for source.
	 */
	void set_source(const Variant &val);
	/**
	 * @brief Get the source property.
	 * @return Current value of source.
	 */
	Variant get_source() const;

	/**
	 * @brief Set the usedBy property.
	 * @param val New value for usedBy.
	 */
	void set_usedBy(const Variant &val);
	/**
	 * @brief Get the usedBy property.
	 * @return Current value of usedBy.
	 */
	Variant get_usedBy() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERSFILTER
