#ifndef GODOT_SOLANA_META_DATA_HPP
#define GODOT_SOLANA_META_DATA_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

#include "meta_data/creator.hpp"

namespace godot{

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
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;
public:
    void set_token_name(const String& token_name);
    String get_token_name();
    void set_symbol(const String& symbol);
    String get_symbol();
    void set_uri(const String& uri);
    String get_uri();
    void set_seller_fee_basis_points(const uint16_t seller_fee_basis_points);
    uint16_t get_seller_fee_basis_points();

    void set_primary_sale_happened(bool primary_sale_happened);
    bool get_primary_sale_happened();
    void set_is_mutable(bool is_mutable);
    bool get_is_mutable();

    void set_mint(const Variant& mint);
    Variant get_mint();
    void set_update_authority(const Variant& update_authority);
    Variant get_update_authority();

    void set_edition_nonce(const Variant& edition_nonce);
    Variant get_edition_nonce();
    void set_token_standard(const Variant& token_standard);
    Variant get_token_standard();
    void set_collection_details(const Variant& collection_details);
    Variant get_collection_details();
    void set_programmable_config(const Variant& programmable_config);
    Variant get_programmable_config();

    void add_creator(const Variant& creator);
    Array get_creators();
    void set_collection(const Variant& collection);
    Variant get_collection();

    PackedByteArray serialize(const bool is_mutable) const;
};
}

#endif