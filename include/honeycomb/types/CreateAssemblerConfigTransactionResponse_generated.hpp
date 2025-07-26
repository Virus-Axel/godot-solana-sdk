#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEASSEMBLERCONFIGTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEASSEMBLERCONFIGTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateAssemblerConfigTransactionResponse.
 */
class CreateAssemblerConfigTransactionResponse : public Resource {
	GDCLASS(CreateAssemblerConfigTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray assemblerConfig;
	PackedByteArray treeAddress;
	int32_t proofBytes;
	int32_t space;
	float cost;
	int32_t maxTreeCapacity;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateAssemblerConfigTransactionResponse.
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
	 * @brief Set the assemblerConfig property.
	 * @param val New value for assemblerConfig.
	 */
	void set_assemblerConfig(const PackedByteArray &val);
	/**
	 * @brief Get the assemblerConfig property.
	 * @return Current value of assemblerConfig.
	 */
	PackedByteArray get_assemblerConfig() const;

	/**
	 * @brief Set the treeAddress property.
	 * @param val New value for treeAddress.
	 */
	void set_treeAddress(const PackedByteArray &val);
	/**
	 * @brief Get the treeAddress property.
	 * @return Current value of treeAddress.
	 */
	PackedByteArray get_treeAddress() const;

	/**
	 * @brief Set the proofBytes property.
	 * @param val New value for proofBytes.
	 */
	void set_proofBytes(const int32_t &val);
	/**
	 * @brief Get the proofBytes property.
	 * @return Current value of proofBytes.
	 */
	int32_t get_proofBytes() const;

	/**
	 * @brief Set the space property.
	 * @param val New value for space.
	 */
	void set_space(const int32_t &val);
	/**
	 * @brief Get the space property.
	 * @return Current value of space.
	 */
	int32_t get_space() const;

	/**
	 * @brief Set the cost property.
	 * @param val New value for cost.
	 */
	void set_cost(const float &val);
	/**
	 * @brief Get the cost property.
	 * @return Current value of cost.
	 */
	float get_cost() const;

	/**
	 * @brief Set the maxTreeCapacity property.
	 * @param val New value for maxTreeCapacity.
	 */
	void set_maxTreeCapacity(const int32_t &val);
	/**
	 * @brief Get the maxTreeCapacity property.
	 * @return Current value of maxTreeCapacity.
	 */
	int32_t get_maxTreeCapacity() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEASSEMBLERCONFIGTRANSACTIONRESPONSE
