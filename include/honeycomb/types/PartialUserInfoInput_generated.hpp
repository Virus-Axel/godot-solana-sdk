#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type PartialUserInfoInput.
 */
class PartialUserInfoInput : public Resource {
	GDCLASS(PartialUserInfoInput, Resource)

private:
	String name;
	String bio;
	String pfp;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this PartialUserInfoInput.
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
	 * @brief Set the bio property.
	 * @param val New value for bio.
	 */
	void set_bio(const String &val);
	/**
	 * @brief Get the bio property.
	 * @return Current value of bio.
	 */
	String get_bio() const;

	/**
	 * @brief Set the pfp property.
	 * @param val New value for pfp.
	 */
	void set_pfp(const String &val);
	/**
	 * @brief Get the pfp property.
	 * @return Current value of pfp.
	 */
	String get_pfp() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
