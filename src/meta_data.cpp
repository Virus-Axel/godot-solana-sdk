#include "meta_data.hpp"

#include "utils.hpp"
#include "pubkey.hpp"

namespace godot{

void MetaDataCreator::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_address"), &MetaDataCreator::get_address);
    ClassDB::bind_method(D_METHOD("set_address", "p_value"), &MetaDataCreator::set_address);
    ClassDB::bind_method(D_METHOD("get_verified"), &MetaDataCreator::get_verified);
    ClassDB::bind_method(D_METHOD("set_verified", "p_value"), &MetaDataCreator::set_verified);
    ClassDB::bind_method(D_METHOD("get_share"), &MetaDataCreator::get_share);
    ClassDB::bind_method(D_METHOD("set_share", "p_value"), &MetaDataCreator::set_share);

    ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::OBJECT, "address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,PhantomController", PROPERTY_USAGE_DEFAULT), "set_address", "get_address");
    ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::BOOL, "verified", PROPERTY_HINT_NONE), "set_verified", "get_verified");
    ClassDB::add_property("MetaDataCreator", PropertyInfo(Variant::INT, "share", PROPERTY_HINT_NONE), "set_share", "get_share");
}

void MetaDataCreator::set_address(const Variant& p_value){
    address = p_value;
}
Variant MetaDataCreator::get_address(){
    return address;
}

void MetaDataCreator::set_verified(const bool p_value){
    verified = p_value;
}
bool MetaDataCreator::get_verified(){
    return verified;
}

void MetaDataCreator::set_share(const uint32_t p_value){
    share = p_value;
}
uint32_t MetaDataCreator::get_share(){
    return share;
}

PackedByteArray MetaDataCreator::serialize() const{
    PackedByteArray res;
    res.append_array(Pubkey(address).get_bytes());
    res.append(verified);
    res.append(share);

    return res;
}

void MetaDataCollection::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_key"), &MetaDataCollection::get_key);
    ClassDB::bind_method(D_METHOD("set_key", "p_value"), &MetaDataCollection::set_key);
    ClassDB::bind_method(D_METHOD("get_verified"), &MetaDataCollection::get_verified);
    ClassDB::bind_method(D_METHOD("set_verified", "p_value"), &MetaDataCollection::set_verified);

    ClassDB::add_property("MetaDataCollection", PropertyInfo(Variant::OBJECT, "key", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey,Keypair,PhantomController", PROPERTY_USAGE_DEFAULT), "set_key", "get_key");
    ClassDB::add_property("MetaDataCollection", PropertyInfo(Variant::BOOL, "verified", PROPERTY_HINT_NONE), "set_verified", "get_verified");
}

void MetaDataCollection::set_key(const Variant& p_value){
    key = p_value;
}
Variant MetaDataCollection::get_key(){
    return key;
}

void MetaDataCollection::set_verified(const bool p_value){
    verified = p_value;
}
bool MetaDataCollection::get_verified(){
    return verified;
}

PackedByteArray MetaDataCollection::serialize() const{
    PackedByteArray res;
    res.append(verified);
    res.append_array(Pubkey(key).get_bytes());

    return res;
}

void MetaDataUses::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_use_method"), &MetaDataUses::get_use_method);
    ClassDB::bind_method(D_METHOD("set_use_method", "p_value"), &MetaDataUses::set_use_method);
    ClassDB::bind_method(D_METHOD("get_total"), &MetaDataUses::get_total);
    ClassDB::bind_method(D_METHOD("set_total", "p_value"), &MetaDataUses::set_total);
    ClassDB::bind_method(D_METHOD("get_remaining"), &MetaDataUses::get_remaining);
    ClassDB::bind_method(D_METHOD("set_remaining", "p_value"), &MetaDataUses::set_remaining);

    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "use_method", PROPERTY_HINT_ENUM, "Burn,Multiple,Single", PROPERTY_USAGE_DEFAULT), "set_use_method", "get_use_method");
    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "total", PROPERTY_HINT_NONE), "set_total", "get_total");
    ClassDB::add_property("MetaDataUses", PropertyInfo(Variant::INT, "remaining", PROPERTY_HINT_NONE), "set_remaining", "get_remaining");
}

void MetaDataUses::set_use_method(const uint32_t p_value){
    use_method = (UseMethod)p_value;
}
uint32_t MetaDataUses::get_use_method(){
    return use_method;
}

void MetaDataUses::set_total(const uint64_t p_value){
    total = p_value;
}
uint64_t MetaDataUses::get_total(){
    return total;
}

void MetaDataUses::set_remaining(const uint64_t p_value){
    remaining = p_value;
}
uint64_t MetaDataUses::get_remaining(){
    return remaining;
}

PackedByteArray MetaDataUses::serialize() const{
    PackedByteArray res;
    res.resize(17);
    res[0] = use_method;
    res.encode_u64(1, remaining);
    res.encode_u64(9, total);

    return res;
}

void MetaData::_bind_methods(){
    ClassDB::bind_method(D_METHOD("get_token_name"), &MetaData::get_token_name);
    ClassDB::bind_method(D_METHOD("set_token_name", "token_name"), &MetaData::set_token_name);
    ClassDB::bind_method(D_METHOD("get_symbol"), &MetaData::get_symbol);
    ClassDB::bind_method(D_METHOD("set_symbol", "symbol"), &MetaData::set_symbol);
    ClassDB::bind_method(D_METHOD("get_uri"), &MetaData::get_uri);
    ClassDB::bind_method(D_METHOD("set_uri", "uri"), &MetaData::set_uri);
}

bool MetaData::_set(const StringName &p_name, const Variant &p_value){
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

bool MetaData::_get(const StringName &p_name, Variant &r_ret) const{
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

void MetaData::_get_property_list(List<PropertyInfo> *p_list) const{
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

void MetaData::set_token_name(const String& token_name){
    name = token_name;
}
String MetaData::get_token_name(){
    return name;
}
void MetaData::set_symbol(const String& symbol){
    this->symbol = symbol;
}
String MetaData::get_symbol(){
    return symbol;
}
void MetaData::set_uri(const String& uri){
    this->uri = uri;
}
String MetaData::get_uri(){
    return uri;
}
void MetaData::set_seller_fee_basis_points(const uint16_t seller_fee_basis_points){
    this->seller_fee_basis_points = seller_fee_basis_points;
}
uint16_t MetaData::get_seller_fee_basis_points(){
    return seller_fee_basis_points;
}

void MetaData::add_creator(const Variant& creator){
    creators.append(creator);
}

void MetaData::set_collection(const Variant& collection){
    this->collection = collection;
}

PackedByteArray MetaData::serialize(const bool is_mutable) const{
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
    if(enable_collection && !creators.is_empty()){
        PackedByteArray serialized_creators;
        serialized_creators.resize(4);
        serialized_creators.encode_u32(0, creators.size());
        for(unsigned int i = 0; i < creators.size(); i++){
            const MetaDataCreator *creators_ptr = Object::cast_to<MetaDataCreator>(creators[i]);
            const PackedByteArray serialized_collection = creators_ptr->serialize();
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