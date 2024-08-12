#include "meta_data/create_metadata_args.hpp"

#include "meta_data/creator.hpp"
#include "meta_data/collection.hpp"
#include "meta_data/uses.hpp"
#include "utils.hpp"

#include <godot_cpp/core/property_info.hpp>


namespace godot{

void CreateMetaDataArgs::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_token_name"), &CreateMetaDataArgs::get_token_name);
    ClassDB::bind_method(D_METHOD("set_token_name", "token_name"), &CreateMetaDataArgs::set_token_name);
    ClassDB::bind_method(D_METHOD("get_symbol"), &CreateMetaDataArgs::get_symbol);
    ClassDB::bind_method(D_METHOD("set_symbol", "symbol"), &CreateMetaDataArgs::set_symbol);
    ClassDB::bind_method(D_METHOD("get_uri"), &CreateMetaDataArgs::get_uri);
    ClassDB::bind_method(D_METHOD("set_uri", "uri"), &CreateMetaDataArgs::set_uri);

    ClassDB::bind_method(D_METHOD("get_seller_fee_basis_points"), &CreateMetaDataArgs::get_seller_fee_basis_points);

    ClassDB::bind_method(D_METHOD("get_creators"), &CreateMetaDataArgs::get_creators);
    ClassDB::bind_method(D_METHOD("get_collection"), &CreateMetaDataArgs::get_collection);
}

bool CreateMetaDataArgs::_set(const StringName &p_name, const Variant &p_value){
    String property_name = p_name;
    if(property_name == "name"){
        name = p_value;
    }
    else if(property_name == "symbol"){
        symbol = p_value;
    }
    else if(property_name == "uri"){
        uri = p_value;
    }
    else if(property_name == "seller_fee_basis_points"){
        seller_fee_basis_points = (uint32_t(p_value));
    }
    else if(property_name == "enable_creators"){
        enable_creators = p_value;
        notify_property_list_changed();
    }
    else if(property_name == "enable_collection"){
        enable_collection = p_value;
        notify_property_list_changed();
    }
    else if(property_name == "enable_uses"){
        enable_uses = p_value;
        notify_property_list_changed();
    }
    else if(property_name == "enable_collection_details"){
        enable_collection_details = p_value;
        notify_property_list_changed();
    }
    else if(property_name == "creators"){
        creators = p_value;
    }
    else if(property_name == "collection"){
        collection = p_value;
    }
    else if(property_name == "uses"){
        uses = p_value;
    }
    else if(property_name == "collection_size"){
        collection_size = p_value;
    }
    else{
        return false;
    }
    return true;
}

bool CreateMetaDataArgs::_get(const StringName &p_name, Variant &r_ret) const{
    String property_name = p_name;
    if(property_name == "name"){
        r_ret = name;
    }
    else if(property_name == "symbol"){
        r_ret = symbol;
    }
    else if(property_name == "uri"){
        r_ret = uri;
    }
    else if(property_name == "seller_fee_basis_points"){
        r_ret = seller_fee_basis_points;
    }
    else if(property_name == "enable_creators"){
        r_ret = enable_creators;
    }
    else if(property_name == "enable_collection"){
        r_ret = enable_collection;
    }
    else if(property_name == "enable_uses"){
        r_ret = enable_uses;
    }
    else if(property_name == "enable_collection_details"){
        r_ret = enable_collection_details;
    }
    else if(property_name == "creators"){
        r_ret = creators;
    }
    else if(property_name == "collection"){
        r_ret = collection;
    }
    else if(property_name == "uses"){
        r_ret = uses;
    }
    else if(property_name == "collection_size"){
        r_ret = collection_size;
    }
    else{
        return false;
    }
    return true;
}

void CreateMetaDataArgs::_get_property_list(List<PropertyInfo> *p_list) const{
    p_list->push_back(PropertyInfo(Variant::STRING, "name"));
    p_list->push_back(PropertyInfo(Variant::STRING, "symbol"));
    p_list->push_back(PropertyInfo(Variant::STRING, "uri"));

    p_list->push_back(PropertyInfo(Variant::INT, "seller_fee_basis_points"));

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_creators"));
    if(enable_creators){
	    p_list->push_back(PropertyInfo(Variant::ARRAY, "creators", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("MetaDataCreator")));
    }
    
    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_collection"));
	if(enable_collection){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "collection", PROPERTY_HINT_RESOURCE_TYPE, "MetaDataCollection"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_uses"));
    if(enable_uses){
        p_list->push_back(PropertyInfo(Variant::OBJECT, "uses", PROPERTY_HINT_RESOURCE_TYPE, "MetaDataUses"));
    }

    p_list->push_back(PropertyInfo(Variant::BOOL, "enable_collection_details"));
    if(enable_collection_details){
        p_list->push_back(PropertyInfo(Variant::INT, "collection_size", PROPERTY_HINT_NONE));
    }
    
}

void CreateMetaDataArgs::set_token_name(const String& token_name){
    name = token_name;
}
String CreateMetaDataArgs::get_token_name(){
    return name;
}
void CreateMetaDataArgs::set_symbol(const String& symbol){
    this->symbol = symbol;
}
String CreateMetaDataArgs::get_symbol(){
    return symbol;
}
void CreateMetaDataArgs::set_uri(const String& uri){
    this->uri = uri;
}
String CreateMetaDataArgs::get_uri(){
    return uri;
}
void CreateMetaDataArgs::set_seller_fee_basis_points(const uint16_t seller_fee_basis_points){
    this->seller_fee_basis_points = seller_fee_basis_points;
}
uint16_t CreateMetaDataArgs::get_seller_fee_basis_points(){
    return seller_fee_basis_points;
}
/*
void set_edition_nounce();
Variant get_edition_nounce();
void set_token_standard();
Variant get_token_standard();
void set_collection_details();
Variant get_collection_details();
void programmable_config();
Variant get_programmable_config();*/

void CreateMetaDataArgs::add_creator(const Variant& creator){
    creators.append(creator);
}

Array CreateMetaDataArgs::get_creators(){
    return creators;
}

Variant CreateMetaDataArgs::get_collection(){
    return collection;
}

void CreateMetaDataArgs::set_collection(const Variant& collection){
    this->collection = collection;
}

PackedByteArray CreateMetaDataArgs::serialize(const bool is_mutable) const{
    PackedByteArray res;
    const uint32_t DATA_LENGTH = 14 + name.length() + symbol.length() + uri.length();
    res.resize(DATA_LENGTH);

    res.encode_u32(0, name.length());
    res.encode_u32(4 + name.length(), symbol.length());
    res.encode_u32(8 + name.length() + symbol.length(), uri.length());
    for(unsigned int i = 0; i < name.length(); i++){
        res[4 + i] = name.to_ascii_buffer()[i];
    }
    for(unsigned int i = 0; i < symbol.length(); i++){
        res[8 + name.length() + i] = symbol.to_ascii_buffer()[i];
    }
    for(unsigned int i = 0; i < uri.length(); i++){
        res[12 + name.length() + symbol.length() + i] = uri.to_ascii_buffer()[i];
    }

    uint32_t data_location = 12 + name.length() + symbol.length() + uri.length();

    res.encode_u16(data_location, seller_fee_basis_points);

    res.append((int) (enable_creators && !creators.is_empty()));
    if(enable_creators && !creators.is_empty()){
        PackedByteArray serialized_creators;
        serialized_creators.resize(4);
        serialized_creators.encode_u32(0, creators.size());
        for(unsigned int i = 0; i < creators.size(); i++){
            const MetaDataCreator *creators_ptr = Object::cast_to<MetaDataCreator>(creators[i]);
            const PackedByteArray serialized_collection = creators_ptr->serialize();
            serialized_creators.append_array(serialized_collection);
        }
        res.append_array(serialized_creators);
    }

    res.append((int) enable_collection);
    if(enable_collection){
        const MetaDataCollection *collection_ptr = Object::cast_to<MetaDataCollection>(collection);
        const PackedByteArray serialized_collection = collection_ptr->serialize();
        res.append_array(serialized_collection);
    }

    res.append((int) enable_uses);
    if(enable_uses){
        const MetaDataUses *uses_ptr = Object::cast_to<MetaDataUses>(uses);
        const PackedByteArray serialized_collection = uses_ptr->serialize();
        res.append_array(serialized_collection);
    }

    res.append((int) is_mutable);

    res.append((int) enable_collection_details);
    if(enable_collection_details){
        res.append(0);
        PackedByteArray collection_size_data;
        collection_size_data.resize(8);
        collection_size_data.encode_u64(0, collection_size);
        res.append_array(collection_size_data);
    }

    return res;
}

}
