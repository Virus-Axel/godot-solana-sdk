#ifndef _SOLANA_SDK_RUST_WRAPPER
#define _SOLANA_SDK_RUST_WRAPPER

#include <cstdint>


extern "C"{
    unsigned int bs58_decode(const char* input, unsigned int input_length, char* result);
    unsigned int bs58_encode(const char* input, unsigned int input_length, char* result);

    void *create_unique_pubkey();
    void *create_pubkey_from_array(const unsigned char* data);
    void *create_pubkey_with_seed(const void* basePubkey, const char* seed, unsigned int seed_length, const void* owner_pubkey);
    void *create_associated_token_account(const void* basePubkey, const void* owner_pubkey);

    void get_pubkey_bytes(const void* key, unsigned char* bytes);

    void print_pubkey(const void*);
    void free_pubkey(const void*);

    void *create_unique_hash();
    void *create_hash_from_array(const unsigned char* data);
    void free_hash(const void*);

    void *create_keypair();
    void *create_keypair_from_bytes(const unsigned char* data);

    void get_keypair_bytes(const void* key, unsigned char* bytes);

    void free_keypair(const void*);

    void *create_account(uint64_t lamports, uint64_t space, const void* owner);
    void free_account(const void*);

    void *create_account_meta(const void* account_key, bool is_signer, bool writeable);
    void free_account_meta(const void* account_key);

    void *create_instruction_with_bytes(const void* program_id, unsigned char* data, int data_size, void**, int account_meta_length);
    void free_instruction(const void* instruction);

    void *create_transaction_signed_with_payer(void** instruction_array, int array_size, const void* payer, void** signers_array, int signers_array_size, const void* latest_blockhash);
    void *create_transaction_unsigned_with_payer(void** instruction_array, int array_size, const void* payer);
    int serialize_transaction(const void* transaction, unsigned char* buffer, const int buffer_size);
    int sign_transaction(const void* transaction, void** signer_array, int signer_size, const void* latest_blockhash);
    int partially_sign_transaction(const void* transaction, void** signer_array, int signer_size, const void* latest_blockhash);
    void free_transaction(const void* transaction);
}

#endif