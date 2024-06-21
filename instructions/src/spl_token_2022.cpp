#include <spl_token_2022.hpp>

const std::string TokenProgram2022::ID = "TokenzQdBNbLqP5VEhdkAS6EPFLC1PHnBqCXEpPxuEb";

PackedByteArray TokenProgram2022::metadata_update_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {0xdd, 0xe9, 0x31, 0x2d, 0xb5, 0xca, 0xdc, 0xc8};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}
PackedByteArray TokenProgram2022::metadata_initialize_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {0xd2, 0xe1, 0x1e, 0xa2, 0x58, 0xb8, 0x4d, 0x8d};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void TokenProgram2022::_bind_methods(){
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_mint", "mint_pubkey", "mint_authority", "freeze_authority=null", "decimals=9"), &TokenProgram2022::initialize_mint);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_account", "account_pubkey", "mint_pubkey", "owner_pubkey"), &TokenProgram2022::initialize_account);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("mint_to", "mint_pubkey", "account_pubkey", "owner_pubkey", "mint_authority", "amount"), &TokenProgram2022::mint_to);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("transfer_checked", "source_pubkey", "mint_pubkey", "destination_pubkey", "source_authority", "amount", "decimals"), &TokenProgram2022::transfer_checked);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("freeze_account", "account_pubkey", "mint_pubkey", "owner_pubkey", "freeze_authority"), &TokenProgram2022::freeze_account);

    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("get_pid"), &TokenProgram2022::get_pid);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_metadata_pointer", "payer", "mint_keypair"), &TokenProgram2022::initialize_metadata_pointer);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("initialize_metadata", "payer", "mint_keypair", "name", "symbol", "uri"), &TokenProgram2022::initialize_metadata);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("update_metadata", "payer", "mint_keypair", "field", "value"), &TokenProgram2022::update_metadata);
    ClassDB::bind_static_method("TokenProgram2022", D_METHOD("get_mint_account_size_from_data", "name", "symbol", "uri", "custom_data"), &TokenProgram2022::get_mint_account_size_from_data);
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

Variant TokenProgram2022::initialize_metadata_pointer(const Variant& payer, const Variant& mint_keypair){
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

Variant TokenProgram2022::initialize_metadata(const Variant& payer, const Variant& mint_keypair, const String& name, const String& symbol, const String& uri){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.append_array(metadata_initialize_discriminator());
    
    PackedByteArray temp_array;
    temp_array.resize(4);
    temp_array.encode_u32(0, name.length());
    data.append_array(temp_array);
    data.append_array(name.to_ascii_buffer());

    temp_array.encode_u32(0, symbol.length());
    data.append_array(temp_array);
    data.append_array(symbol.to_ascii_buffer());

    temp_array.encode_u32(0, uri.length());
    data.append_array(temp_array);
    data.append_array(uri.to_ascii_buffer());

    const Variant new_pid = get_pid();
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_keypair, false, true));
    result->append_meta(AccountMeta(payer, true, false));
    result->append_meta(AccountMeta(mint_keypair, false, true));
    result->append_meta(AccountMeta(payer, true, false));

    return result;
}

Variant TokenProgram2022::update_metadata(const Variant& payer, const Variant& mint_keypair, const String& field, const String& value){
    Instruction *result = memnew(Instruction);
    PackedByteArray data;
    
    data.append_array(metadata_update_discriminator());
    
    PackedByteArray temp_array;
    temp_array.resize(4);

    if(field == "name"){
        data.append(0);
    }
    else if(field == "symbol"){
        data.append(1);
    }
    else if(field == "uri"){
        data.append(2);
    }
    else{
        // Custom field.
        data.append(3);

        temp_array.encode_u32(0, field.length());
        data.append_array(temp_array);
        data.append_array(field.to_ascii_buffer());
    }

    temp_array.encode_u32(0, value.length());
    data.append_array(temp_array);
    data.append_array(value.to_ascii_buffer());

    //data.append_array(Pubkey(mint_keypair).to_bytes());

    const Variant new_pid = get_pid();
    result->set_program_id(new_pid);
    result->set_data(data);

    result->append_meta(AccountMeta(mint_keypair, false, true));
    result->append_meta(AccountMeta(payer, true, false));

    return result;
}

unsigned int TokenProgram2022::get_mint_account_size_from_data(const String& name, const String symbol, const String uri, const Dictionary custom_data){
    const unsigned int LENGTH_BYTES = 4;
    const unsigned int AMOUNT_OF_REQUIRED_FIELDS = 3;
    const unsigned int EMPTY_ACCOUNT_SIZE = 302;

    unsigned int ret = EMPTY_ACCOUNT_SIZE;

    ret +=  AMOUNT_OF_REQUIRED_FIELDS * LENGTH_BYTES;
    ret += name.length();
    ret += symbol.length();
    ret += uri.length();

    if(!custom_data.is_empty()){
        ret += LENGTH_BYTES;
        for(unsigned int i = 0; i < custom_data.size(); i++){
            ret += 2 * LENGTH_BYTES;
            ret += String(custom_data.keys()[i]).length();
            ret += String(custom_data.values()[i]).length();
        }
    }

    return ret;
}