#include <spl_token_2022.hpp>

const std::string TokenProgram2022::ID = "TokenzQdBNbLqP5VEhdkAS6EPFLC1PHnBqCXEpPxuEb";

void TokenProgram2022::_bind_methods(){
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram2022::initialize_mint);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram2022::initialize_account);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram2022::mint_to);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram2022::transfer_checked);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "owner_pubkey", "freeze_authority"), &TokenProgram2022::freeze_account);

    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("get_pid"), &TokenProgram2022::get_pid);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_metadata", "payer", "mint_keypair"), &TokenProgram2022::initialize_metadata);
}

Variant TokenProgram2022::get_pid(){
    return Pubkey::new_from_string(ID.c_str()); 
}

Variant TokenProgram2022::initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority, const uint32_t decimals){
    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    return _initialize_mint(new_pid, mint_pubkey, mint_authority, freeze_authority, decimals);
}

Variant TokenProgram2022::initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey){
    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    return _initialize_account(new_pid, account_pubkey, mint_pubkey, owner_pubkey);
}

Variant TokenProgram2022::mint_to(const Variant& mint_pubkey, const Variant& account_pubkey, const Variant& owner_pubkey, const Variant& mint_authority, uint64_t amount){
    const Variant new_pid = Pubkey::new_from_string(String(ID.c_str()));
    return _mint_to(new_pid, mint_pubkey, account_pubkey, owner_pubkey, mint_authority, amount);
}

Variant TokenProgram2022::transfer_checked(const Variant& source_pubkey, const Variant& mint_pubkey, const Variant& destination_pubkey, const Variant& source_authority, uint64_t amount, uint32_t decimals){
    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    return _transfer_checked(new_pid, source_pubkey, mint_pubkey, destination_pubkey, source_authority, amount, decimals);
}

Variant TokenProgram2022::freeze_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey, const Variant& freeze_authority){
    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    return _freeze_account(new_pid, account_pubkey, mint_pubkey, owner_pubkey, freeze_authority);
}

Variant TokenProgram2022::initialize_metadata(const Variant& payer, const Variant& mint_keypair){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(2);
    const unsigned char DISCRIMINATOR = 39;
    data[0] = DISCRIMINATOR;
    data.append_array(Pubkey(payer).to_bytes());
    data.append_array(Pubkey(mint_keypair).to_bytes());

    const Variant new_pid = get_pid();
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_keypair, true, true));

    return result;
}