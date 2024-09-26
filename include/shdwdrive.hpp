#ifndef GODOT_SOLANA_DSK_SHDW_DRIVE_HPP
#define GODOT_SOLANA_DSK_SHDW_DRIVE_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "solana_client.hpp"

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

public:
    void from_bytes(const PackedByteArray& bytes);
};

class UserInfo: public Resource{
GDCLASS(UserInfo, Resource)
private:
    unsigned int account_counter = 0;
    unsigned int delete_counter = 0;
    bool agreed_to_terms = false;
    bool had_bad_scam_scan = false;

    static PackedByteArray discriminator();

public:
    void from_bytes(const PackedByteArray& bytes);
};

class ShdwDrive : public Node {
GDCLASS(ShdwDrive, Node)
private:
    static const std::string ShdwDrive::ID;

    UserInfo* user_info = nullptr;
    StorageAccountV2* storage_account = nullptr;

    SolanaClient *fetch_user_account_client;

    void fetch_userinfo_callback(const Dictionary& params);
    void fetch_storage_account_callback(const Dictionary& params);
    uint64_t human_size_to_bytes(const String& human_size);
protected:
    static void _bind_methods();
public:
    Variant fetch_user_info(const Variant address);
    Variant fetch_storage_account(const Variant address);
    Variant create_storage_account(const Variant& owner_keypair, const String& name, const String& size);

    void send_create_storage_tx();

    static Variant new_user_info_pubkey(const Variant& base_keypair);

    static Variant get_pid();
};

} // godot


#endif