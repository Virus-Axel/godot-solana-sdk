#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEUPDATEMISSIONTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEUPDATEMISSIONTRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateUpdateMissionTransactionResponse.
 */
class CreateUpdateMissionTransactionResponse : public Resource {
	GDCLASS(CreateUpdateMissionTransactionResponse, Resource)

private:
	Variant tx;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateUpdateMissionTransactionResponse.
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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEUPDATEMISSIONTRANSACTIONRESPONSE
