#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DISRCRIMINATORFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DISRCRIMINATORFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type DisrcriminatorFilter.
 */
class DisrcriminatorFilter : public Resource {
	GDCLASS(DisrcriminatorFilter, Resource)

private:
	String programId;
	String accountName;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this DisrcriminatorFilter.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the programId property.
	 * @param val New value for programId.
	 */
	void set_programId(const String &val);
	/**
	 * @brief Get the programId property.
	 * @return Current value of programId.
	 */
	String get_programId() const;

	/**
	 * @brief Set the accountName property.
	 * @param val New value for accountName.
	 */
	void set_accountName(const String &val);
	/**
	 * @brief Get the accountName property.
	 * @return Current value of accountName.
	 */
	String get_accountName() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DISRCRIMINATORFILTER
