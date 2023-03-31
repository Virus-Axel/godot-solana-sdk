#include "solana_sdk.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void SolanaSDK::_bind_methods() {
    ClassDB::bind_method(D_METHOD("append_account", "account_key"), &SolanaSDK::append_account);
}

SolanaSDK::SolanaSDK() {
    // initialize any variables here
}

Error SolanaSDK::append_account(String account_key){
    std::cout << "hi" << std::endl;
    return (Error) append_account_w(account_key.utf8().get_data());
}

SolanaSDK::~SolanaSDK() {
    // add your cleanup here
}
