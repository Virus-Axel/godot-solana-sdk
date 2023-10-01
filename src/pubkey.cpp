#include "pubkey.hpp"
#include "utils.hpp"

#include <solana_sdk.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "blake3.h"
#include "sha256.hpp"

#include "curve25519.hpp"
#include <iostream>
#include <string.h>

using internal::gdextension_interface_print_warning;

namespace godot{

void Pubkey::_update_pointer(){

}

void Pubkey::_free_pointer(){
    //free_pubkey(data_pointer);
}

bool Pubkey::are_bytes_curve_point() const{
    return is_y_point_valid(bytes.ptr());
}

void Pubkey::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_value"), &Pubkey::get_value);
    ClassDB::bind_method(D_METHOD("set_value", "p_value"), &Pubkey::set_value);
    ClassDB::bind_method(D_METHOD("get_bytes"), &Pubkey::get_bytes);
    ClassDB::bind_method(D_METHOD("set_bytes", "p_value"), &Pubkey::set_bytes);
    ClassDB::bind_method(D_METHOD("get_seed"), &Pubkey::get_seed);
    ClassDB::bind_method(D_METHOD("set_seed", "p_value"), &Pubkey::set_seed);
    ClassDB::bind_method(D_METHOD("get_type"), &Pubkey::get_type);
    ClassDB::bind_method(D_METHOD("set_type", "p_value"), &Pubkey::set_type);
    ClassDB::bind_method(D_METHOD("get_base"), &Pubkey::get_base);
    ClassDB::bind_method(D_METHOD("set_base", "p_value"), &Pubkey::set_base);
    ClassDB::bind_method(D_METHOD("get_owner"), &Pubkey::get_owner);
    ClassDB::bind_method(D_METHOD("set_owner", "p_value"), &Pubkey::set_owner);
    ClassDB::bind_method(D_METHOD("get_wallet_address"), &Pubkey::get_wallet_address);
    ClassDB::bind_method(D_METHOD("set_wallet_address", "p_value"), &Pubkey::set_wallet_address);
    ClassDB::bind_method(D_METHOD("get_token_mint_address"), &Pubkey::get_token_mint_address);
    ClassDB::bind_method(D_METHOD("set_token_mint_address", "p_value"), &Pubkey::set_token_mint_address);

    
    ClassDB::bind_method(D_METHOD("create_from_string", "from"), &Pubkey::create_from_string);
    ClassDB::bind_method(D_METHOD("create_program_address", "seeds", "program_id"), &Pubkey::create_program_address);
    ClassDB::bind_method(D_METHOD("create_with_seed", "basePubkey", "seed", "owner_pubkey"), &Pubkey::create_with_seed);
    ClassDB::bind_method(D_METHOD("get_associated_token_address", "wallet_pubkey", "token_mint_pubkey"), &Pubkey::get_associated_token_address);

}


bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "value") {
		set_value(p_value);
	}
    else if(name == "bytes"){
        set_bytes(p_value);
    }
    else if(name == "seed"){
        set_seed(p_value);
    }
    else if(name == "type"){
        set_type(p_value);
    }
    else if(name == "base"){
        set_base(p_value);
    }
    else if(name == "owner"){
        set_owner(p_value);
    }
    else if (name == "wallet_address"){
        set_wallet_address(p_value);
    }
    else if (name == "token_mint_address"){
        set_token_mint_address(p_value);
    }
    else{
	    return false;
    }
    return true;
}

bool Pubkey::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "value") {
		r_ret = value;
	}
    else if (name == "bytes") {
		r_ret = bytes;
	}
    else if (name == "seed"){
        r_ret = seed;
    }
    else if (name == "type"){
        r_ret = type;
    }
    else if (name == "base"){
        r_ret = base;
    }
    else if (name == "owner"){
        r_ret = owner;
    }
    else if (name == "wallet_address"){
        r_ret = wallet_address;
    }
    else if (name == "token_mint_address"){
        r_ret = token_mint_address;
    }
    else{
	    return false;
    }
    return true;
}

void Pubkey::set_value(const String& p_value){
    value = p_value;

    // Update bytes accordingly.
    PackedByteArray decoded_value = SolanaSDK::bs58_decode(value);
    bytes = decoded_value;

    // Print warnings if key length is bad.
    if(decoded_value.is_empty() && value.length() != 0){
        internal::gdextension_interface_print_warning("Value contains non-base58 characters", "_set", "pubkey.cpp", 18, false);
    }
    else if (decoded_value.size() != 32){
        internal::gdextension_interface_print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 21, false);
    }
}

String Pubkey::get_value(){
    if(type == "CUSTOM"){
        return value;
    }
    else{
        void* key = to_ptr();
        String ret = "";
        if (key == nullptr){
            internal::gdextension_interface_print_warning("Invalid Pubkey", "get_value", "pubkey.cpp", 167, false);
        }
        else{
            PackedByteArray byte_array;
            byte_array.resize(32);
            unsigned char key_bytes[32];
            //get_pubkey_bytes(key, key_bytes);
            for(int i = 0; i < 32; i++){
                byte_array[i] = key_bytes[i];
            }
            ret = SolanaSDK::bs58_encode(byte_array);
        }
        return ret;
    }
}

void Pubkey::set_seed(const String& p_value){
    seed = p_value;
}

String Pubkey::get_seed(){
    return seed;
}

void Pubkey::set_bytes(const PackedByteArray& p_value){
    bytes = p_value;

    // Do not feed zero bytes into encode function.
    if (bytes.size() == 0){
        value = "";
    }
    else{
        String encoded_value = SolanaSDK::bs58_encode(bytes);
        value = encoded_value;
    }

    // Print warnings if byte length is bad.
    if (bytes.size() != 32){
        internal::gdextension_interface_print_warning("Pubkey must be 32 bytes", "_set", "pubkey.cpp", 14, false);
    }
    
}
PackedByteArray Pubkey::get_bytes() const{
    return bytes;
}

void Pubkey::set_type(const String p_value){
    type = p_value;
    notify_property_list_changed();
}

String Pubkey::get_type(){
    return type;
}

void Pubkey::set_base(const Variant p_value){
    base = p_value;
}
Variant Pubkey::get_base(){
    return base;
}

void Pubkey::set_owner(const Variant p_value){
    owner = p_value;
}
Variant Pubkey::get_owner(){
    return owner;
}

void Pubkey::set_wallet_address(const Variant p_value){
    wallet_address = p_value;
}
Variant Pubkey::get_wallet_address(){
    return wallet_address;
}

void Pubkey::set_token_mint_address(const Variant p_value){
    token_mint_address = p_value;
}
Variant Pubkey::get_token_mint_address(){
    return token_mint_address;
}

void Pubkey::create_from_string(const String& from){
    bytes = SolanaSDK::bs58_decode(from);
}

void Pubkey::create_from_array(const unsigned char* data){
    bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        bytes[i] = data[i];
    }
}

void Pubkey::create_with_seed(Variant basePubkey, String seed, Variant owner_pubkey){

    Object *base_pubkey_cast = basePubkey;
    Pubkey *base_pubkey_type = Object::cast_to<Pubkey>(base_pubkey_cast);

    Object *owner_pubkey_cast = basePubkey;
    Pubkey *owner_pubkey_type = Object::cast_to<Pubkey>(owner_pubkey_cast);
 
    std::cout << base_pubkey_type->get_bytes().size() << std::endl;
    std::cout << owner_pubkey_type->get_bytes().size() << std::endl;
    std::cout << seed.length() << std::endl;
    for(int i = 0; i < 32; i++){
        std::cout << (int)owner_pubkey_type->get_bytes().ptr()[i] << std::endl;
    }

    SHA256 hasher;

    hasher.update(base_pubkey_type->get_bytes().ptr(), base_pubkey_type->get_bytes().size());
    hasher.update(seed.to_utf8_buffer().ptr(), seed.length());
    hasher.update(owner_pubkey_type->get_bytes().ptr(), owner_pubkey_type->get_bytes().size());

    //blake3_hasher_update(&hasher, base_pubkey_type->get_bytes().ptr(), base_pubkey_type->get_bytes().size());
    //blake3_hasher_update(&hasher, seed.to_utf8_buffer().ptr(), seed.length());
    //blake3_hasher_update(&hasher, owner_pubkey_type->get_bytes().ptr(), owner_pubkey_type->get_bytes().size());

    uint8_t *sha256_hash = hasher.digest();

    //blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);

    bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        std::cout << (int) sha256_hash[i] << std::endl;
        bytes[i] = sha256_hash[i];
    }
    delete[] sha256_hash;
}

bool Pubkey::create_program_address(const PackedStringArray seeds, const Variant &program_id){
    // Perform seeds checks.
    if(seeds.size() > MAX_SEEDS){
        return false;
    }
    for(unsigned int i = 0; i < seeds.size(); i++){
        if(seeds[i].length() > MAX_SEED_LEN){
            return false;
        }
    }

    // Create the hash from seeds.
    //blake3_hasher hasher;
    //blake3_hasher_init(&hasher);
    SHA256 hasher;
 
    for(unsigned int i = 0; i < seeds.size(); i++){
        hasher.update(seeds[i].to_utf8_buffer().ptr(), seeds[i].length());
    }

    // Include program ID and PDA marker in hash.
    Object *program_id_cast = program_id;
    Pubkey *program_id_type = Object::cast_to<Pubkey>(program_id_cast);
    
    hasher.update((*program_id_type).get_bytes().ptr(), (*program_id_type).get_bytes().size());
    hasher.update(PDA_MARKER, 21);

    uint8_t hash[PUBKEY_BYTES];
    uint8_t *hash_ptr;
    hash_ptr = hasher.digest();
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        hash[i] = hash_ptr[i];
    }

    // Remove this memory ASAP.
    delete[] hash_ptr;
    
    //blake3_hasher_finalize(&hasher, hash, PUBKEY_BYTES);

    if(is_y_point_valid(hash)){
        return false;
    }

    bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        bytes[i] = hash[i];
    }
    return true;
}

bool Pubkey::get_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address){
    PackedStringArray arr;
    arr.append(wallet_address);
    arr.append(token_mint_address);
    arr.append(String(SolanaSDK::SPL_TOKEN_ADDRESS.c_str()));

    String pid = String(SolanaSDK::SPL_ASSOCIATED_TOKEN_ADDRESS.c_str());

    return create_program_address(arr, (Variant*) &pid);
}

void Pubkey::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags seed_visibility = PROPERTY_USAGE_NO_EDITOR;
    PropertyUsageFlags custom_visibility = PROPERTY_USAGE_NO_EDITOR;
    PropertyUsageFlags atoken_visibility = PROPERTY_USAGE_NO_EDITOR;

    p_list->push_back(PropertyInfo(Variant::STRING, "type", PROPERTY_HINT_ENUM, "UNIQUE,SEED,CUSTOM,ASSOCIATED_TOKEN"));

    if(type == "SEED"){
        seed_visibility = PROPERTY_USAGE_DEFAULT;
    }
    else if(type == "CUSTOM"){
        custom_visibility = PROPERTY_USAGE_DEFAULT;
    }
    else if(type == "ASSOCIATED_TOKEN"){
        atoken_visibility = PROPERTY_USAGE_DEFAULT;
    }

    p_list->push_back(PropertyInfo(Variant::STRING, "seed", PROPERTY_HINT_NONE, "", seed_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "base", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "owner", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", seed_visibility, ""));
	p_list->push_back(PropertyInfo(Variant::STRING, "value", PROPERTY_HINT_NONE, "", custom_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes", PROPERTY_HINT_NONE, "", custom_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "wallet_address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", atoken_visibility, ""));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "token_mint_address", PROPERTY_HINT_RESOURCE_TYPE, "Pubkey", atoken_visibility, ""));
}

Pubkey::Pubkey() {
}

Pubkey::Pubkey(const String& from){
    create_from_string(from);
}

Pubkey::~Pubkey() {
    _free_pointer_if_not_null();
}

}