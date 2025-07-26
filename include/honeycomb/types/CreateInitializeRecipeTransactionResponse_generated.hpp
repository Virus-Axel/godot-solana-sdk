#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZERECIPETRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZERECIPETRANSACTIONRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateInitializeRecipeTransactionResponse.
 */
class CreateInitializeRecipeTransactionResponse : public Resource {
	GDCLASS(CreateInitializeRecipeTransactionResponse, Resource)

private:
	Variant transactions;
	PackedByteArray recipe;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateInitializeRecipeTransactionResponse.
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
	 * @brief Set the recipe property.
	 * @param val New value for recipe.
	 */
	void set_recipe(const PackedByteArray &val);
	/**
	 * @brief Get the recipe property.
	 * @return Current value of recipe.
	 */
	PackedByteArray get_recipe() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZERECIPETRANSACTIONRESPONSE
