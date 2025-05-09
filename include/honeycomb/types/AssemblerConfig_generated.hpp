#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIG

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AssemblerConfig.
 */
class AssemblerConfig : public Resource {
	GDCLASS(AssemblerConfig, Resource)

private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	String ticker;
	Variant project;
	PackedStringArray order;
	Variant merkle_trees;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AssemblerConfig.
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
	 * @brief Set the ticker property.
	 * @param val New value for ticker.
	 */
	void set_ticker(const String &val);
	/**
	 * @brief Get the ticker property.
	 * @return Current value of ticker.
	 */
	String get_ticker() const;

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
	 * @brief Set the order property.
	 * @param val New value for order.
	 */
	void set_order(const PackedStringArray &val);
	/**
	 * @brief Get the order property.
	 * @return Current value of order.
	 */
	PackedStringArray get_order() const;

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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIG
