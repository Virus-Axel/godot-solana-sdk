#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONPOOLTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONPOOLTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCreateMissionPoolTransactionResponse.
 */
class CreateCreateMissionPoolTransactionResponse : public Resource {
	GDCLASS(CreateCreateMissionPoolTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray missionPoolAddress;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCreateMissionPoolTransactionResponse.
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
	 * @brief Set the missionPoolAddress property.
	 * @param val New value for missionPoolAddress.
	 */
	void set_missionPoolAddress(const PackedByteArray &val);
	/**
	 * @brief Get the missionPoolAddress property.
	 * @return Current value of missionPoolAddress.
	 */
	PackedByteArray get_missionPoolAddress() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONPOOLTRANSACTIONRESPONSE
