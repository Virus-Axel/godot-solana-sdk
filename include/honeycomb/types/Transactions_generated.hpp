#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Transactions.
 */
class Transactions : public Resource {
	GDCLASS(Transactions, Resource)

private:
	Array transactions;
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
	 * @return Dictionary representation of this Transactions.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the transactions property.
	 * @param val New value for transactions.
	 */
	void set_transactions(const Array &val);
	/**
	 * @brief Get the transactions property.
	 * @return Current value of transactions.
	 */
	Array get_transactions() const;

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

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONS
