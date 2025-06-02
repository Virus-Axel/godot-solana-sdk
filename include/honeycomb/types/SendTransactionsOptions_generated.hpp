#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONSOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONSOPTIONS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SendTransactionsOptions.
 */
class SendTransactionsOptions : public Resource {
	GDCLASS(SendTransactionsOptions, Resource)

private:
	String commitment;
	bool skipPreflight;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SendTransactionsOptions.
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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONSOPTIONS
