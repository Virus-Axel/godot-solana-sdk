#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCreateMissionTransactionResponse.
 */
class CreateCreateMissionTransactionResponse : public Resource {
	GDCLASS(CreateCreateMissionTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray missionAddress;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCreateMissionTransactionResponse.
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
	 * @brief Set the missionAddress property.
	 * @param val New value for missionAddress.
	 */
	void set_missionAddress(const PackedByteArray &val);
	/**
	 * @brief Get the missionAddress property.
	 * @return Current value of missionAddress.
	 */
	PackedByteArray get_missionAddress() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONTRANSACTIONRESPONSE
