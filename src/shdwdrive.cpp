#include "shdwdrive.hpp"

#include "solana_client.hpp"
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/json.hpp>
#include <pubkey.hpp>
#include <keypair.hpp>
#include <instruction.hpp>
#include <transaction.hpp>

#include "solana_utils.hpp"

#include <system_program.hpp>
#include <spl_token.hpp>
#include <godot_cpp/classes/http_request.hpp>

namespace godot{

const std::string ShdwDrive::ID = "2e1wdyNhUvE76y6yUCvah2KaviavMJYKoRun8acMRBZZ";


PackedByteArray StorageAccountV2::discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {133, 53, 253, 82, 212, 5, 201, 218};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void StorageAccountV2::_bind_methods(){

}

void StorageAccountV2::from_bytes(const PackedByteArray& bytes){
    ERR_FAIL_COND_EDMSG(bytes.slice(0, 8) == discriminator(), "Account is not StorageAccountV2.");

    int c = 8;
    immutable = bytes[c++];
    to_be_deleted = bytes[c++];
    delete_request_epoch = bytes.decode_u32(c);
    c += 4;
    storage = bytes.decode_u64(c);
    c += 8;
    owner1 = Pubkey::new_from_bytes(bytes.slice(c, c + 32));
    c += 32;
    account_counter_seed = bytes.decode_u32(c);
    c += 4;
    creation_time = bytes.decode_u32(c);
    c += 4;
    creation_epoch = bytes.decode_u32(c);
    c += 4;
    last_fee_epoch = bytes.decode_u32(c);
    c += 4;
    uint32_t identifier_size = bytes.decode_u32(c);
    c += 4;
    identifier = bytes.slice(c, c + identifier_size).get_string_from_ascii();
}

PackedByteArray UserInfo::discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {83, 134, 200, 56, 144, 56, 10, 62};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

void UserInfo::_bind_methods(){

}

uint32_t UserInfo::get_account_counter(){
    return account_counter;
}

void UserInfo::from_bytes(const PackedByteArray& bytes){
    ERR_FAIL_COND_EDMSG(bytes.slice(0, 8) == discriminator(), "Account is not UserInfo.");

    account_counter = bytes.decode_u32(8);
    delete_counter = bytes.decode_u32(12);
    agreed_to_terms = bytes[16] == 1;
    had_bad_scam_scan = bytes[17] == 1;
}

ShdwDrive::ShdwDrive(){
    user_info = memnew(UserInfo);
    api_request = memnew(RpcSingleHttpRequestClient);
    api_request->set_skip_id(true);

    create_storage_account_client = memnew(SolanaClient);
    create_storage_account_client->set_async_override(true);
    create_storage_account_transaction = memnew(Transaction);

    fetch_user_info_client = memnew(SolanaClient);
    fetch_user_info_client->set_async_override(true);
    Callable fetch_user_info_callable = Callable(this, "fetch_userinfo_callback");
    fetch_user_info_client->set_callback(fetch_user_info_callable);

    fetch_storage_account_client = memnew(SolanaClient);
    fetch_storage_account_client->set_async_override(true);
    Callable fetch_storage_account_callable = Callable(this, "fetch_storage_account_callback");
    fetch_storage_account_client->set_callback(fetch_storage_account_callable);
}

void ShdwDrive::_process(double delta){
    api_request->process(delta);
    create_storage_account_transaction->_process(delta);
    fetch_storage_account_client->_process(delta);
    fetch_user_info_client->_process(delta);
}

Variant ShdwDrive::fetch_user_info(const Variant address){
    UserInfo *user_info = memnew(UserInfo);
    
    fetch_user_info_client->get_account_info(Pubkey::string_from_variant(address));
    return OK;
}

Variant ShdwDrive::fetch_storage_account(const Variant address){
    StorageAccountV2 *storage_account = memnew(StorageAccountV2);

    fetch_storage_account_client->get_account_info(Pubkey::string_from_variant(address));
    return OK;
}

PackedByteArray ShdwDrive::initialize_accountv2_discriminator(){
    const unsigned char DISCRIMINATOR_ARRAY[] = {8, 182, 149, 144, 185, 31, 209, 105};
    PackedByteArray result;
    for (unsigned int i = 0; i < 8; i++){
        result.append(DISCRIMINATOR_ARRAY[i]);
    }
    return result;
}

uint64_t ShdwDrive::human_size_to_bytes(const String& human_size){
    return 0;
}

Variant ShdwDrive::get_uploader(){
    return Pubkey::new_from_string("972oJTFyjmVNsWM4GHEGPWUomAiJf2qrVotLtwnKmWem");
}

void ShdwDrive::fetch_userinfo_callback(const Dictionary& params){
    std::cout << "fetch user info callback entered" << std::endl;
    // TODO(Virax): Add helper for this common pattern.
    if(!params.has("result")){
        std::cout << JSON::stringify(params).ascii() << std::endl;
        ERR_PRINT_ED("Fetching user info account failed.");
        return;
    }
    if(!((Dictionary)params["result"]).has("value")){
        ERR_PRINT_ED("User is not init");
        return;
    }

    Dictionary account = ((Dictionary)params["result"])["value"];

    if(account.has("data")){
        const PackedByteArray data_info = account["data"];

        user_info->from_bytes(data_info);
    }
    else{
        new_user = true;
    }

    std::cout << "emitting user_info_fetched" << std::endl;
    emit_signal("user_info_fetched");
}

void ShdwDrive::fetch_storage_account_callback(const Dictionary& params){
    std::cout << "fetch storage account callback entered" << std::endl;
    // TODO(Virax): Add helper for this common pattern.
    if(!params.has("result")){
        ERR_PRINT_ED("Fetching storage account failed.");
        return;
    }
    if(!((Dictionary)params["result"]).has("value")){
        return;
    }

    Dictionary account = ((Dictionary)params["result"])["value"];
    const PackedByteArray data_info = account["data"];

    StorageAccountV2* storage_account = memnew(StorageAccountV2);
    storage_account->from_bytes(data_info);
    std::cout << "emitting" << std::endl;

    emit_signal("storage_account_fetched", storage_account);
}

void ShdwDrive::create_storage_call_api(const Variant& params){
    std::cout << "Finish" << std::endl;
}

void ShdwDrive::_bind_methods(){
    ClassDB::add_signal("ShdwDrive", MethodInfo("user_info_fetched", PropertyInfo(Variant::OBJECT, "user_info")));
    ClassDB::add_signal("ShdwDrive", MethodInfo("storage_account_fetched", PropertyInfo(Variant::OBJECT, "storage_account")));

    ClassDB::bind_method(D_METHOD("fetch_userinfo_callback", "params"), &ShdwDrive::fetch_userinfo_callback);
    ClassDB::bind_method(D_METHOD("fetch_storage_account_callback", "params"), &ShdwDrive::fetch_storage_account_callback);
    ClassDB::bind_method(D_METHOD("create_storage_call_api", "params"), &ShdwDrive::create_storage_call_api);
    ClassDB::bind_method(D_METHOD("create_storage_account", "owner_keypair", "storage_name", "storage_size"), &ShdwDrive::create_storage_account);
    ClassDB::bind_method(D_METHOD("send_create_storage_tx"), &ShdwDrive::send_create_storage_tx);
    ClassDB::bind_method(D_METHOD("send_create_storage_tx_signed"), &ShdwDrive::send_create_storage_tx_signed);
    ClassDB::bind_method(D_METHOD("create_store_api_response", "response"), &ShdwDrive::create_store_api_response);
}

Variant ShdwDrive::create_storage_account(const Variant& owner_keypair, const String& name, const String& size){
    this->owner_keypair = owner_keypair;
    this->storage_name = name;
    this->storage_size = human_size_to_bytes(size);

    Callable callback = Callable(this, "send_create_storage_tx");

    connect("user_info_fetched", callback, CONNECT_ONE_SHOT);
    connect("storage_account_fetched", callback, CONNECT_ONE_SHOT);

    fetch_user_info(new_user_info_pubkey(owner_keypair));
    //fetch_storage_account(owner_keypair);

    return nullptr;
}

void ShdwDrive::send_create_storage_tx(){
    std::cout << "send_create_storage_tx entered" << std::endl;
    ERR_FAIL_COND(user_info == nullptr && new_user == false);
    //ERR_FAIL_COND(storage_account == nullptr);

    Variant instruction = initialize_account(owner_keypair, "axel", 100);

    create_storage_account_transaction->set_payer(owner_keypair);
    create_storage_account_transaction->add_instruction(instruction);
    create_storage_account_transaction->update_latest_blockhash();

    Array partially_signers;
    partially_signers.append(owner_keypair);
    std::cout << "trying to sign " << std::endl;
    Callable callback = Callable(this, "send_create_storage_tx_signed");
    create_storage_account_transaction->connect("signer_state_changed", callback, CONNECT_ONE_SHOT);
    create_storage_account_transaction->partially_sign(partially_signers);
}

void ShdwDrive::send_create_storage_tx_signed(){
    PackedByteArray ser = create_storage_account_transaction->serialize();

    for(unsigned int i = 0; i < ser.size(); i++){
        std::cout << (int)ser[i] << ", ";
    }
    std::cout << std::endl;

    const String SHDW_DRIVE_ENDPOINT = "https://shadow-storage.genesysgo.net:443/storage-account";

    PackedStringArray http_headers;
    http_headers.append("Content-Type: application/json");
    http_headers.append("Accept-Encoding: json");

    Dictionary dict;
    dict["transaction"] = SolanaUtils::bs64_encode(ser);

    Dictionary url = SolanaClient::parse_url(SHDW_DRIVE_ENDPOINT);

    Callable callback = Callable(this, "create_store_api_response");
    api_request->asynchronous_request(dict, url, callback);
    //api_request->asynchronous_request(SHDW_DRIVE_ENDPOINT + String("/storage-account"), http_headers, HTTPClient::METHOD_POST, JSON::stringify(dict));
}

void ShdwDrive::create_store_api_response(const Dictionary& response){
    std::cout << JSON::stringify(response).ascii() << std::endl;
}

Variant ShdwDrive::new_user_info_pubkey(const Variant& base_keypair){
    Array seeds;
    seeds.append(String("user-info").to_ascii_buffer());
    seeds.append(Pubkey::bytes_from_variant(base_keypair));
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_storage_config_pubkey(){
    Array seeds;
    seeds.append(String("storage-config").to_ascii_buffer());
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_stake_account_pubkey(const Variant& storage_account){
    Array seeds;
    seeds.append(String("stake-account").to_ascii_buffer());
    seeds.append(Pubkey::bytes_from_variant(storage_account));
    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::new_storage_account_pubkey(const Variant& owner_key, uint64_t account_seed){
    Array seeds;
    seeds.append(String("storage-account").to_ascii_buffer());
    seeds.append(Pubkey::bytes_from_variant(owner_key));
    PackedByteArray account_seed_buffer;
    account_seed_buffer.resize(4);
    account_seed_buffer.encode_u32(0, account_seed);
    seeds.append(account_seed_buffer);

    return Pubkey::new_pda_bytes(seeds, get_pid());
}

Variant ShdwDrive::initialize_account(const Variant& owner_keypair, const String name, uint64_t storage){
    Instruction *result = memnew(Instruction);

    PackedByteArray data = initialize_accountv2_discriminator();
    data.resize(12);
    data.encode_u32(8, name.length());
    data.append_array(name.to_ascii_buffer());
    PackedByteArray storage_bytes;
    storage_bytes.resize(8);
    storage_bytes.encode_u64(0, storage);
    data.append_array(storage_bytes);

    const Variant new_pid = memnew(Pubkey(String(ID.c_str())));
    result->set_program_id(new_pid);
    result->set_data(data);

    const Variant STAKE_ACCOUNT = new_stake_account_pubkey(owner_keypair);
    const Variant TOKEN_MINT = Pubkey::new_from_string("SHDWyBxihqiCj6YekG2GUr7wqKLeLAMK1gHZck9pL6y");
    Variant ata_account = Pubkey::new_associated_token_address(owner_keypair, TOKEN_MINT);
    const Variant STORAGE_CONFIG = new_storage_config_pubkey();
    const Variant USER_INFO_PUBKEY = new_user_info_pubkey(owner_keypair);

    uint64_t account_seed = 0;
    if(!new_user){
        account_seed = user_info->get_account_counter();
    }
    const Variant STORAGE_ACCOUNT = new_storage_account_pubkey(owner_keypair, account_seed);
    const Variant UPLOADER = get_uploader();

    result->append_meta(*memnew(AccountMeta(STORAGE_CONFIG, false, true)));
    result->append_meta(*memnew(AccountMeta(USER_INFO_PUBKEY, false, true)));
    result->append_meta(*memnew(AccountMeta(STORAGE_ACCOUNT, false, true)));
    result->append_meta(*memnew(AccountMeta(STAKE_ACCOUNT, false, true)));
    result->append_meta(*memnew(AccountMeta(TOKEN_MINT, false, false)));
    result->append_meta(*memnew(AccountMeta(owner_keypair, true, true)));
    result->append_meta(*memnew(AccountMeta(UPLOADER, true, false)));
    result->append_meta(*memnew(AccountMeta(ata_account, false, true)));

    const Variant SYSTEM_PROGRAM_PID = SystemProgram::get_pid();
    const Variant TOKEN_PID = TokenProgram::get_pid();
    const Variant SYSVAR_RENT = Pubkey::new_from_string("SysvarRent111111111111111111111111111111111");

    result->append_meta(*memnew(AccountMeta(SYSTEM_PROGRAM_PID, false, false)));
    result->append_meta(*memnew(AccountMeta(TOKEN_PID, false, false)));
    result->append_meta(*memnew(AccountMeta(SYSVAR_RENT, false, false)));

    return result;
}

Variant ShdwDrive::get_pid(){
    return Pubkey::new_from_string(ID.c_str());
}

ShdwDrive::~ShdwDrive(){
    memfree(api_request);
    memfree(user_info);
    memfree(create_storage_account_client);
    memfree(create_storage_account_transaction);
    memfree(fetch_user_info_client);
    memfree(fetch_storage_account_client);
}

}