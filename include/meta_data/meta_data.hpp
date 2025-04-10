#ifndef GODOT_SOLANA_META_DATA_HPP
#define GODOT_SOLANA_META_DATA_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

#include "meta_data/creator.hpp"

namespace godot{

/**
 * @brief Contains token metadata.
 */
class MetaData: public Resource{
    GDCLASS(MetaData, Resource)
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
    void set_token_name(const String& token_name);

    /**
     * @brief Get the token name property.
     * 
     * @return String Token name.
     */
    String get_token_name();

    /**
     * @brief Set the symbol property.
     * 
     * @param symbol symbol to set.
     */
    void set_symbol(const String& symbol);

    /**
     * @brief Get the symbol property.
     * 
     * @return String symbol.
     */
    String get_symbol();

    /**
     * @brief Set the uri property.
     * 
     * @param uri property value to set.
     */
    void set_uri(const String& uri);

    /**
     * @brief Get the uri property.
     * 
     * @return String uri.
     */
    String get_uri();

    /**
     * @brief Set the seller fee basis points property.
     * 
     * @param seller_fee_basis_points value to set.
     */
    void set_seller_fee_basis_points(const uint16_t seller_fee_basis_points);

    /**
     * @brief Get the seller fee basis points property.
     * 
     * @return uint16_t seller fee basis points.
     */
    uint16_t get_seller_fee_basis_points();

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
    bool get_primary_sale_happened();

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
    bool get_is_mutable();

    /**
     * @brief Set the mint property.
     * 
     * @param mint Mint value to set.
     */
    void set_mint(const Variant& mint);

    /**
     * @brief Get the mint property.
     * 
     * @return Variant Mint property.
     */
    Variant get_mint();

    /**
     * @brief Set the update authority property.
     * 
     * @param update_authority Value to set.
     */
    void set_update_authority(const Variant& update_authority);

    /**
     * @brief Get the update authority property.
     * 
     * @return Variant update authority property.
     */
    Variant get_update_authority();

    /**
     * @brief Set the edition nonce property.
     * 
     * @param edition_nonce Value to set.
     */
    void set_edition_nonce(const Variant& edition_nonce);

    /**
     * @brief Get the edition nonce property.
     * 
     * @return Variant edition nonce property.
     */
    Variant get_edition_nonce();

    /**
     * @brief Set the token standard property.
     * 
     * @param token_standard value to set.
     */
    void set_token_standard(const Variant& token_standard);

    /**
     * @brief Get the token standard property.
     * 
     * @return Variant token standard property.
     */
    Variant get_token_standard();

    /**
     * @brief Set the collection details property.
     * 
     * @param collection_details Value to set.
     */
    void set_collection_details(const Variant& collection_details);

    /**
     * @brief Get the collection details property.
     * 
     * @return Variant collection details property.
     */
    Variant get_collection_details();

    /**
     * @brief Set the programmable config property.
     * 
     * @param programmable_config Value to set.
     */
    void set_programmable_config(const Variant& programmable_config);

    /**
     * @brief Get the programmable config property.
     * 
     * @return Variant programmable config property.
     */
    Variant get_programmable_config();

    /**
     * @brief Add creator to creator array.
     * 
     * @param creator Creator to add.
     */
    void add_creator(const Variant& creator);

    /**
     * @brief Get the creators array.
     * 
     * @return Array Creators.
     */
    Array get_creators();

    /**
     * @brief Set the collection property.
     * 
     * @param collection Value to set.
     */
    void set_collection(const Variant& collection);

    /**
     * @brief Get the collection property.
     * 
     * @return Variant collection property.
     */
    Variant get_collection();

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
    PackedByteArray serialize(const bool is_mutable) const;
};
}

#endif