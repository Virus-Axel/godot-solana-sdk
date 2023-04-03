#ifndef _SOLANA_SDK_RUST_WRAPPER
#define _SOLANA_SDK_RUST_WRAPPER

#include <cstdint>

extern "C"{

    /*
        Description:
        Appends an account to the accounts vector
        
        key_pointer:
        A pointer to a null terminated string containing the base58
        encoded public key of the account.

        Returns:
        An integer indicating the status of the operation
    */
    int append_account_w(const char* key_pointer);

    /*
        Description:
        Appends a keypair to the signers vector
        
        priv_key_pointer:
        A pointer to a null terminated string containing the base58
        encoded private key of the signing keypair.

        pub_key_pointer:
        A pointer to a null terminated string containing the base58
        encoded public key of the signing keypair.

        Returns:
        An integer indicating the status of the operation
    */
    int append_signer_w(const char* priv_key_pointer, const char* pub_key_pointer);

    void *create_unique_pubkey();
    void *create_pubkey_from_array(const unsigned char* data);
    void print_pubkey(const void*);
    void free_pubkey(const void*);

    void *create_keypair();
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
    void free_transaction(const void* transaction);
}

#endif