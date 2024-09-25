#ifndef GODOT_SOLANA_DSK_SHDW_DRIVE_HPP
#define GODOT_SOLANA_DSK_SHDW_DRIVE_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include "solana_client.hpp"

namespace godot{

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
    SolanaClient *fetch_user_account_client;

    void fetch_userinfo_callback(const Dictionary& params);
    uint64_t human_size_to_bytes(const String& human_size);
protected:
    static void _bind_methods();
public:
    Variant fetch(const Variant address);
    Variant create_storage_account(const String& name, const String& size);
};

} // godot


#endif