#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONBUNDLESOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONBUNDLESOPTIONS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SendTransactionBundlesOptions.
 */
class SendTransactionBundlesOptions : public Resource {
	GDCLASS(SendTransactionBundlesOptions, Resource)

private:
	String commitment;
	bool skipPreflight;
	Variant bundles;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SendTransactionBundlesOptions.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the commitment property.
	 * @param val New value for commitment.
	 */
	void set_commitment(const String &val);
	/**
	 * @brief Get the commitment property.
	 * @return Current value of commitment.
	 */
	String get_commitment() const;

	/**
	 * @brief Set the skipPreflight property.
	 * @param val New value for skipPreflight.
	 */
	void set_skipPreflight(const bool &val);
	/**
	 * @brief Get the skipPreflight property.
	 * @return Current value of skipPreflight.
	 */
	bool get_skipPreflight() const;

	/**
	 * @brief Set the bundles property.
	 * @param val New value for bundles.
	 */
	void set_bundles(const Variant &val);
	/**
	 * @brief Get the bundles property.
	 * @return Current value of bundles.
	 */
	Variant get_bundles() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONBUNDLESOPTIONS
