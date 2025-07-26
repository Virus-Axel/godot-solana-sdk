#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLMULTIPLIERTYPEPARAMSSTAKEAMOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLMULTIPLIERTYPEPARAMSSTAKEAMOUNT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplMultiplierTypeParamsStakeAmount.
 */
class SplMultiplierTypeParamsStakeAmount : public Resource {
	GDCLASS(SplMultiplierTypeParamsStakeAmount, Resource)

private:
	int64_t minAmount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplMultiplierTypeParamsStakeAmount.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the minAmount property.
	 * @param val New value for minAmount.
	 */
	void set_minAmount(const int64_t &val);
	/**
	 * @brief Get the minAmount property.
	 * @return Current value of minAmount.
	 */
	int64_t get_minAmount() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLMULTIPLIERTYPEPARAMSSTAKEAMOUNT
