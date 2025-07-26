#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SOCIALINFO
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SOCIALINFO

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SocialInfo.
 */
class SocialInfo : public Resource {
	GDCLASS(SocialInfo, Resource)

private:
	String twitter;
	String discord;
	String steam;
	Array civic;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SocialInfo.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the twitter property.
	 * @param val New value for twitter.
	 */
	void set_twitter(const String &val);
	/**
	 * @brief Get the twitter property.
	 * @return Current value of twitter.
	 */
	String get_twitter() const;

	/**
	 * @brief Set the discord property.
	 * @param val New value for discord.
	 */
	void set_discord(const String &val);
	/**
	 * @brief Get the discord property.
	 * @return Current value of discord.
	 */
	String get_discord() const;

	/**
	 * @brief Set the steam property.
	 * @param val New value for steam.
	 */
	void set_steam(const String &val);
	/**
	 * @brief Get the steam property.
	 * @return Current value of steam.
	 */
	String get_steam() const;

	/**
	 * @brief Set the civic property.
	 * @param val New value for civic.
	 */
	void set_civic(const Array &val);
	/**
	 * @brief Get the civic property.
	 * @return Current value of civic.
	 */
	Array get_civic() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SOCIALINFO
