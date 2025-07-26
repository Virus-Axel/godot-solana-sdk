#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MultiplierTypeInput.
 */
class MultiplierTypeInput : public Resource {
	GDCLASS(MultiplierTypeInput, Resource)

private:
	int64_t minStakeDuration;
	int64_t minNftCount;
	String creator;
	String collection;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MultiplierTypeInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the minStakeDuration property.
	 * @param val New value for minStakeDuration.
	 */
	void set_minStakeDuration(const int64_t &val);
	/**
	 * @brief Get the minStakeDuration property.
	 * @return Current value of minStakeDuration.
	 */
	int64_t get_minStakeDuration() const;

	/**
	 * @brief Set the minNftCount property.
	 * @param val New value for minNftCount.
	 */
	void set_minNftCount(const int64_t &val);
	/**
	 * @brief Get the minNftCount property.
	 * @return Current value of minNftCount.
	 */
	int64_t get_minNftCount() const;

	/**
	 * @brief Set the creator property.
	 * @param val New value for creator.
	 */
	void set_creator(const String &val);
	/**
	 * @brief Get the creator property.
	 * @return Current value of creator.
	 */
	String get_creator() const;

	/**
	 * @brief Set the collection property.
	 * @param val New value for collection.
	 */
	void set_collection(const String &val);
	/**
	 * @brief Get the collection property.
	 * @return Current value of collection.
	 */
	String get_collection() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEINPUT
