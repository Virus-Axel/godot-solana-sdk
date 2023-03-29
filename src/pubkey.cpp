#include "pubkey.hpp"
#include "utils.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Pubkey::_bind_methods() {
    ClassDB::bind_method(D_METHOD("from_encoded_string", "encoded_string"), &Pubkey::from_encoded_string);
    ClassDB::bind_method(D_METHOD("from_bytes", "encoded_string"), &Pubkey::from_bytes);
}

Pubkey::Pubkey() {
    // initialize any variables here
}

Error Pubkey::from_encoded_string(const String& encoded_string){
    std::cout << encoded_string.utf8().get_data() << std::endl;
    PackedByteArray arr = bs58_decode(encoded_string);

    return from_bytes(arr);
}

Error Pubkey::from_bytes(const PackedByteArray& bytes){
    if(bytes.size() != PUBKEY_LENGTH){
        return ERR_PARAMETER_RANGE_ERROR;
    }
    data = bytes;

    return OK;
}

Pubkey::~Pubkey() {
    // add your cleanup here
}
