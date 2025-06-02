#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TIMEREQUIREMENTPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TIMEREQUIREMENTPARAMS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type TimeRequirementParams.
 */
class TimeRequirementParams : public Resource {
	GDCLASS(TimeRequirementParams, Resource)

private:
	int64_t duration;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this TimeRequirementParams.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the duration property.
	 * @param val New value for duration.
	 */
	void set_duration(const int64_t &val);
	/**
	 * @brief Get the duration property.
	 * @return Current value of duration.
	 */
	int64_t get_duration() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TIMEREQUIREMENTPARAMS
