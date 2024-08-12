#ifndef GODOT_SOLANA_CREATE_META_DATA_ARGS_HPP
#define GODOT_SOLANA_CREATE_META_DATA_ARGS_HPP

#include <godot_cpp/classes/resource.hpp>

namespace godot{

class CreateMetaDataArgs: public Resource{
    GDCLASS(CreateMetaDataArgs, Resource)

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

    void add_creator(const Variant& creator);
    Array get_creators();
    void set_collection(const Variant& collection);
    Variant get_collection();

    PackedByteArray serialize(const bool is_mutable) const;
};

}

#endif