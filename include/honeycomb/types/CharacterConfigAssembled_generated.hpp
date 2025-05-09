#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGASSEMBLED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGASSEMBLED

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterConfigAssembled.
 */
class CharacterConfigAssembled : public Resource {
	GDCLASS(CharacterConfigAssembled, Resource)

private:
	String kind;
	Variant assemblerConfig;
	String name;
	String symbol;
	String description;
	Array creators;
	int32_t sellerFeeBasisPoints;
	String collectionName;
	Variant mintAs;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterConfigAssembled.
	 */
	Dictionary to_dict();

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

	/**
	 * @brief Set the assemblerConfig property.
	 * @param val New value for assemblerConfig.
	 */
	void set_assemblerConfig(const Variant &val);
	/**
	 * @brief Get the assemblerConfig property.
	 * @return Current value of assemblerConfig.
	 */
	Variant get_assemblerConfig() const;

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
	 * @brief Set the symbol property.
	 * @param val New value for symbol.
	 */
	void set_symbol(const String &val);
	/**
	 * @brief Get the symbol property.
	 * @return Current value of symbol.
	 */
	String get_symbol() const;

	/**
	 * @brief Set the description property.
	 * @param val New value for description.
	 */
	void set_description(const String &val);
	/**
	 * @brief Get the description property.
	 * @return Current value of description.
	 */
	String get_description() const;

	/**
	 * @brief Set the creators property.
	 * @param val New value for creators.
	 */
	void set_creators(const Array &val);
	/**
	 * @brief Get the creators property.
	 * @return Current value of creators.
	 */
	Array get_creators() const;

	/**
	 * @brief Set the sellerFeeBasisPoints property.
	 * @param val New value for sellerFeeBasisPoints.
	 */
	void set_sellerFeeBasisPoints(const int32_t &val);
	/**
	 * @brief Get the sellerFeeBasisPoints property.
	 * @return Current value of sellerFeeBasisPoints.
	 */
	int32_t get_sellerFeeBasisPoints() const;

	/**
	 * @brief Set the collectionName property.
	 * @param val New value for collectionName.
	 */
	void set_collectionName(const String &val);
	/**
	 * @brief Get the collectionName property.
	 * @return Current value of collectionName.
	 */
	String get_collectionName() const;

	/**
	 * @brief Set the mintAs property.
	 * @param val New value for mintAs.
	 */
	void set_mintAs(const Variant &val);
	/**
	 * @brief Get the mintAs property.
	 * @return Current value of mintAs.
	 */
	Variant get_mintAs() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERCONFIGASSEMBLED
