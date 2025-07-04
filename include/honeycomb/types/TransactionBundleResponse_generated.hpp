#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLERESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLERESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TransactionBundleResponse.
 */
class TransactionBundleResponse : public Resource {
	GDCLASS(TransactionBundleResponse, Resource)

private:
	int32_t bundleId;
	Array responses;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TransactionBundleResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the bundleId property.
	 * @param val New value for bundleId.
	 */
	void set_bundleId(const int32_t &val);
	/**
	 * @brief Get the bundleId property.
	 * @return Current value of bundleId.
	 */
	int32_t get_bundleId() const;

	/**
	 * @brief Set the responses property.
	 * @param val New value for responses.
	 */
	void set_responses(const Array &val);
	/**
	 * @brief Get the responses property.
	 * @return Current value of responses.
	 */
	Array get_responses() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONBUNDLERESPONSE
