#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHCONFIRMED
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHCONFIRMED

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AuthConfirmed.
 */
class AuthConfirmed : public Resource {
	GDCLASS(AuthConfirmed, Resource)

private:
	String accessToken;
	Variant user;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AuthConfirmed.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the accessToken property.
	 * @param val New value for accessToken.
	 */
	void set_accessToken(const String &val);
	/**
	 * @brief Get the accessToken property.
	 * @return Current value of accessToken.
	 */
	String get_accessToken() const;

	/**
	 * @brief Set the user property.
	 * @param val New value for user.
	 */
	void set_user(const Variant &val);
	/**
	 * @brief Get the user property.
	 * @return Current value of user.
	 */
	Variant get_user() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHCONFIRMED
