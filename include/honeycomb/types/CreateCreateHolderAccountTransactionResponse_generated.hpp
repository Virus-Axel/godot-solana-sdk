#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEHOLDERACCOUNTTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEHOLDERACCOUNTTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCreateHolderAccountTransactionResponse.
 */
class CreateCreateHolderAccountTransactionResponse : public Resource {
	GDCLASS(CreateCreateHolderAccountTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray holderAccount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCreateHolderAccountTransactionResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the tx property.
	 * @param val New value for tx.
	 */
	void set_tx(const Variant &val);
	/**
	 * @brief Get the tx property.
	 * @return Current value of tx.
	 */
	Variant get_tx() const;

	/**
	 * @brief Set the holderAccount property.
	 * @param val New value for holderAccount.
	 */
	void set_holderAccount(const PackedByteArray &val);
	/**
	 * @brief Get the holderAccount property.
	 * @return Current value of holderAccount.
	 */
	PackedByteArray get_holderAccount() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEHOLDERACCOUNTTRANSACTIONRESPONSE
