#ifndef SOLANA_SDK_HASH_HPP
#define SOLANA_SDK_HASH_HPP

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

const int HASH_LENGTH = 32;

namespace godot {

/**
 * @brief Arbitrary byte long hash.
 *
 * @warning Do not use, it has no purpose.
 * @todo Find a purpose of this or remove.
 */
class Hash : public Resource {
	GDCLASS_CUSTOM(Hash, Resource)

private:
	bool unique = true;
	String value = "";
	PackedByteArray bytes;

protected:
	/**
	 * @brief Bind methods of Hash Resource.
	 *
	 */
	static void _bind_methods();

	/**
	 * @brief Set Hash property.
	 *
	 * @param p_name Name of property to set.
	 * @param p_value Value to set.
	 * @return true on success.
	 * @return false on failure.
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @brief Get Hash property.
	 *
	 * @param p_name Name of property to get.
	 * @param r_ret Variable to write property to.
	 * @return true on success.
	 * @return false on failure.
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;

	/**
	 * @brief Get property list
	 *
	 * @param p_list Property list.
	 */
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	Hash() = default;

	/**
	 * @brief Set the bytes value.
	 *
	 * @param p_value Value of bytes.
	 */
	void set_value(const String &p_value);

	/**
	 * @brief Get the value property
	 *
	 * @return String Value property.
	 */
	String get_value();

	/**
	 * @brief Set the bytes property.
	 *
	 * @param p_value Value of the bytes to set.
	 */
	void set_bytes(const PackedByteArray &p_value);

	/**
	 * @brief Get the bytes property.
	 *
	 * @return PackedByteArray Bytes property.
	 */
	PackedByteArray get_bytes();

	/**
	 * @brief Set the unique property.
	 *
	 * @param p_value Unique property value to set.
	 */
	void set_unique(bool p_value);

	/**
	 * @brief Get the unique property.
	 *
	 * @return bool Unique property.
	 */
	[[nodiscard]] bool get_unique() const;

	~Hash() override = default;
};
} //namespace godot

#endif