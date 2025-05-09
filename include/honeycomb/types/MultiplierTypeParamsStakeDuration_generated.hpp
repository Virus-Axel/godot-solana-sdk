#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSSTAKEDURATION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSSTAKEDURATION

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MultiplierTypeParamsStakeDuration.
 */
class MultiplierTypeParamsStakeDuration : public Resource {
	GDCLASS(MultiplierTypeParamsStakeDuration, Resource)

private:
	int64_t minDuration;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MultiplierTypeParamsStakeDuration.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the minDuration property.
	 * @param val New value for minDuration.
	 */
	void set_minDuration(const int64_t &val);
	/**
	 * @brief Get the minDuration property.
	 * @return Current value of minDuration.
	 */
	int64_t get_minDuration() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSSTAKEDURATION
