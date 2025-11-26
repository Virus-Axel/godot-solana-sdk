#ifndef IDL_UTILS_HPP
#define IDL_UTILS_HPP

#include <cstdint>

#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {

/**
 * @brief Provides utility methods to help with IDL management.
 *
 * This class provides static methods for various IDL operation. Their main purposes are
 * to identify IDL elements, serialize and deserialize data types.
 */
class IdlUtils {
private:
	static Variant::Type godot_type_from_dictionary(const Dictionary &anchor_type_dict);
	static Variant::Type godot_type_from_string(const String &anchor_type_string);

	static PackedByteArray serialize_int(int64_t value);
	static PackedByteArray serialize_float(double value);
	static PackedByteArray serialize_string(const String &value);
	static PackedByteArray serialize_byte_array(const PackedByteArray &value);
	static PackedByteArray serialize_variant_array(const Array &value);
	static PackedByteArray serialize_dictionary(const Dictionary &value);
	static PackedByteArray serialize_object(const Variant &value);
	static PackedByteArray serialize_typed_primitive(const Dictionary &dict);
	static PackedByteArray serialize_option(const Dictionary &value);

public:
	/**
	 * @brief Check if IDL element is of a primitive data type.
	 *
	 * Checks for the custom dictionary fields "dataType" and "value". If "value" is not "option"
	 * the dictionary is considered to be a typed primitive.
	 *
	 * @note The typed primitive type is custom to the Godot Solana SDK. It is not conventional structure.
	 *
	 * @param dict IDL element to check.
	 * @return true if element represents a typed primitive.
	 * @return false if element not represents a typed primitive.
	 */
	static bool is_typed_primitive(const Dictionary &dict);

	/**
	 * @brief Check if IDL element is of an option data type.
	 *
	 * Check for the custom dictionary fields "dataType" and "value". If "value" is "option" the
	 * dictionary is considered to be an option. If the provided data type is not a dictionary, it
	 * is not an option.
	 *
	 * @note The option type is custom to the Godot Solana SDK. It is not conventional structure.
	 *
	 * @param var IDL element to check.
	 * @return true if element represents an option.
	 * @return false if element not represents an option.
	 */
	static bool is_option(const Variant &var);

	/**
	 * @brief Get the godot type of an anchor type.
	 *
	 * @param anchor_type Anchor type information.
	 * @return Variant::Type Corresponding godot type.
	 */
	static Variant::Type get_godot_type(const Variant &anchor_type);

	/**
	 * @brief Get a decorated integer Dictionary.
	 *
	 * Creates a Dictionary object to represent a typed integer primitive. This decorated integer is
	 * used to be able to save how many bits the integer need to be.
	 *
	 * @note The decorated integer type is custom to the Godot Solana SDK. It is not conventional structure.
	 *
	 * @param anchor_int_type The String used by anchor to specify the data type.
	 * @param integer_value Value of the integer.
	 * @return Dictionary Decorated Integer in a Dictionary.
	 */
	static Dictionary get_decorated_integer(const String &anchor_int_type, const Variant &integer_value);

	/**
	 * @brief Get the decorated float Dictionary
	 *
	 * Creates a Dictionary object to represent a typed float primitive. This decorated float is
	 * used to be able to save whether single precision or double precision is being used.
	 *
	 * @param anchor_float_type The String used by anchor to specify the data type.
	 * @param float_value Value of the integer.
	 * @return Dictionary Decorated Integer in a Dictionary.
	 */
	static Dictionary get_decorated_float(const String &anchor_float_type, const Variant &float_value);

	/**
	 * @brief Check if dataType is any of the values in the provided array.
	 *
	 * @param anchor_type Anchor type dictionary.
	 * @param data_type_strings Array of type strings to check for.
	 * @return true If dataType is in the array of Strings.
	 * @return false If dataType is not in the array of Strings.
	 */
	static bool type_contains_any(const Dictionary &anchor_type, const PackedStringArray &data_type_strings);

	/**
	 * @brief Searches an element array in the IDL for the provided name.
	 *
	 * @param idl IDL to search in.
	 * @param idl_key Key to an array element to search in.
	 * @param name Name to find in the array.
	 * @return Dictionary value at the name that was found. Empty if none was found.
	 */
	static Dictionary find_name_in_key_array(const Dictionary &idl, const String &idl_key, const String &name);

	/**
	 * @brief Serializes a Variant.
	 *
	 * @param variant Variant to serialize.
	 * @return PackedByteArray Byte serialization.
	 */
	static PackedByteArray serialize_variant(const Variant &variant);

	/**
	 * @brief Get the type name of an anchor object.
	 *
	 * @param anchor_type Anchor object.
	 * @return String name of anchor object.
	 */
	static String get_object_name(const Variant &anchor_type);
};

} //namespace godot

#endif