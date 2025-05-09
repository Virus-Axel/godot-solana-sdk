#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFOINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CivicInfoInput.
 */
class CivicInfoInput : public Resource {
	GDCLASS(CivicInfoInput, Resource)

private:
	String gatekeeperNetwork;
	String wallet;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CivicInfoInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the gatekeeperNetwork property.
	 * @param val New value for gatekeeperNetwork.
	 */
	void set_gatekeeperNetwork(const String &val);
	/**
	 * @brief Get the gatekeeperNetwork property.
	 * @return Current value of gatekeeperNetwork.
	 */
	String get_gatekeeperNetwork() const;

	/**
	 * @brief Set the wallet property.
	 * @param val New value for wallet.
	 */
	void set_wallet(const String &val);
	/**
	 * @brief Get the wallet property.
	 * @return Current value of wallet.
	 */
	String get_wallet() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFOINPUT
