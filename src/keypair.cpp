#include "keypair.hpp"
#include "xed25519.h"
#include "osrng.h"

#include <godot_cpp/core/class_db.hpp>
#include <solana_sdk.hpp>

namespace godot{

void Keypair::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_public_value"), &Keypair::get_public_value);
    ClassDB::bind_method(D_METHOD("set_public_value", "p_value"), &Keypair::set_public_value);
    ClassDB::bind_method(D_METHOD("get_public_bytes"), &Keypair::get_public_bytes);
    ClassDB::bind_method(D_METHOD("set_public_bytes", "p_value"), &Keypair::set_public_bytes);
    ClassDB::bind_method(D_METHOD("get_private_value"), &Keypair::get_private_value);
    ClassDB::bind_method(D_METHOD("set_private_value", "p_value"), &Keypair::set_private_value);
    ClassDB::bind_method(D_METHOD("get_private_bytes"), &Keypair::get_private_bytes);
    ClassDB::bind_method(D_METHOD("set_private_bytes", "p_value"), &Keypair::set_private_bytes);
    ClassDB::bind_method(D_METHOD("get_unique"), &Keypair::get_unique);
    ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Keypair::set_unique);
}

void Keypair::_free_pointer(){
    //free_keypair(data_pointer);
}
void Keypair::_update_pointer(){
    
}

void Keypair::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;

    // Hide other properties if unique is true.
    if(unique){
        visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "public_value", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "public_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::STRING, "private_value", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "private_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

bool Keypair::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "public_value") {
		set_public_value(p_value);
		return true;
	}
    else if(name == "public_bytes"){
        set_public_bytes(p_value);
		return true;
    }
    else if (name == "private_value") {
		set_private_value(p_value);
		return true;
	}
    else if(name == "private_bytes"){
        set_private_bytes(p_value);
		return true;
    }
    else if(name == "unique"){
        set_unique(p_value);
        return true;
    }
	return false;
}

bool Keypair::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "public_value") {
		r_ret = public_value;
		return true;
	}
    else if(name == "public_bytes"){
        r_ret = public_bytes;
		return true;
    }
    else if (name == "private_value") {
		r_ret = private_value;
		return true;
	}
    else if(name == "private_bytes"){
        r_ret = private_bytes;
		return true;
    }
    else if (name == "unique") {
		r_ret = unique;
		return true;
	}
	return false;
}

Keypair::Keypair() {
    CryptoPP::AutoSeededRandomPool prng;
    signer.AccessPrivateKey().GenerateRandom(prng);
        
    const CryptoPP::ed25519PrivateKey& privKey = dynamic_cast<const CryptoPP::ed25519PrivateKey&>(signer.GetPrivateKey());
    
    private_bytes.resize(CryptoPP::ed25519PrivateKey::SECRET_KEYLENGTH);
    const CryptoPP::byte *private_byte_pointer = privKey.GetPrivateKeyBytePtr();
    for(unsigned int i = 0; i < CryptoPP::ed25519PrivateKey::SECRET_KEYLENGTH; i++){
        private_bytes[i] = private_byte_pointer[i];
    }
    verifier = CryptoPP::ed25519::Verifier(signer);

    const CryptoPP::ed25519PublicKey& pubKey = dynamic_cast<const CryptoPP::ed25519PublicKey&>(verifier.GetPublicKey());
    const CryptoPP::byte *public_byte_pointer = pubKey.GetPublicKeyBytePtr();
    public_bytes.resize(CryptoPP::ed25519PublicKey::PUBLIC_KEYLENGTH);
    for(unsigned int i = 0; i < CryptoPP::ed25519PublicKey::PUBLIC_KEYLENGTH; i++){
        public_bytes[i] = public_byte_pointer[i];
    }

    private_value = SolanaSDK::bs58_encode(private_bytes);

    std::cout << "privvalue: " << private_value.to_ascii_buffer().ptr() << std::endl;
    public_value = SolanaSDK::bs58_encode(public_bytes);
    std::cout << "privvalue: " << public_value.to_ascii_buffer().ptr() << std::endl;
}

void Keypair::set_public_value(const String& p_value){
    public_value = p_value;

    // Keypair is not unique anymore.
    unique = false;

    // Update public bytes accordingly.
    PackedByteArray decoded_value = SolanaSDK::bs58_decode(public_value);
    public_bytes = decoded_value;

    // Print warnings if key length is bad.
    if(decoded_value.is_empty() && public_value.length() != 0){
        internal::gdextension_interface_print_warning("Value contains non-base58 characters", "_set", "keypair.cpp", 119, false);
    }
    else if (decoded_value.size() != KEY_LENGTH){
        internal::gdextension_interface_print_warning("Public key must be 32 bytes", "_set", "keypair.cpp", 122, false);
    }
}

String Keypair::get_public_value(){
    return public_value;
}

void Keypair::set_public_bytes(const PackedByteArray& p_value){
    public_bytes = p_value;

    // Keypair is not unique.
    unique = false;

    // Do not feed empty value to encode function.
    if (public_bytes.size() == 0){
        public_value = "";
    }
    else{
        String encoded_value = SolanaSDK::bs58_encode(public_bytes);
        public_value = encoded_value;
    }

    // Print warning if key length is bad.
    if (public_bytes.size() != KEY_LENGTH){
        internal::gdextension_interface_print_warning("Public key must be 32 bytes", "_set", "pubkey.cpp", 147, false);
    }
}
PackedByteArray Keypair::get_public_bytes(){
    return public_bytes;
}

void Keypair::set_private_value(const String& p_value){
    private_value = p_value;
    unique = false;

    // Update private bytes accordingly.
    PackedByteArray decoded_value = SolanaSDK::bs58_decode(private_value);
    private_bytes = decoded_value;

    // Print warnings if key length is bad.
    if(decoded_value.is_empty() && private_value.length() != 0){
        internal::gdextension_interface_print_warning("Value contains non-base58 characters", "_set", "keypair.cpp", 164, false);
    }
    else if (decoded_value.size() != KEY_LENGTH){
        internal::gdextension_interface_print_warning("Private key must be 32 bytes", "_set", "keypair.cpp", 167, false);
    }
}

String Keypair::get_private_value(){
    return private_value;
}

void Keypair::set_private_bytes(const PackedByteArray& p_value){
    private_bytes = p_value;
    unique = false;

    // Do not feed 0 bytes to encode algorithm.
    if (private_bytes.size() == 0){
        private_value = "";
    }
    else{
        String encoded_value = SolanaSDK::bs58_encode(private_bytes);
        private_value = encoded_value;
    }

    // Print warnings if key length is bad.
    if (private_bytes.size() != KEY_LENGTH){
        internal::gdextension_interface_print_warning("Private key must be 32 bytes", "_set", "keypair.cpp", 150, false);
    }
}

PackedByteArray Keypair::get_private_bytes(){
    return private_bytes;
}

void Keypair::set_unique(const bool p_value){
    unique = p_value;
    notify_property_list_changed();
}

bool Keypair::get_unique(){
    return unique;
}

Keypair::~Keypair() {
    _free_pointer_if_not_null();
}

}