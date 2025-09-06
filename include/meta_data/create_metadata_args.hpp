#ifndef GODOT_SOLANA_CREATE_META_DATA_ARGS_HPP
#define GODOT_SOLANA_CREATE_META_DATA_ARGS_HPP

#include <cstdint>

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Arguments used when creating metadata.
 */
class CreateMetaDataArgs : public Resource {
	GDCLASS_CUSTOM(CreateMetaDataArgs, Resource)

private:
	bool enable_creators = false;
	bool enable_collection = false;
	bool enable_uses = false;
	bool enable_collection_details = false;

	String name;
	String symbol;
	String uri;
	uint16_t seller_fee_basis_points = 0;
	Array creators;
	Variant collection;
	Variant uses;
	uint64_t collection_size = 0;

protected:
	/**
	 * @brief Bind methods of CreateMetaDataArgs Resource.
	 */
	static void _bind_methods();

	/**
	 * @brief Set a property with given name.
	 *
	 * @param p_name Name of property to set.
	 * @param p_value Value to set.
	 * @return true if operation succeeded.
	 * @return false if operation failed.
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @brief Get a property with given name.
	 *
	 * @param p_name Name of the property to get.
	 * @param r_ret Value to read to.
	 * @return true if operation succeeded.
	 * @return false if operation failed.
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;

	/**
	 * @brief Get list of all properties.
	 *
	 * @param p_list List of properties.
	 */
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	/**
	 * @brief Set the token name property.
	 *
	 * @param token_name Property value to set.
	 */
	void set_token_name(const String &token_name);

	/**
	 * @brief Get the token name property.
	 *
	 * @return String Token name property.
	 */
	[[nodiscard]] String get_token_name() const;

	/**
	 * @brief Set the symbol property.
	 *
	 * @param symbol Property value to set.
	 */
	void set_symbol(const String &symbol);

	/**
	 * @brief Get the symbol property.
	 *
	 * @return String Symbol property.
	 */
	[[nodiscard]] String get_symbol() const;

	/**
	 * @brief Set the URI property.
	 *
	 * @param uri Property value to set.
	 */
	void set_uri(const String &uri);

	/**
	 * @brief Get the URI property.
	 *
	 * @return String URI property.
	 */
	[[nodiscard]] String get_uri() const;

	/**
	 * @brief Set the seller fee basis points property.
	 *
	 * @param seller_fee_basis_points Property value to set.
	 */
	void set_seller_fee_basis_points(uint16_t seller_fee_basis_points);

	/**
	 * @brief Get the seller fee basis points property.
	 *
	 * @return uint16_t seller fee basis points property.
	 */
	[[nodiscard]] uint16_t get_seller_fee_basis_points() const;

	/**
	 * @brief Add a creator to the creator array.
	 *
	 * @param creator Creator to add to array.
	 */
	void add_creator(const Variant &creator);

	/**
	 * @brief Get the creator property.
	 *
	 * @return Array creator property.
	 */
	[[nodiscard]] Array get_creators() const;

	/**
	 * @brief Set the collection property.
	 *
	 * @param collection Property value to set.
	 */
	void set_collection(const Variant &collection);

	/**
	 * @brief Get the collection property.
	 *
	 * @return Variant Collection property.
	 */
	[[nodiscard]] Variant get_collection() const;

	/**
	 * @brief Serialize the CreateMetaDataArgs.
	 *
	 * @param is_mutable Determines if the token shall be mutable.
	 * @return PackedByteArray Serialized bytes.
	 */
	[[nodiscard]] PackedByteArray serialize(bool is_mutable) const;
};

} //namespace godot

#endif