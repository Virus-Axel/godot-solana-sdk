#include "spl_token.hpp"
#include "mpl_token_metadata.hpp"

const std::string TokenProgram::ID = "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA";

void TokenProgram::_bind_methods(){
    ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram::initialize_mint);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram::initialize_account);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram::mint_to);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram::transfer_checked);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "owner_pubkey", "freeze_authority"), &TokenProgram::freeze_account);

    ClassDB::bind_static_method("TokenProgram", D_METHOD("new_token_record_address", "omner", "mint"), &TokenProgram::new_token_record_address);
    ClassDB::bind_static_method("TokenProgram", D_METHOD("get_pid"), &TokenProgram::get_pid);
}

// TODO(Virax): Data is unexplained. Fix this

Variant TokenProgram::new_token_record_address(const Variant &token, const Variant &mint){
    Array seeds;

    seeds.append(String("metadata").to_ascii_buffer());
    seeds.append(Pubkey(MplTokenMetadata::get_pid()).to_bytes());
    seeds.append(Pubkey(mint).to_bytes());
    seeds.append(String("token_record").to_ascii_buffer());
    seeds.append(Pubkey(token).to_bytes());

    return Pubkey::new_pda_bytes(seeds, MplTokenMetadata::get_pid());
}

Variant TokenProgram::new_delegate_record_address(const Variant& update_authority, const Variant &mint, const Variant& delegate_address, const MetaDataDelegateRole role){
    Array seeds;
    seeds.append(String("metadata").to_ascii_buffer());
    seeds.append(Pubkey(MplTokenMetadata::get_pid()).to_bytes());
    seeds.append(Pubkey(mint).to_bytes());

    // TODO(Virax): Change based on delegate role.
    seeds.append(String("collection_delegate").to_ascii_buffer());
    seeds.append(Pubkey(update_authority).to_bytes());
    seeds.append(Pubkey(delegate_address).to_bytes());

    return Pubkey::new_pda_bytes(seeds, MplTokenMetadata::get_pid());
}

Variant TokenProgram::initialize_mint(const Variant& mint_pubkey, const Variant& mint_authority, const Variant& freeze_authority, const uint32_t decimals){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    if(freeze_authority.get_type() == Variant::OBJECT){
        data.resize(67);

        data[34] = 1;
        PackedByteArray mint_authority_bytes = Pubkey(mint_authority).to_bytes();

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

    PackedByteArray mint_authority_bytes = Pubkey(mint_authority).to_bytes();

    for(unsigned int i = 0; i < 32; i++){
        data[2 + i] = mint_authority_bytes[i];
    }

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_pubkey, false, true));

    Pubkey *rent = memnew(Pubkey);
    rent->from_string("SysvarRent111111111111111111111111111111111");
    result->append_meta(AccountMeta(rent, false, false));

    return result;
}

Variant TokenProgram::initialize_account(const Variant& account_pubkey, const Variant& mint_pubkey, const Variant& owner_pubkey){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.resize(33);
    data[0] = 18;

    PackedByteArray owner_bytes = Pubkey(owner_pubkey).to_bytes();
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

    PackedByteArray owner_bytes = Pubkey(owner_pubkey).to_bytes();

    const Variant new_pid = Pubkey::new_from_string(String(ID.c_str()));
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_pubkey, false, true));
    result->append_meta(AccountMeta(account_pubkey, false, true));
    result->append_meta(AccountMeta(owner_pubkey, false, true));
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