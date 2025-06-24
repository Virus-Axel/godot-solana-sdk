#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHRESPONSE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AuthResponse.
 */
class AuthResponse : public Resource {
	GDCLASS(AuthResponse, Resource)

private:
	String message;
	PackedByteArray tx;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AuthResponse.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the message property.
	 * @param val New value for message.
	 */
	void set_message(const String &val);
	/**
	 * @brief Get the message property.
	 * @return Current value of message.
	 */
	String get_message() const;

	/**
	 * @brief Set the tx property.
	 * @param val New value for tx.
	 */
	void set_tx(const PackedByteArray &val);
	/**
	 * @brief Get the tx property.
	 * @return Current value of tx.
	 */
	PackedByteArray get_tx() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHRESPONSE
