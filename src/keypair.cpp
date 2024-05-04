#include "keypair.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <solana_utils.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>

namespace godot{

void Keypair::_bind_methods() {
    ClassDB::bind_static_method("Keypair", D_METHOD("new_from_seed", "seed"), (Variant(*)(const PackedByteArray&))&Keypair::new_from_seed);
    ClassDB::bind_static_method("Keypair", D_METHOD("new_random"), &Keypair::new_random);
    ClassDB::bind_static_method("Keypair", D_METHOD("new_from_bytes", "bytes"), &Keypair::new_from_bytes);
    ClassDB::bind_static_method("Keypair", D_METHOD("new_from_file", "filename"), &Keypair::new_from_file);

    ClassDB::bind_method(D_METHOD("get_public_string"), &Keypair::get_public_string);
    ClassDB::bind_method(D_METHOD("set_public_string", "p_value"), &Keypair::set_public_string);
    ClassDB::bind_method(D_METHOD("get_public_bytes"), &Keypair::get_public_bytes);
    ClassDB::bind_method(D_METHOD("set_public_bytes", "p_value"), &Keypair::set_public_bytes);
    ClassDB::bind_method(D_METHOD("get_private_string"), &Keypair::get_private_string);
    ClassDB::bind_method(D_METHOD("set_private_string", "p_value"), &Keypair::set_private_string);
    ClassDB::bind_method(D_METHOD("get_private_bytes"), &Keypair::get_private_bytes);
    ClassDB::bind_method(D_METHOD("set_private_bytes", "p_value"), &Keypair::set_private_bytes);
    ClassDB::bind_method(D_METHOD("get_unique"), &Keypair::get_unique);
    ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &Keypair::set_unique);
    ClassDB::bind_method(D_METHOD("get_seed"), &Keypair::get_seed);
    ClassDB::bind_method(D_METHOD("set_seed", "p_value"), &Keypair::set_seed);

    ClassDB::bind_method(D_METHOD("sign_message", "message"), &Keypair::sign_message);
    ClassDB::bind_method(D_METHOD("save_to_file", "filename"), &Keypair::save_to_file);
    ClassDB::bind_method(D_METHOD("verify_signature", "signature", "message"), &Keypair::verify_signature);
}

void Keypair::_get_property_list(List<PropertyInfo> *p_list) const {
    PropertyUsageFlags visibility = PROPERTY_USAGE_DEFAULT;

    // Hide other properties if unique is true.
    if(unique){
        visibility = PROPERTY_USAGE_NO_EDITOR;
    }
    else{
        p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "seed"));
    }
    p_list->push_back(PropertyInfo(Variant::BOOL, "unique"));
	p_list->push_back(PropertyInfo(Variant::STRING, "public_string", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "public_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::STRING, "private_string", PROPERTY_HINT_NONE, "", visibility, ""));
    p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "private_bytes", PROPERTY_HINT_NONE, "", visibility, ""));
}

bool Keypair::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "public_string") {
		set_public_string(p_value);
		return true;
	}
    else if(name == "public_bytes"){
        set_public_bytes(p_value);
		return true;
    }
    else if (name == "private_string") {
		set_private_string(p_value);
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
    else if(name == "seed"){
        set_seed(p_value);
        return true;
    }
	return false;
}

bool Keypair::_get(const StringName &p_name, Variant &r_ret) const {
	String name = p_name;
	if (name == "public_string") {
		r_ret = public_string;
		return true;
	}
    else if(name == "public_bytes"){
        r_ret = public_bytes;
		return true;
    }
    else if (name == "private_string") {
		r_ret = private_string;
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
    else if (name == "seed") {
		r_ret = seed;
		return true;
	}
	return false;
}

Keypair::Keypair() {
    seed.resize(KEY_LENGTH);
    private_bytes.resize(KEY_LENGTH*2);
    public_bytes.resize(KEY_LENGTH);
    from_seed();
}

void Keypair::from_seed(){
    ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), seed.ptr());

    private_string = SolanaUtils::bs58_encode(private_bytes);
    public_string = SolanaUtils::bs58_encode(public_bytes);
}

void Keypair::save_to_file(const String &filename){
    const int64_t SEED_LENGTH = 32;
    ERR_FAIL_COND_EDMSG(seed.size() != SEED_LENGTH, "Keypair seed is not known, unable to store.");

    Ref<FileAccess> file = FileAccess::open(filename, FileAccess::WRITE);
    ERR_FAIL_COND_EDMSG(!file->is_open(), "Failed to open file " + filename);

    PackedByteArray bytes;

    bytes.append_array(get_seed());
    bytes.append_array(get_public_bytes());
    String line = JSON::stringify(bytes);
    file->store_line(line.replace("\"", ""));
    file->close();
}

void Keypair::random(){

    RandomNumberGenerator rand;
    rand.randomize();

    private_bytes.resize(KEY_LENGTH*2);
    public_bytes.resize(KEY_LENGTH);

    unsigned char random_seed[KEY_LENGTH];
    seed.resize(KEY_LENGTH);
    for(unsigned int i = 0; i < KEY_LENGTH; i++){
        random_seed[i] = rand.randi();
        seed[i] = random_seed[i];
    }

    ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), random_seed);

    private_string = SolanaUtils::bs58_encode(private_bytes);
    public_string = SolanaUtils::bs58_encode(public_bytes);
}

Keypair::Keypair(const PackedByteArray &seed){
    ERR_FAIL_COND_EDMSG(seed.size() != 32, "Seed must be 32 bytes");

    private_bytes.resize(KEY_LENGTH*2);
    public_bytes.resize(KEY_LENGTH);

    ed25519_create_keypair(public_bytes.ptrw(), private_bytes.ptrw(), seed.ptr());

    private_string = SolanaUtils::bs58_encode(private_bytes);
    public_string = SolanaUtils::bs58_encode(public_bytes);
}

Variant Keypair::new_from_seed(const String &seed){
    Keypair *res = memnew(Keypair);
    res->set_seed(seed.to_ascii_buffer());
    return res;
}

Variant Keypair::new_from_seed(const PackedByteArray &seed){
    Keypair *res = memnew(Keypair);
    res->set_seed(seed);
    return res;
}

Variant Keypair::new_from_bytes(const PackedByteArray &bytes){
    ERR_FAIL_COND_V_EDMSG(bytes.size() != 64, nullptr, "Expects 64 bytes input");

    return new_from_seed(bytes.slice(0, 32));
}

Variant Keypair::new_from_file(const String &filename){
    Ref<FileAccess> file = FileAccess::open(filename, FileAccess::READ);
    ERR_FAIL_COND_V_EDMSG(!file.is_valid(), nullptr, "Failed to open file " + filename);
    ERR_FAIL_COND_V_EDMSG(!file->is_open(), nullptr, "Failed to open file " + filename);

    String content = file->get_as_text();
    file->close();

    Array bytes = JSON::parse_string(content);

    return new_from_bytes(PackedByteArray(bytes));
}

Variant Keypair::new_random(){
    Keypair *res = memnew(Keypair);
    res->random();
    return res;
}

bool Keypair::is_keypair(const Variant& object){
    return ((Object*)object)->is_class("Keypair");
}

void Keypair::set_public_string(const String& p_value){
    public_string = p_value;

    // Keypair is not unique anymore.
    unique = false;

    // Update public bytes accordingly.
    PackedByteArray decoded_value = SolanaUtils::bs58_decode(public_string);
    public_bytes = decoded_value;

    // Print warnings if key length is bad.
    ERR_FAIL_COND_EDMSG((decoded_value.is_empty() && public_string.length() != 0), "Value contains non-base58 characters");
    ERR_FAIL_COND_EDMSG(decoded_value.size() != KEY_LENGTH, "Public key must be 32 bytes");
}

String Keypair::get_public_string(){
    return public_string;
}

void Keypair::set_public_bytes(const PackedByteArray& p_value){
    public_bytes = p_value;

    // Keypair is not unique.
    unique = false;

    // Do not feed empty value to encode function.
    if (public_bytes.size() == 0){
        public_string = "";
    }
    else{
        String encoded_value = SolanaUtils::bs58_encode(public_bytes);
        public_string = encoded_value;
    }

    // Print warning if key length is bad.
    ERR_FAIL_COND_EDMSG(public_bytes.size() != KEY_LENGTH, "Public key must be 32 bytes.");
}
PackedByteArray Keypair::get_public_bytes() const{
    return public_bytes;
}

void Keypair::set_private_string(const String& p_value){
    private_string = p_value;
    unique = false;

    // Update private bytes accordingly.
    PackedByteArray decoded_value = SolanaUtils::bs58_decode(private_string);
    private_bytes = decoded_value;

    // Print warnings if key length is bad.
    ERR_FAIL_COND_EDMSG(decoded_value.is_empty() && private_string.length() != 0, "Value contains non-base58 characters.");
    ERR_FAIL_COND_EDMSG(decoded_value.size() != KEY_LENGTH, "Private key must be 32 bytes.");
}

String Keypair::get_private_string(){
    return private_string;
}

void Keypair::set_private_bytes(const PackedByteArray& p_value){
    private_bytes = p_value;
    unique = false;

    // Do not feed 0 bytes to encode algorithm.
    if (private_bytes.size() == 0){
        private_string = "";
    }
    else{
        String encoded_value = SolanaUtils::bs58_encode(private_bytes);
        private_string = encoded_value;
    }

    // Print warnings if key length is bad.
    ERR_FAIL_COND_EDMSG(private_bytes.size() != KEY_LENGTH, "Private key must be 32 bytes.");
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

void Keypair::set_seed(const PackedByteArray &p_value){
    ERR_FAIL_COND_EDMSG(p_value.size() != 32, "Seed must be 32 bytes");

    seed = p_value;
    from_seed();
}

PackedByteArray Keypair::get_seed(){
    return seed;
}

PackedByteArray Keypair::sign_message(const PackedByteArray& message){
    unsigned char signature[64];

    ed25519_sign(signature, message.ptr(), message.size(), public_bytes.ptr(), private_bytes.ptr());

    PackedByteArray result;
    result.resize(SIGNATURE_LENGTH);
    for(int i = 0; i < SIGNATURE_LENGTH; i++){
        result[i] = signature[i];
    }

    return result;
}

bool Keypair::verify_signature(const PackedByteArray& signature, const PackedByteArray& message){
    return ed25519_verify(signature.ptr(), message.ptr(), message.size(), public_bytes.ptr()) == 1;
}

Keypair::~Keypair() {

}

}