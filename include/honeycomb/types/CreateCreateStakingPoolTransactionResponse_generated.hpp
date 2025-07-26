#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATESTAKINGPOOLTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATESTAKINGPOOLTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCreateStakingPoolTransactionResponse.
 */
class CreateCreateStakingPoolTransactionResponse : public Resource {
	GDCLASS(CreateCreateStakingPoolTransactionResponse, Resource)

private:
	Variant transactions;
	PackedByteArray stakingPoolAddress;
	PackedByteArray multipliersAddress;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCreateStakingPoolTransactionResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the transactions property.
	 * @param val New value for transactions.
	 */
	void set_transactions(const Variant &val);
	/**
	 * @brief Get the transactions property.
	 * @return Current value of transactions.
	 */
	Variant get_transactions() const;

	/**
	 * @brief Set the stakingPoolAddress property.
	 * @param val New value for stakingPoolAddress.
	 */
	void set_stakingPoolAddress(const PackedByteArray &val);
	/**
	 * @brief Get the stakingPoolAddress property.
	 * @return Current value of stakingPoolAddress.
	 */
	PackedByteArray get_stakingPoolAddress() const;

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

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATESTAKINGPOOLTRANSACTIONRESPONSE
