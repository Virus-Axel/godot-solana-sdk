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