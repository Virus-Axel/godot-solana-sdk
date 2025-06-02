#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERHISTORY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERHISTORY

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterHistory.
 */
class CharacterHistory : public Resource {
	GDCLASS(CharacterHistory, Resource)

private:
	PackedByteArray address;
	String event;
	Dictionary event_data;
	String time;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterHistory.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the event property.
	 * @param val New value for event.
	 */
	void set_event(const String &val);
	/**
	 * @brief Get the event property.
	 * @return Current value of event.
	 */
	String get_event() const;

	/**
	 * @brief Set the event_data property.
	 * @param val New value for event_data.
	 */
	void set_event_data(const Dictionary &val);
	/**
	 * @brief Get the event_data property.
	 * @return Current value of event_data.
	 */
	Dictionary get_event_data() const;

	/**
	 * @brief Set the time property.
	 * @param val New value for time.
	 */
	void set_time(const String &val);
	/**
	 * @brief Get the time property.
	 * @return Current value of time.
	 */
	String get_time() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERHISTORY
