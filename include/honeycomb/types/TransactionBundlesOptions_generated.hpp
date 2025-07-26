#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLESOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLESOPTIONS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TransactionBundlesOptions.
 */
class TransactionBundlesOptions : public Resource {
	GDCLASS(TransactionBundlesOptions, Resource)

private:
	int32_t bundleSize;
	int32_t firstBundleSize;
	int32_t lastBundleSize;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TransactionBundlesOptions.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the bundleSize property.
	 * @param val New value for bundleSize.
	 */
	void set_bundleSize(const int32_t &val);
	/**
	 * @brief Get the bundleSize property.
	 * @return Current value of bundleSize.
	 */
	int32_t get_bundleSize() const;

	/**
	 * @brief Set the firstBundleSize property.
	 * @param val New value for firstBundleSize.
	 */
	void set_firstBundleSize(const int32_t &val);
	/**
	 * @brief Get the firstBundleSize property.
	 * @return Current value of firstBundleSize.
	 */
	int32_t get_firstBundleSize() const;

	/**
	 * @brief Set the lastBundleSize property.
	 * @param val New value for lastBundleSize.
	 */
	void set_lastBundleSize(const int32_t &val);
	/**
	 * @brief Get the lastBundleSize property.
	 * @return Current value of lastBundleSize.
	 */
	int32_t get_lastBundleSize() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLESOPTIONS
