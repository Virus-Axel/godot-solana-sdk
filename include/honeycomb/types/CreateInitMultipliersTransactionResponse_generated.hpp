#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITMULTIPLIERSTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITMULTIPLIERSTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateInitMultipliersTransactionResponse.
 */
class CreateInitMultipliersTransactionResponse : public Resource {
	GDCLASS(CreateInitMultipliersTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray multipliersAddress;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateInitMultipliersTransactionResponse.
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
	 * @brief Set the multipliersAddress property.
	 * @param val New value for multipliersAddress.
	 */
	void set_multipliersAddress(const PackedByteArray &val);
	/**
	 * @brief Get the multipliersAddress property.
	 * @return Current value of multipliersAddress.
	 */
	PackedByteArray get_multipliersAddress() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITMULTIPLIERSTRANSACTIONRESPONSE
