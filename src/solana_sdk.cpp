#include "solana_sdk.hpp"

#include "../wrapper/wrapper.h"

#include <godot_cpp/core/class_db.hpp>
#include <utils.hpp>

using namespace godot;

void SolanaSDK::_bind_methods() {
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_encode", "input"), &SolanaSDK::bs58_encode);
    ClassDB::bind_static_method("SolanaSDK", D_METHOD("bs58_decode", "input"), &SolanaSDK::bs58_decode);
}

SolanaSDK::SolanaSDK() {
    // initialize any variables here
}

PackedByteArray SolanaSDK::bs58_decode(String str){
	PackedByteArray result;
	char buffer[str.length() * 2];
	unsigned int resultlen = ::bs58_decode((const char*) str.to_utf8_buffer().ptr(), str.length(), buffer);
	result.resize(resultlen);
	for(unsigned int i = 0; i < resultlen; i++){
		result[i] = buffer[i];
	}
	return result;
}

String SolanaSDK::bs58_encode(PackedByteArray input){
	PackedByteArray result;
	char buffer[input.size() * 2];
	unsigned int resultlen = ::bs58_encode((const char*) input.ptr(), input.size(), buffer);
	result.resize(resultlen);
	for(unsigned int i = 0; i < resultlen; i++){
		result[i] = buffer[i];
	}
	return result.get_string_from_utf8();
}

SolanaSDK::~SolanaSDK() {
    // add your cleanup here
}
