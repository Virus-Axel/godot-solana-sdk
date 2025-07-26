#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type HoneycombTransaction.
 */
class HoneycombTransaction : public Resource {
	GDCLASS(HoneycombTransaction, Resource)

private:
	PackedByteArray transaction;
	String blockhash;
	int32_t lastValidBlockHeight;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this HoneycombTransaction.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the transaction property.
	 * @param val New value for transaction.
	 */
	void set_transaction(const PackedByteArray &val);
	/**
	 * @brief Get the transaction property.
	 * @return Current value of transaction.
	 */
	PackedByteArray get_transaction() const;

	/**
	 * @brief Set the blockhash property.
	 * @param val New value for blockhash.
	 */
	void set_blockhash(const String &val);
	/**
	 * @brief Get the blockhash property.
	 * @return Current value of blockhash.
	 */
	String get_blockhash() const;

	/**
	 * @brief Set the lastValidBlockHeight property.
	 * @param val New value for lastValidBlockHeight.
	 */
	void set_lastValidBlockHeight(const int32_t &val);
	/**
	 * @brief Get the lastValidBlockHeight property.
	 * @return Current value of lastValidBlockHeight.
	 */
	int32_t get_lastValidBlockHeight() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
