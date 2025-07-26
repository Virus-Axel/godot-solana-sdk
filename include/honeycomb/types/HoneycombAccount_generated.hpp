#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBACCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBACCOUNT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type HoneycombAccount.
 */
class HoneycombAccount : public Resource {
	GDCLASS(HoneycombAccount, Resource)

private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	Dictionary parsed_data;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this HoneycombAccount.
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
	 * @brief Set the parsed_data property.
	 * @param val New value for parsed_data.
	 */
	void set_parsed_data(const Dictionary &val);
	/**
	 * @brief Get the parsed_data property.
	 * @return Current value of parsed_data.
	 */
	Dictionary get_parsed_data() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBACCOUNT
