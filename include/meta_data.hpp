#ifndef GODOT_SOLANA_META_DATA_HPP
#define GODOT_SOLANA_META_DATA_HPP

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot{

class MetaDataCreator: public Resource{
    GDCLASS(MetaDataCreator, Resource)
private:
    Variant address;
    bool verified = false;
    uint16_t share = 0;

protected:
    static void _bind_methods();
public:
    void set_address(const Variant& p_value);
    Variant get_address();

    void set_verified(const bool p_value);
    bool get_verified();

    void set_share(const uint32_t p_value);
    uint32_t get_share();

    PackedByteArray serialize() const;
};

class MetaDataCollection: public Resource{
    GDCLASS(MetaDataCollection, Resource)

private:
    Variant key;
    bool verified = false;

protected:
    static void _bind_methods();

public:
    void set_key(const Variant& p_value);
    Variant get_key();

    void set_verified(const bool p_value);
    bool get_verified();

    PackedByteArray serialize() const;
};

class MetaDataUses: public Resource{
    GDCLASS(MetaDataUses, Resource)

private:
    enum UseMethod{
        Burn = 0,
        Multiple = 1,
        Single = 2,
    };
    UseMethod use_method = UseMethod::Burn;
    uint64_t total = 0;
    uint64_t remaining = 0;

protected:
    static void _bind_methods();

public:
    void set_use_method(const uint32_t p_value);
    uint32_t get_use_method();

    void set_total(const uint64_t p_value);
    uint64_t get_total();

    void set_remaining(const uint64_t p_value);
    uint64_t get_remaining();

    PackedByteArray serialize() const;
};

MAKE_TYPED_ARRAY(MetaDataCreator, Variant::OBJECT)

class MetaData: public Resource{
    GDCLASS(MetaData, Resource)

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
    void set_collection(const Variant& collection);

    PackedByteArray serialize(const bool is_mutable) const;
};

}

#endif