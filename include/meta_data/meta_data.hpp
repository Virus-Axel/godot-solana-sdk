#ifndef GODOT_SOLANA_META_DATA_HPP
#define GODOT_SOLANA_META_DATA_HPP

#include <cstdint>

#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/list.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Contains token metadata.
 */
class MetaData : public Resource {
	GDCLASS_CUSTOM(MetaData, Resource)
private:
	bool enable_creators = false;
	bool enable_collection = false;
	bool enable_uses = false;
	bool enable_collection_details = false;

	bool primary_sale_happened = false;
	bool is_mutable = false;

	String name;
	String symbol;
	String uri;
	uint16_t seller_fee_basis_points = 0;
	Array creators;
	Variant collection;
	Variant uses;
	uint64_t collection_size = 0;

	Variant mint;
	Variant update_authority;

	Variant edition_nonce;
	Variant token_standard;
	Variant collection_details;
	Variant programmable_config;

	void copy_grouping(const Dictionary &dict);
	void copy_creators(const Dictionary &dict);

protected:
	/**
	 * @brief Bind methods for MetaData Resource.
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
	 * @param token_name Name to set.
	 */
	void set_token_name(const String &token_name);

	/**
	 * @brief Get the token name property.
	 *
	 * @return String Token name.
	 */
	[[nodiscard]] String get_token_name() const;

	/**
	 * @brief Set the symbol property.
	 *
	 * @param symbol symbol to set.
	 */
	void set_symbol(const String &symbol);

	/**
	 * @brief Get the symbol property.
	 *
	 * @return String symbol.
	 */
	[[nodiscard]] String get_symbol() const;

	/**
	 * @brief Set the uri property.
	 *
	 * @param uri property value to set.
	 */
	void set_uri(const String &uri);

	/**
	 * @brief Get the uri property.
	 *
	 * @return String uri.
	 */
	[[nodiscard]] String get_uri() const;

	/**
	 * @brief Set the seller fee basis points property.
	 *
	 * @param seller_fee_basis_points value to set.
	 */
	void set_seller_fee_basis_points(uint16_t seller_fee_basis_points);

	/**
	 * @brief Get the seller fee basis points property.
	 *
	 * @return uint16_t seller fee basis points.
	 */
	[[nodiscard]] uint16_t get_seller_fee_basis_points() const;

	/**
	 * @brief Set the primary sale happened property.
	 *
	 * @param primary_sale_happened value to set.
	 */
	void set_primary_sale_happened(bool primary_sale_happened);

	/**
	 * @brief Get the primary sale happened property.
	 *
	 * @return bool primary sale happened property.
	 */
	[[nodiscard]] bool get_primary_sale_happened() const;

	/**
	 * @brief Set the is mutable property.
	 *
	 * @param is_mutable Value to set.
	 */
	void set_is_mutable(bool is_mutable);

	/**
	 * @brief Get the is mutable property.
	 *
	 * @return bool is mutable property.
	 */
	[[nodiscard]] bool get_is_mutable() const;

	/**
	 * @brief Set the mint property.
	 *
	 * @param mint Mint value to set.
	 */
	void set_mint(const Variant &mint);

	/**
	 * @brief Get the mint property.
	 *
	 * @return Variant Mint property.
	 */
	[[nodiscard]] Variant get_mint() const;

	/**
	 * @brief Set the update authority property.
	 *
	 * @param update_authority Value to set.
	 */
	void set_update_authority(const Variant &update_authority);

	/**
	 * @brief Get the update authority property.
	 *
	 * @return Variant update authority property.
	 */
	[[nodiscard]] Variant get_update_authority() const;

	/**
	 * @brief Set the edition nonce property.
	 *
	 * @param edition_nonce Value to set.
	 */
	void set_edition_nonce(const Variant &edition_nonce);

	/**
	 * @brief Get the edition nonce property.
	 *
	 * @return Variant edition nonce property.
	 */
	[[nodiscard]] Variant get_edition_nonce() const;

	/**
	 * @brief Set the token standard property.
	 *
	 * @param token_standard value to set.
	 */
	void set_token_standard(const Variant &token_standard);

	/**
	 * @brief Get the token standard property.
	 *
	 * @return Variant token standard property.
	 */
	[[nodiscard]] Variant get_token_standard() const;

	/**
	 * @brief Set the collection details property.
	 *
	 * @param collection_details Value to set.
	 */
	void set_collection_details(const Variant &collection_details);

	/**
	 * @brief Get the collection details property.
	 *
	 * @return Variant collection details property.
	 */
	[[nodiscard]] Variant get_collection_details() const;

	/**
	 * @brief Set the programmable config property.
	 *
	 * @param programmable_config Value to set.
	 */
	void set_programmable_config(const Variant &programmable_config);

	/**
	 * @brief Get the programmable config property.
	 *
	 * @return Variant programmable config property.
	 */
	[[nodiscard]] Variant get_programmable_config() const;

	/**
	 * @brief Add creator to creator array.
	 *
	 * @param creator Creator to add.
	 */
	void add_creator(const Variant &creator);

	/**
	 * @brief Get the creators array.
	 *
	 * @return Array Creators.
	 */
	[[nodiscard]] Array get_creators() const;

	/**
	 * @brief Set the collection property.
	 *
	 * @param collection Value to set.
	 */
	void set_collection(const Variant &collection);

	/**
	 * @brief Get the collection property.
	 *
	 * @return Variant collection property.
	 */
	[[nodiscard]] Variant get_collection() const;

	/**
	 * @godot
	 * @brief Copies the existing data from a dictionary returned by the DAS methods
	 *
	 * Checks the provided dictionary for metadata fields and copies them if they exist.
	 * If they do not exist the method will proceed without warnings.
	 * This method is meant to be used with dictionary format returned from DAS SolanaClient methods.
	 *
	 * @param other Dictionary with data to copy. Must be compliant with the format returned from the
	 * DAS SolanaClient methods.
	 */
	void copy_from_dict(const Dictionary &other);

	/**
	 * @brief Serialize MetaData.
	 *
	 * @param is_mutable Controls if Token should be mutable or not.
	 * @return PackedByteArray Serialized bytes.
	 */
	[[nodiscard]] PackedByteArray serialize(bool is_mutable) const;
};
} //namespace godot

#endif