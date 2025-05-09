#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BADGECRITERIA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BADGECRITERIA

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type BadgeCriteria.
 */
class BadgeCriteria : public Resource {
	GDCLASS(BadgeCriteria, Resource)

private:
	int64_t startTime;
	int64_t endTime;
	int32_t index;
	String condition;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this BadgeCriteria.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the startTime property.
	 * @param val New value for startTime.
	 */
	void set_startTime(const int64_t &val);
	/**
	 * @brief Get the startTime property.
	 * @return Current value of startTime.
	 */
	int64_t get_startTime() const;

	/**
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int64_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int64_t get_endTime() const;

	/**
	 * @brief Set the index property.
	 * @param val New value for index.
	 */
	void set_index(const int32_t &val);
	/**
	 * @brief Get the index property.
	 * @return Current value of index.
	 */
	int32_t get_index() const;

	/**
	 * @brief Set the condition property.
	 * @param val New value for condition.
	 */
	void set_condition(const String &val);
	/**
	 * @brief Get the condition property.
	 * @return Current value of condition.
	 */
	String get_condition() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BADGECRITERIA
