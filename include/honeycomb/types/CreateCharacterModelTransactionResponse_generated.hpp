#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECHARACTERMODELTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECHARACTERMODELTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateCharacterModelTransactionResponse.
 */
class CreateCharacterModelTransactionResponse : public Resource {
	GDCLASS(CreateCharacterModelTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray characterModel;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateCharacterModelTransactionResponse.
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
	 * @brief Set the characterModel property.
	 * @param val New value for characterModel.
	 */
	void set_characterModel(const PackedByteArray &val);
	/**
	 * @brief Get the characterModel property.
	 * @return Current value of characterModel.
	 */
	PackedByteArray get_characterModel() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECHARACTERMODELTRANSACTIONRESPONSE
