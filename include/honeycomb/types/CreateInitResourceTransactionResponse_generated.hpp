#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITRESOURCETRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITRESOURCETRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateInitResourceTransactionResponse.
 */
class CreateInitResourceTransactionResponse : public Resource {
	GDCLASS(CreateInitResourceTransactionResponse, Resource)

private:
	Variant tx;
	PackedByteArray resource;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateInitResourceTransactionResponse.
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
	 * @brief Set the resource property.
	 * @param val New value for resource.
	 */
	void set_resource(const PackedByteArray &val);
	/**
	 * @brief Get the resource property.
	 * @return Current value of resource.
	 */
	PackedByteArray get_resource() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITRESOURCETRANSACTIONRESPONSE
