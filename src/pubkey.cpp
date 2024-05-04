#include "pubkey.hpp"
#include "utils.hpp"

#include <solana_utils.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include "sha256.hpp"

#include "curve25519.hpp"
#include "keypair.hpp"
#include "account_meta.hpp"
#include "wallet_adapter.hpp"
#include "spl_token.hpp"


namespace godot{

bool Pubkey::are_bytes_curve_point() const{
    return is_y_point_valid(bytes.ptr());
}

void Pubkey::_bind_methods() {
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_seed", "basePubkey", "seed", "owner_pubkey"), &Pubkey::new_from_seed);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_string", "from"), &Pubkey::new_from_string);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_from_bytes", "from"), &Pubkey::new_from_bytes);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_program_address", "seeds", "program_id"), &Pubkey::new_program_address);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_associated_token_address", "wallet_address", "token_mint_address"), &Pubkey::new_associated_token_address);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_pda", "seeds", "program_id"), &Pubkey::new_pda);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_pda_bytes", "seeds", "program_id"), &Pubkey::new_pda_bytes);
    ClassDB::bind_static_method("Pubkey", D_METHOD("new_random"), &Pubkey::new_random);

    ClassDB::bind_method(D_METHOD("to_string"), &Pubkey::to_string);
    ClassDB::bind_method(D_METHOD("set_string", "p_value"), &Pubkey::from_string);
    ClassDB::bind_method(D_METHOD("to_bytes"), &Pubkey::to_bytes);
    ClassDB::bind_method(D_METHOD("from_bytes", "p_value"), &Pubkey::from_bytes);
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
    ClassDB::bind_method(D_METHOD("get_associated_token_address", "wallet_pubkey", "token_mint_pubkey"), &Pubkey::get_associated_token_address);
}


bool Pubkey::_set(const StringName &p_name, const Variant &p_value) {
	String name = p_name;
	if (name == "value") {
		from_string(p_value);
	}
    else if(name == "bytes"){
        from_bytes(p_value);
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

void Pubkey::from_string(const String& p_value){
    value = p_value;

    // Update bytes accordingly.
    PackedByteArray decoded_value = SolanaUtils::bs58_decode(value);
    bytes = decoded_value;

    // Print warnings if key length is bad.
    ERR_FAIL_COND_EDMSG(decoded_value.is_empty() && (value.length() != 0), "Value contains non-base58 characters");
    if (decoded_value.size() != 32){
        Array params;
        params.push_back(decoded_value.size());
        ERR_PRINT_ED(String("Pubkey must be 32 bytes. It is {0}").format(params).utf8());
    }
}

String Pubkey::to_string() const{
    return value;
}

void Pubkey::set_seed(const String& p_value){
    seed = p_value;
}

String Pubkey::get_seed(){
    return seed;
}

void Pubkey::from_bytes(const PackedByteArray& p_value){
    bytes = p_value;

    // Do not feed zero bytes into encode function.
    if (bytes.size() == 0){
        value = "";
    }
    else{
        String encoded_value = SolanaUtils::bs58_encode(bytes);
        value = encoded_value;
    }

    // Print warnings if byte length is bad.
    ERR_FAIL_COND_EDMSG(bytes.size() != 32, "Pubkey must be 32 bytes");
}
PackedByteArray Pubkey::to_bytes() const{
    return bytes;
}

void Pubkey::set_type(const String p_value){
    type = p_value;
    notify_property_list_changed();
}

String Pubkey::get_type() const{
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
    bytes = SolanaUtils::bs58_decode(from);
}

void Pubkey::create_from_array(const unsigned char* data){
    bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        bytes[i] = data[i];
    }
}

Variant Pubkey::new_from_seed(Variant basePubkey, String seed, Variant owner_pubkey){
    Pubkey *res = memnew(Pubkey);
 
    SHA256 hasher;

    const Pubkey base = Pubkey(basePubkey);
    const Pubkey owner = Pubkey(owner_pubkey);

    hasher.update(base.to_bytes().ptr(), base.to_bytes().size());
    hasher.update(seed.to_utf8_buffer().ptr(), seed.length());
    hasher.update(owner.to_bytes().ptr(), owner.to_bytes().size());

    uint8_t *sha256_hash = hasher.digest();

    res->bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        res->bytes[i] = sha256_hash[i];
    }
    res->from_bytes(res->to_bytes());

    delete[] sha256_hash;

    return res;
}

Variant Pubkey::new_from_string(const String& from){
    Pubkey *res = memnew(Pubkey);
    res->from_string(from);
    return res;
}

Variant Pubkey::new_from_bytes(const PackedByteArray& from){
    Pubkey *res = memnew(Pubkey);
    res->from_bytes(from);
    return res;
}

Variant Pubkey::new_program_address(const PackedStringArray seeds, const Variant &program_id){
    Pubkey *res = memnew(Pubkey);
    res->create_program_address(seeds, program_id);
    return res;
}

Variant Pubkey::new_pda(const PackedStringArray seeds, const Variant &program_id){    
    TypedArray<PackedByteArray> arr;

    for(unsigned int i = 0; i < seeds.size(); i++){
        arr.append(seeds[i].to_ascii_buffer());
    }

    arr.append(PackedByteArray());
    
    Pubkey *res = memnew(Pubkey);
    for(uint8_t i = 255; i > 0; i--){
        PackedByteArray bump_seed;
        bump_seed.push_back(i);
        arr[arr.size() - 1] = bump_seed;
        if(res->create_program_address_bytes(arr, program_id)){
            return res;
        }
    }
    
    internal::gdextension_interface_print_warning("y points were not valid", "new_associated_token_address", __FILE__, __LINE__, false);
    return nullptr;
}

Variant Pubkey::new_pda_bytes(const Array seeds, const Variant &program_id){
    TypedArray<PackedByteArray> arr;

    for(unsigned int i = 0; i < seeds.size(); i++){
        arr.append(seeds[i]);
    }

    arr.append(PackedByteArray());
    
    Pubkey *res = memnew(Pubkey);
    for(uint8_t i = 255; i > 0; i--){
        PackedByteArray bump_seed;
        bump_seed.push_back(i);
        arr[arr.size() - 1] = bump_seed;
        if(res->create_program_address_bytes(arr, program_id)){
            return res;
        }
    }
    
    internal::gdextension_interface_print_warning("y points were not valid", "new_associated_token_address", __FILE__, __LINE__, false);
    return nullptr;
}

Variant Pubkey::new_random(){
    PackedByteArray random_bytes;
    random_bytes.resize(PUBKEY_BYTES);
    RandomNumberGenerator rand;
    rand.randomize();
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        random_bytes[i] = rand.randi() % 256;
    }
    return new_from_bytes(random_bytes);
}

bool Pubkey::is_pubkey(const Variant &object){
    return ((Object*)object)->is_class("Pubkey");
}

bool Pubkey::is_pubkey_compatible(const Variant &object){
    return (
        is_pubkey(object) ||
        Keypair::is_keypair(object) ||
        AccountMeta::is_account_meta(object) ||
        WalletAdapter::is_wallet_adapter(object)
    );
}

Variant Pubkey::new_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address){    
    TypedArray<PackedByteArray> arr;

    arr.append(Pubkey(wallet_address).to_bytes());
    arr.append(Object::cast_to<Pubkey>(TokenProgram::get_pid())->to_bytes());
    arr.append(Pubkey(token_mint_address).to_bytes());

    arr.append(PackedByteArray());

    String pid = String(SolanaUtils::SPL_ASSOCIATED_TOKEN_ADDRESS.c_str());

    Variant pid_key = Pubkey::new_from_string(pid);
    
    Pubkey *res = memnew(Pubkey);
    for(uint8_t i = 255; i > 0; i--){
        PackedByteArray bump_seed;
        bump_seed.push_back(i);
        arr[3] = bump_seed;
        if(res->create_program_address_bytes(arr, pid_key)){
            return res;
        }
    }
    
    ERR_PRINT_ED("y points were not valid");
    return nullptr;
}


bool Pubkey::create_program_address_bytes(const Array seeds, const Variant &program_id){
    ERR_FAIL_COND_V_EDMSG(!Pubkey::is_pubkey_compatible(program_id), false, "Program ID must be pubkey");

    // Perform seeds checks.
    ERR_FAIL_COND_V_EDMSG(seeds.size() > MAX_SEEDS, false, "Too many seeds");
    for(unsigned int i = 0; i < seeds.size(); i++){
        ERR_FAIL_COND_V_EDMSG(((PackedByteArray)seeds[i]).size() > MAX_SEED_LEN, false, "Seed is too long");
    }

    SHA256 hasher;
 
    for(unsigned int i = 0; i < seeds.size(); i++){
        hasher.update(((PackedByteArray)seeds[i]).ptr(), ((PackedByteArray)seeds[i]).size());
    }

    // Include program ID and PDA marker in hash.
    hasher.update(Pubkey(program_id).to_bytes().ptr(), Pubkey(program_id).to_bytes().size());
    hasher.update(PDA_MARKER, 21);

    uint8_t hash[PUBKEY_BYTES];
    uint8_t *hash_ptr;
    hash_ptr = hasher.digest();
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        hash[i] = hash_ptr[i];
    }

    // Remove this memory ASAP.
    delete[] hash_ptr;
    
    if(is_y_point_valid(hash)){
        return false;
    }

    PackedByteArray new_bytes;
    new_bytes.resize(PUBKEY_BYTES);
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        new_bytes[i] = hash[i];
    }
    from_bytes(new_bytes);

    return true;
}

bool Pubkey::create_program_address(const PackedStringArray seeds, const Variant &program_id){
    ERR_FAIL_COND_V_EDMSG(!Pubkey::is_pubkey_compatible(program_id), false, "Program ID must be pubkey");
    
    // Perform seeds checks.
    ERR_FAIL_COND_V_EDMSG(seeds.size() > MAX_SEEDS, false, "Too many seeds");
    for(unsigned int i = 0; i < seeds.size(); i++){
        ERR_FAIL_COND_V_EDMSG(seeds[i].length() > MAX_SEED_LEN, false, "Seed is too long");
    }

    SHA256 hasher;
 
    for(unsigned int i = 0; i < seeds.size(); i++){
        hasher.update(seeds[i].to_utf8_buffer().ptr(), seeds[i].length());
    }

    // Include program ID and PDA marker in hash.
    hasher.update(Pubkey(program_id).to_bytes().ptr(), Pubkey(program_id).to_bytes().size());
    hasher.update(PDA_MARKER, 21);

    uint8_t hash[PUBKEY_BYTES];
    uint8_t *hash_ptr;
    hash_ptr = hasher.digest();
    for(unsigned int i = 0; i < PUBKEY_BYTES; i++){
        hash[i] = hash_ptr[i];
    }

    // Remove this memory ASAP.
    delete[] hash_ptr;
    
    ERR_FAIL_COND_V_EDMSG(is_y_point_valid(hash), false, "y point is not valid");

    PackedByteArray new_bytes;
    new_bytes.resize(PUBKEY_BYTES);
    memcpy(new_bytes.ptrw(), hash, PUBKEY_BYTES);

    from_bytes(new_bytes);

    return true;
}

bool Pubkey::get_associated_token_address(const Variant &wallet_address, const Variant &token_mint_address){
    PackedStringArray arr;
    arr.append(wallet_address);
    arr.append(token_mint_address);
    arr.append(String(SolanaUtils::SPL_TOKEN_ADDRESS.c_str()));

    String pid = String(SolanaUtils::SPL_ASSOCIATED_TOKEN_ADDRESS.c_str());

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
    from_string(from);
}

Pubkey::Pubkey(const Variant& from){
    *this = from;
}

void Pubkey::operator=(const Variant& other){
    if(other.get_type() == Variant::STRING){
        from_string(other);
        return;
    }
    else if(other.get_type() == Variant::ARRAY){
        from_bytes(PackedByteArray(other));
        return;
    }
    else if(other.get_type() == Variant::PACKED_BYTE_ARRAY){
        from_bytes(other);
        return;
    }
    else if(other.get_type() != Variant::Type::OBJECT){
        ERR_PRINT_ED("Assigning pubkey with an non-object type.");
        return;
    }

    if(Pubkey::is_pubkey(other)){
        this->bytes = Object::cast_to<Pubkey>(other)->to_bytes();
        this->value = Object::cast_to<Pubkey>(other)->to_string();
        this->type = Object::cast_to<Pubkey>(other)->get_type();
    }
    else if(Keypair::is_keypair(other)){
        this->from_bytes(Object::cast_to<Keypair>(other)->get_public_bytes());
    }
    else if(AccountMeta::is_account_meta(other)){
        *this = Object::cast_to<AccountMeta>(other)->get_pubkey();
    }
    else if(WalletAdapter::is_wallet_adapter(other)){
        WalletAdapter *phantom_controller = Object::cast_to<WalletAdapter>(other);
        if(phantom_controller->is_connected()){
            *this = phantom_controller->get_connected_key();
        }
        else{
            // Append placeholder
            this->bytes.resize(32);
        }
    }

    else{
        ERR_PRINT_ED("Assigning pubkey with an unexpected object.");
    }
}

bool Pubkey::operator==(const Pubkey& other) const{
    return (bytes == other.bytes);
}

Pubkey::operator StringName() const{
    return to_string();
}

Pubkey::operator PackedByteArray() const{
    return to_bytes();
}

Pubkey::~Pubkey() {
}

}