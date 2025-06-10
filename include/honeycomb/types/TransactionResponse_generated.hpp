#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TransactionResponse.
 */
class TransactionResponse : public Resource {
	GDCLASS(TransactionResponse, Resource)

private:
	String signature;
	Dictionary error;
	String status;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TransactionResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the signature property.
	 * @param val New value for signature.
	 */
	void set_signature(const String &val);
	/**
	 * @brief Get the signature property.
	 * @return Current value of signature.
	 */
	String get_signature() const;

	/**
	 * @brief Set the error property.
	 * @param val New value for error.
	 */
	void set_error(const Dictionary &val);
	/**
	 * @brief Get the error property.
	 * @return Current value of error.
	 */
	Dictionary get_error() const;

	/**
	 * @brief Set the status property.
	 * @param val New value for status.
	 */
	void set_status(const String &val);
	/**
	 * @brief Get the status property.
	 * @return Current value of status.
	 */
	String get_status() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONRESPONSE
