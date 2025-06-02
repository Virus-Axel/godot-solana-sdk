#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUTCUSTODYINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUTCUSTODYINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ImportResourceInputCustodyInput.
 */
class ImportResourceInputCustodyInput : public Resource {
	GDCLASS(ImportResourceInputCustodyInput, Resource)

private:
	int64_t supply;
	String burnerDestination;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ImportResourceInputCustodyInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the supply property.
	 * @param val New value for supply.
	 */
	void set_supply(const int64_t &val);
	/**
	 * @brief Get the supply property.
	 * @return Current value of supply.
	 */
	int64_t get_supply() const;

	/**
	 * @brief Set the burnerDestination property.
	 * @param val New value for burnerDestination.
	 */
	void set_burnerDestination(const String &val);
	/**
	 * @brief Get the burnerDestination property.
	 * @return Current value of burnerDestination.
	 */
	String get_burnerDestination() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUTCUSTODYINPUT
