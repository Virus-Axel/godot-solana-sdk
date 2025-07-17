#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBEGINCOOKINGTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBEGINCOOKINGTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateBeginCookingTransactionResponse.
 */
class CreateBeginCookingTransactionResponse : public Resource {
	GDCLASS(CreateBeginCookingTransactionResponse, Resource)

private:
	Variant transaction;
	PackedByteArray cooking;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateBeginCookingTransactionResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the transaction property.
	 * @param val New value for transaction.
	 */
	void set_transaction(const Variant &val);
	/**
	 * @brief Get the transaction property.
	 * @return Current value of transaction.
	 */
	Variant get_transaction() const;

	/**
	 * @brief Set the cooking property.
	 * @param val New value for cooking.
	 */
	void set_cooking(const PackedByteArray &val);
	/**
	 * @brief Get the cooking property.
	 * @return Current value of cooking.
	 */
	PackedByteArray get_cooking() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBEGINCOOKINGTRANSACTIONRESPONSE
