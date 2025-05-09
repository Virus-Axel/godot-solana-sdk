#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_FAUCET
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_FAUCET

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Faucet.
 */
class Faucet : public Resource {
	GDCLASS(Faucet, Resource)

private:
	PackedByteArray address;
	Variant project;
	Variant resource;
	int64_t amount;
	int64_t repeatInterval;
	int64_t lastClaimed;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Faucet.
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
	 * @brief Set the resource property.
	 * @param val New value for resource.
	 */
	void set_resource(const Variant &val);
	/**
	 * @brief Get the resource property.
	 * @return Current value of resource.
	 */
	Variant get_resource() const;

	/**
	 * @brief Set the amount property.
	 * @param val New value for amount.
	 */
	void set_amount(const int64_t &val);
	/**
	 * @brief Get the amount property.
	 * @return Current value of amount.
	 */
	int64_t get_amount() const;

	/**
	 * @brief Set the repeatInterval property.
	 * @param val New value for repeatInterval.
	 */
	void set_repeatInterval(const int64_t &val);
	/**
	 * @brief Get the repeatInterval property.
	 * @return Current value of repeatInterval.
	 */
	int64_t get_repeatInterval() const;

	/**
	 * @brief Set the lastClaimed property.
	 * @param val New value for lastClaimed.
	 */
	void set_lastClaimed(const int64_t &val);
	/**
	 * @brief Get the lastClaimed property.
	 * @return Current value of lastClaimed.
	 */
	int64_t get_lastClaimed() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_FAUCET
