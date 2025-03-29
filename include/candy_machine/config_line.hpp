#ifndef SOLANA_SDK_MPL_CONFIG_LINE
#define SOLANA_SDK_MPL_CONFIG_LINE

#include <account_meta.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <solana_client.hpp>

namespace godot {

/**
 * @brief Name and URI information for Candy Machine Program.
 */
class ConfigLine : public Resource {
	GDCLASS(ConfigLine, Resource)
private:
	String name;
	String uri;

protected:
	/**
	 * @brief Bind methods of ConfigLine Resource.
	 */
	static void _bind_methods();

public:
	ConfigLine();

	/**
	 * @brief Set the name property.
	 * 
	 * @param value name value.
	 */
	void set_name(const String &value);

	/**
	 * @brief Get the name property.
	 * 
	 * @return String Name property.
	 */
	String get_name();

	/**
	 * @brief Set the URI property.
	 * 
	 * @param value URI value.
	 */
	void set_uri(const String &value);

	/**
	 * @brief Get the URI property.
	 * 
	 * @return String URI property.
	 */
	String get_uri();

	/**
	 * @brief Serialize the ConfigLine.
	 * 
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize();
	~ConfigLine();
};

/**
 * @brief Settings about how asset names and URI will be assigned to tokens.
 */
class ConfigLineSetting : public Resource {
	GDCLASS(ConfigLineSetting, Resource)
private:
	String prefix_name;
	String prefix_uri;
	uint32_t name_length = 0;
	uint32_t uri_length = 0;
	bool is_sequential = false;

protected:
	/**
	 * @brief Bind methods of ConfigLineSetting Resource.
	 */
	static void _bind_methods();

public:
	ConfigLineSetting();

	/**
	 * @brief Set the prefix name property.
	 * 
	 * @param value prefix name value.
	 */
	void set_prefix_name(const String &value);

	/**
	 * @brief Get the prefix name property
	 * 
	 * @return String Prefix name property.
	 */
	String get_prefix_name();

	/**
	 * @brief Set the name length property.
	 * 
	 * @param value name length value.
	 */
	void set_name_length(const uint32_t value);

	/**
	 * @brief Get the name length property
	 * 
	 * @return String Name length property.
	 */
	uint32_t get_name_length();

	/**
	 * @brief Set the prefix URI property.
	 * 
	 * @param value prefix URI value.
	 */
	void set_prefix_uri(const String &value);

	/**
	 * @brief Get the prefix URI property
	 * 
	 * @return String Prefix URI property.
	 */
	String get_prefix_uri();

	/**
	 * @brief Set the URI length property.
	 * 
	 * @param value URI length value.
	 */
	void set_uri_length(const uint32_t value);

	/**
	 * @brief Get the URI length property
	 * 
	 * @return uint32_t URI length property.
	 */
	uint32_t get_uri_length();

	/**
	 * @brief Set the is sequential property.
	 * 
	 * @param value is sequential value.
	 */
	void set_is_sequential(const bool value);

	/**
	 * @brief Get the is sequential property
	 * 
	 * @return bool Is sequential property.
	 */
	bool get_is_sequential();

	/**
	 * @brief Serialize ConfigLineSetting.
	 * 
	 * @return PackedByteArray Serialized bytes.
	 */
	PackedByteArray serialize();
	~ConfigLineSetting();
};

} //namespace godot

#endif