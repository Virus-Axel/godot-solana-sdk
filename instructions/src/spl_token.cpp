#include "spl_token.hpp"

const std::string TokenProgram::ID = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";

void TokenProgram::_bind_methods(){
    ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram::initialize_mint);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram::initialize_account);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram::mint_to);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram::transfer_checked);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "owner_pubkey", "freeze_authority"), &TokenProgram::freeze_account);
}

// TODO(Virax): Data is unexplained. Fix this

Variant TokenProgram::initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority, const uint32_t decimals){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    if(freeze_authority.get_type() == Variant::OBJECT){
        data.resize(67);

        data[34] = 1;
        PackedByteArray mint_authority_bytes = Pubkey(mint_authority).get_bytes();

        for(unsigned int i = 0; i < 32; i++){
            data[35 + i] = mint_authority_bytes[i];
        }
    }
    else{
        data.resize(35);
        data[34] = 0;
    }

    data[0] = 20;
    data[1] = decimals;

    PackedByteArray mint_authority_bytes = Pubkey(mint_authority).get_bytes();

    for(unsigned int i = 0; i < 32; i++){
        data[2 + i] = mint_authority_bytes[i];
    }

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_pubkey, false, true));

    Pubkey *rent = memnew(Pubkey);
    rent->set_value("SysvarRent111111111111111111111111111111111");
    result->append_meta(AccountMeta(rent, false, false));

    return result;
}

Variant TokenProgram::initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(33);
    data[0] = 18;

    PackedByteArray owner_bytes = Pubkey(owner_pubkey).get_bytes();
    for(unsigned int i = 0; i < 32; i++){
        data[1 + i] = owner_bytes[i];
    }

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(account_pubkey, false, true));
    result->append_meta(AccountMeta(mint_pubkey, false, false));

    return result;
}

Variant TokenProgram::mint_to(const Variant& mint_pubkey, const Variant& account_pubkey, const Variant& owner_pubkey, const Variant& mint_authority, uint64_t amount){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(9);
    data[0] = 7;
    data.encode_u64(1, amount);

    PackedByteArray owner_bytes = Pubkey(owner_pubkey).get_bytes();
    for(unsigned int i = 0; i < 32; i++){
        data[1 + i] = owner_bytes[i];
    }

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(account_pubkey, false, true));
    result->append_meta(AccountMeta(mint_pubkey, false, true));
    result->append_meta(AccountMeta(mint_authority, true, false));

    return result;
}

Variant TokenProgram::transfer_checked(const Variant& source_pubkey, const Variant& mint_pubkey, const Variant& destination_pubkey, const Variant& source_authority, uint64_t amount, uint32_t decimals){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(10);
    data[0] = 12;
    data.encode_u64(1, amount);
    data[9] = decimals;

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(source_pubkey, false, true));
    result->append_meta(AccountMeta(mint_pubkey, false, false));
    result->append_meta(AccountMeta(destination_pubkey, false, true));
    result->append_meta(AccountMeta(source_authority, true, false));

    return result;
}

Variant TokenProgram::freeze_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey, const Variant& freeze_authority){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(1);
    data[0] = 10;

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(account_pubkey, false, true));
    result->append_meta(AccountMeta(mint_pubkey, false, false));
    result->append_meta(AccountMeta(freeze_authority, true, false));

    return result;
}

Variant TokenProgram::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}