#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERMODEL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERMODEL

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterModel.
 */
class CharacterModel : public Resource {
	GDCLASS(CharacterModel, Resource)

private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	int32_t bump;
	Variant key;
	Variant project;
	Dictionary config;
	Array attributes;
	Variant merkle_trees;
	Variant cooldown;
	PackedStringArray equipableCriteria;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterModel.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the program_id property.
	 * @param val New value for program_id.
	 */
	void set_program_id(const PackedByteArray &val);
	/**
	 * @brief Get the program_id property.
	 * @return Current value of program_id.
	 */
	PackedByteArray get_program_id() const;

	/**
	 * @brief Set the discriminator property.
	 * @param val New value for discriminator.
	 */
	void set_discriminator(const PackedByteArray &val);
	/**
	 * @brief Get the discriminator property.
	 * @return Current value of discriminator.
	 */
	PackedByteArray get_discriminator() const;

	/**
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the key property.
	 * @param val New value for key.
	 */
	void set_key(const Variant &val);
	/**
	 * @brief Get the key property.
	 * @return Current value of key.
	 */
	Variant get_key() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

	/**
	 * @brief Set the config property.
	 * @param val New value for config.
	 */
	void set_config(const Dictionary &val);
	/**
	 * @brief Get the config property.
	 * @return Current value of config.
	 */
	Dictionary get_config() const;

	/**
	 * @brief Set the attributes property.
	 * @param val New value for attributes.
	 */
	void set_attributes(const Array &val);
	/**
	 * @brief Get the attributes property.
	 * @return Current value of attributes.
	 */
	Array get_attributes() const;

	/**
	 * @brief Set the merkle_trees property.
	 * @param val New value for merkle_trees.
	 */
	void set_merkle_trees(const Variant &val);
	/**
	 * @brief Get the merkle_trees property.
	 * @return Current value of merkle_trees.
	 */
	Variant get_merkle_trees() const;

	/**
	 * @brief Set the cooldown property.
	 * @param val New value for cooldown.
	 */
	void set_cooldown(const Variant &val);
	/**
	 * @brief Get the cooldown property.
	 * @return Current value of cooldown.
	 */
	Variant get_cooldown() const;

	/**
	 * @brief Set the equipableCriteria property.
	 * @param val New value for equipableCriteria.
	 */
	void set_equipableCriteria(const PackedStringArray &val);
	/**
	 * @brief Get the equipableCriteria property.
	 * @return Current value of equipableCriteria.
	 */
	PackedStringArray get_equipableCriteria() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERMODEL
