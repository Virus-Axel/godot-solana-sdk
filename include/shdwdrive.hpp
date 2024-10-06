#ifndef GODOT_SOLANA_SDK_SHDW_DRIVE_HPP
#define GODOT_SOLANA_SDK_SHDW_DRIVE_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/http_request.hpp>
#include "solana_client.hpp"
#include "transaction.hpp"

namespace godot{

class StorageAccountV2: public Resource{
GDCLASS(StorageAccountV2, Resource)
private:
    bool immutable = false;
    bool to_be_deleted = false;
    uint32_t delete_request_epoch = 0;
    uint64_t storage = 0;
    Variant owner1;
    uint32_t account_counter_seed = 0;
    uint32_t creation_time = 0;
    uint32_t creation_epoch = 0;
    uint32_t last_fee_epoch = 0;
    String identifier = "";

    static PackedByteArray discriminator();

protected:
    static void _bind_methods();
public:
    void from_bytes(const PackedByteArray& bytes);
    String get_identifier();
    Dictionary to_dict();
};

class UserInfo: public Resource{
GDCLASS(UserInfo, Resource)
private:
    uint32_t account_counter = 0;
    uint32_t delete_counter = 0;
    bool agreed_to_terms = false;
    bool had_bad_scam_scan = false;

    static PackedByteArray discriminator();

protected:
    static void _bind_methods();

public:
    uint32_t get_account_counter();
    void from_bytes(const PackedByteArray& bytes);
};

class ShdwDrive : public Node {
GDCLASS(ShdwDrive, Node)
private:
    static const std::string ID;

    bool new_user = false;
    Variant owner_keypair;
    String storage_name;
    uint64_t storage_size;
    UserInfo* user_info = nullptr;
    StorageAccountV2* storage_account = nullptr;
    RpcSingleHttpRequestClient *api_request = nullptr;
    HTTPRequest *upload_file_request = nullptr;

    Array storage_list;
    PackedStringArray storage_name_list;
    PackedStringArray storage_key_list;

    SolanaClient *create_storage_account_client = nullptr;
    SolanaClient *fetch_user_info_client = nullptr;
    SolanaClient *fetch_storage_account_client = nullptr;
    SolanaClient *fetch_all_storage_accounts_client = nullptr;
    Transaction *create_storage_account_transaction = nullptr;

    static PackedByteArray initialize_accountv2_discriminator();
    static PackedByteArray create_form_line(const String& line);
    static PackedByteArray create_form_line(const PackedByteArray& content);
    static String get_upload_message(const Variant& storage_account_key, const String& filename_hash);
    static String get_filename_hash(const String& filename);

    void get_all_storage_accounts(const Variant& owner_key);
    void send_fetch_account_infos();

    void fetch_userinfo_callback(const Dictionary& params);
    void fetch_storage_account_callback(const Dictionary& params);
    void upload_file_callback(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body);

    uint64_t human_size_to_bytes(const String& human_size);

    Variant get_uploader();

protected:
    static void _bind_methods();
public:
    ShdwDrive();
    void _process(double delta) override;

    Array get_cached_storage_accounts();

    Variant fetch_user_info(const Variant address);
    Variant fetch_storage_account(const Variant address);
    Variant create_storage_account(const Variant& owner_keypair, const String& name, const Variant& size);

    Variant fetch_storage_key_by_name_callback(const String& storage_name);
    Variant fetch_storage_key_by_name(const Variant& owner_keypair, const String& storage_name);

    void send_create_storage_tx();
    void send_create_storage_tx_signed();

    void create_store_api_response(const Dictionary& response);
    void get_multiple_accounts_callback(const Dictionary& response);

    static Variant new_user_info_pubkey(const Variant& base_keypair);
    static Variant new_stake_account_pubkey(const Variant& storage_account);
    static Variant new_storage_config_pubkey();
    static Variant new_storage_account_pubkey(const Variant& owner_key, uint64_t account_seed);

    Variant initialize_account(const Variant& owner_keypair, const String name, uint64_t storage);
    void upload_file_to_storage(const String& filename, const Variant& storage_owner_keypair, const Variant& storage_account);

    static Variant get_pid();

    ~ShdwDrive();
};

} // godot


#endif