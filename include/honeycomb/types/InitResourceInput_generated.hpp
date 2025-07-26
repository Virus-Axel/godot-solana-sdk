#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type InitResourceInput.
 */
class InitResourceInput : public Resource {
	GDCLASS(InitResourceInput, Resource)

private:
	String name;
	String symbol;
	String uri;
	int32_t decimals;
	PackedStringArray tags;
	String storage;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this InitResourceInput.
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
	 * @brief Set the symbol property.
	 * @param val New value for symbol.
	 */
	void set_symbol(const String &val);
	/**
	 * @brief Get the symbol property.
	 * @return Current value of symbol.
	 */
	String get_symbol() const;

	/**
	 * @brief Set the uri property.
	 * @param val New value for uri.
	 */
	void set_uri(const String &val);
	/**
	 * @brief Get the uri property.
	 * @return Current value of uri.
	 */
	String get_uri() const;

	/**
	 * @brief Set the decimals property.
	 * @param val New value for decimals.
	 */
	void set_decimals(const int32_t &val);
	/**
	 * @brief Get the decimals property.
	 * @return Current value of decimals.
	 */
	int32_t get_decimals() const;

	/**
	 * @brief Set the tags property.
	 * @param val New value for tags.
	 */
	void set_tags(const PackedStringArray &val);
	/**
	 * @brief Get the tags property.
	 * @return Current value of tags.
	 */
	PackedStringArray get_tags() const;

	/**
	 * @brief Set the storage property.
	 * @param val New value for storage.
	 */
	void set_storage(const String &val);
	/**
	 * @brief Get the storage property.
	 * @return Current value of storage.
	 */
	String get_storage() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_INITRESOURCEINPUT
