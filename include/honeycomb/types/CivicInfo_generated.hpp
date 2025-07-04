#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFO

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CivicInfo.
 */
class CivicInfo : public Resource {
	GDCLASS(CivicInfo, Resource)

private:
	String gatekeeperNetwork;
	String expiry;
	int32_t walletIndex;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CivicInfo.
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
	 * @brief Set the expiry property.
	 * @param val New value for expiry.
	 */
	void set_expiry(const String &val);
	/**
	 * @brief Get the expiry property.
	 * @return Current value of expiry.
	 */
	String get_expiry() const;

	/**
	 * @brief Set the walletIndex property.
	 * @param val New value for walletIndex.
	 */
	void set_walletIndex(const int32_t &val);
	/**
	 * @brief Get the walletIndex property.
	 * @return Current value of walletIndex.
	 */
	int32_t get_walletIndex() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CIVICINFO
