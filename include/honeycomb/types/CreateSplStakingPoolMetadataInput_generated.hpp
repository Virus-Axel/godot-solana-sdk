#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLSTAKINGPOOLMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLSTAKINGPOOLMETADATAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateSplStakingPoolMetadataInput.
 */
class CreateSplStakingPoolMetadataInput : public Resource {
	GDCLASS(CreateSplStakingPoolMetadataInput, Resource)

private:
	String name;
	int64_t minStakeDurationSecs;
	int64_t maxStakeDurationSecs;
	int64_t startTime;
	int64_t endTime;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateSplStakingPoolMetadataInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

	/**
	 * @brief Set the minStakeDurationSecs property.
	 * @param val New value for minStakeDurationSecs.
	 */
	void set_minStakeDurationSecs(const int64_t &val);
	/**
	 * @brief Get the minStakeDurationSecs property.
	 * @return Current value of minStakeDurationSecs.
	 */
	int64_t get_minStakeDurationSecs() const;

	/**
	 * @brief Set the maxStakeDurationSecs property.
	 * @param val New value for maxStakeDurationSecs.
	 */
	void set_maxStakeDurationSecs(const int64_t &val);
	/**
	 * @brief Get the maxStakeDurationSecs property.
	 * @return Current value of maxStakeDurationSecs.
	 */
	int64_t get_maxStakeDurationSecs() const;

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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLSTAKINGPOOLMETADATAINPUT
