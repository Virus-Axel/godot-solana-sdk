#ifndef SOLANA_SDK_MPL_CANDY_MACHINE
#define SOLANA_SDK_MPL_CANDY_MACHINE

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/node.hpp>
#include <account_meta.hpp>
#include <solana_client.hpp>

namespace godot{

class CandyGuardAccessList;

class ConfigLine: public Resource{
    GDCLASS(ConfigLine, Resource)
private:
    String name;
    String uri;
protected:
    static void _bind_methods();
public:
    ConfigLine();

    void set_name(const String &value);
    String get_name();

    void set_uri(const String &value);
    String get_uri();

    PackedByteArray serialize();
    ~ConfigLine();
};


class ConfigLineSetting : public Resource{
    GDCLASS(ConfigLineSetting, Resource)
private:
    String prefix_name;
    String prefix_uri;
    uint32_t name_length = 0;
    uint32_t uri_length = 0;
    bool is_sequential = false;
protected:
    static void _bind_methods();
public:
    ConfigLineSetting();

    void set_prefix_name(const String &value);
    String get_prefix_name();

    void set_name_length(const uint32_t value);
    uint32_t get_name_length();

    void set_prefix_uri(const String &value);
    String get_prefix_uri();

    void set_uri_length(const uint32_t value);
    uint32_t get_uri_length();

    void set_is_sequential(const bool value);
    bool get_is_sequential();

    PackedByteArray serialize();
    ~ConfigLineSetting();
};

class CandyMachineData : public Resource{
    GDCLASS(CandyMachineData, Resource)
private:
    uint32_t token_standard = 0;
    PackedByteArray features;
    Variant authority;
    Variant mint_authority;
    Variant collection_mint;
    uint64_t items_redeemed = 0;

    uint64_t items_available = 0;
    String symbol;
    uint32_t seller_fee_basis_points = 0;
    uint64_t max_supply = 0;
    bool is_mutable = false;
    Array creators;
    
    Variant config_line_setting = nullptr;
    Variant hidden_settings = nullptr;

    unsigned int get_config_line_size();

protected:
    static void _bind_methods();
public:

    void set_token_standard(const uint32_t value);
    uint32_t get_token_standard();

    void set_features(const PackedByteArray& value);
    PackedByteArray get_features();

    void set_authority(const Variant& value);
    Variant get_authority();

    void set_mint_authority(const Variant& value);
    Variant get_mint_authority();

    void set_items_available(const uint64_t value);
    uint64_t get_items_available();

    void set_collection_mint(const Variant& value);
    Variant get_collection_mint();

    void set_items_redeemed(const uint64_t value);
    uint64_t get_items_redeemed();

    void set_symbol(const String& value);
    String get_symbol();

    void set_seller_fee_basis_points(const uint32_t value);
    uint32_t get_seller_fee_basis_points();

    void set_max_supply(const uint64_t value);
    uint64_t get_max_supply();

    void set_is_mutable(const bool value);
    bool get_is_mutable();

    void set_creators(const Array &value);
    Array get_creators();

    void set_config_line_setting(const Variant& config_line_setting);
    Variant get_config_line_setting();

    PackedByteArray serialize();
    unsigned int get_space_for_candy(); 
};

class CandyGuardAccessList: public Resource{
    GDCLASS(CandyGuardAccessList, Resource)
private:
    String label = "default";
    bool top_level = true;

    bool enable_address_gate = false;
    Variant address_gate_reciever;

    bool enable_allocation = false;
    uint32_t allocation_id = 0;
    uint32_t allocation_size = 0;

    bool enable_allow_list = false;
    PackedByteArray allowed_merkle_root;

    bool enable_bot_tax = false;
    uint64_t bot_tax_lamports = 0;
    bool validate_last_instruction = false;

    bool enable_end_date = false;
    int64_t end_date = 0;

    bool enable_freeze_sol_payment = false;
    uint64_t freeze_amount_lamports = 0;
    Variant freeze_sol_destination;

    bool enable_freeze_token_payment = false;
    uint64_t amount = 0;
    Variant mint;
    Variant freeze_token_destination_ata;

    bool enable_gatekeeper = false;
    Variant gatekeeper_network;
    bool expire_on_use = false;

    bool enable_mint_limit = false;
    uint32_t limit_id = 0;
    uint32_t mint_limit = 0;

    bool enable_nft_burn = false;
    Variant required_nft_burn_collection;

    bool enable_nft_gate = false;
    Variant required_nft_gate_collection;

    bool enable_nft_payment = false;
    Variant required_nft_payment_collection;
    Variant nft_payment_destination;

    bool enable_program_gate = false;
    Array program_gate_addresses;

    bool enable_redeem_account = false;
    uint64_t max_redeem_amount = 0;

    bool enable_sol_payment = false;
    uint64_t sol_payment_lamports = 0;
    Variant sol_payment_destination;

    bool enable_start_date = false;
    int64_t start_date = 0;

    bool enable_third_party_signer = false;
    Variant signer_key;

    bool enable_token_burn = false;
    uint64_t token_burn_amount = 0;
    Variant token_burn_mint;

    bool enable_token_gate = false;
    uint64_t token_gate_amount = 0;
    Variant token_gate_mint;

    bool enable_token_payment = false;
    uint64_t token_payment_amount = 0;
    Variant token_payment_mint;
    Variant token_payment_destination;

    TypedArray<CandyGuardAccessList> groups;

    
bool _set(const StringName &p_name, const Variant &p_value);
bool _get(const StringName &p_name, Variant &r_ret) const;
void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
    static void _bind_methods();

public:
    PackedByteArray serialize() const;
    PackedByteArray serialize_mint_settings() const;
    PackedByteArray serialize_groups() const;
    PackedByteArray serialize_guard_settings() const;

    CandyGuardAccessList& get_group(const String &label);
    TypedArray<AccountMeta> get_mint_arg_accounts(const Variant& payer);
};

class MplCandyMachine : public SolanaClient{
    GDCLASS(MplCandyMachine, SolanaClient)
private:

protected:
    static void _bind_methods();

public:
    MplCandyMachine();

    static PackedByteArray mint_discriminator();
    static PackedByteArray mint2_discriminator();
    static PackedByteArray initialize_discriminator();
    static PackedByteArray initialize2_discriminator();
    static PackedByteArray add_config_lines_discriminator();

    static const std::string ID;

    static Variant initialize(
        const Variant &authority,
        const Variant &candy_machine_account,
        const Variant &collection_mint,
        const Variant &candy_machine_data,
        bool pnft = false);

    static Variant mint(
        const Variant &payer,
        const Variant &receiver,
        const Variant &mint,
        const Variant &collection_mint,
        const Variant &collection_update_authority,
        const Variant &candy_machine_key);

    static Variant add_config_lines(
        const Variant &candy_machine_key,
        const Variant &authority,
        const Array &config_lines,
        const unsigned int index
    );

    static Variant new_candy_machine_authority_pda(const Variant& candy_machine_key);
    void get_candy_machine_info(const Variant& candy_machine_key);
    void fetch_account_callback(const Dictionary& params);
    static Variant get_pid();
};

class MplCandyGuard : public Node{
    GDCLASS(MplCandyGuard, Node)
private:
    static PackedByteArray serialize_label(const String &label);

protected:
    static void _bind_methods();

public:
    static const std::string ID;

    static Variant initialize(const Variant& owner, const Variant& candy_guard_authority, const Variant& payer, const Variant &candy_guard_acl);

    static Variant mint(
        const Variant &candy_machine_id,
        const Variant &guard_account_id,
        const Variant &payer,
        const Variant &receiver,
        const Variant &mint,
        const Variant &nft_mint_authority,
        const Variant &collection_mint,
        const Variant &collection_update_authority,
        const Variant &candy_guard_acl,
        const String &label);

    static Variant get_pid();
    static Variant new_associated_candy_guard_key(const Variant &candy_machine_key);
};

}

#endif