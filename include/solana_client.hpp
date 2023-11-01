#ifndef SOLANA_SDK_SOLANA_CLIENT_HPP
#define SOLANA_SDK_SOLANA_CLIENT_HPP

#include <godot_cpp/classes/node.hpp>

namespace godot {

class SolanaClient : public Node {
    GDCLASS(SolanaClient, Node)

private:
    static std::string url;
    static std::string commitment;
    static std::string encoding;
    static std::string transaction_detail;
    static std::string identity;
    static uint64_t min_context_slot;
    static uint64_t filter_offset;
    static uint64_t filter_length;
    static uint64_t max_transaction_version;
    static uint64_t first_slot;
    static uint64_t last_slot;
    static bool account_filter_enabled;
    static bool min_constext_slot_enabled;
    static bool max_transaction_version_enabled;
    static bool rewards;
    static bool slot_range_enabled;

    static void append_commitment(Array& options);
    static void append_min_context_slot(Array& options);
    static void append_encoding(Array& options);
    static void append_account_filter(Array& options);
    static void append_data_filter(Array& options);
    static void append_transaction_detail(Array& options);
    static void append_max_transaction_version(Array& options);
    static void append_rewards(Array& options);
    static void append_identity(Array& options);
    static void append_slot_range(Array& options);
    static void append_limit(Array& options);

    static void add_to_param_dict(Array &options, const String& key, const Variant& value);

    static Dictionary make_rpc_dict(const String& method, const Array& params);
    static Dictionary make_rpc_param(const Variant& key, const Variant& value);
    static Dictionary make_rpc_param(const Variant& key, const Dictionary& value);
    static Dictionary make_data_slice(uint64_t offset, uint64_t length);
    static Dictionary quick_http_request(const String& request_body);

protected:
    static void _bind_methods();

public:

    SolanaClient();

    static void set_url(const String& url);
    static void set_commitment(const String& commitment);
    static void set_encoding(const String& encoding);
    static void set_transaction_detail(const String& transaction_detail);

    static void enable_min_context_slot(int slot);
    static void disable_min_context_slot();

    static void enable_account_filter(uint64_t offset, uint64_t length);
    static void disable_account_filter();
    
    static void enable_max_transaction_version(uint64_t version);
    static void disable_max_transaction_version();

    static void enable_rewards();
    static void disable_rewards();

    static void enable_identity(const String& identity);
    static void disable_identity();

    static void enable_slot_range(uint64_t first, uint64_t last);
    static void disable_slot_range();

    static Dictionary get_account_info(const String& account);
    static Dictionary get_balance(const String& account);
    static Dictionary get_block(uint64_t slot, const String& detail);
    static Dictionary get_block_height();
    static Dictionary get_latest_blockhash();
    static Dictionary get_block_production();
    static Dictionary get_block_commitment(uint64_t slot_number);
    static Dictionary get_blocks(uint64_t start_slot, const Variant& end_slot = Variant::NIL);
    static Dictionary get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot);
    static Dictionary get_block_time(uint64_t slot);
    static Dictionary get_cluster_nodes();
    static Dictionary get_epoch_info();
    static Dictionary get_epoch_schedule();
    static Dictionary get_fee_for_message(const String& encoded_message);
    static Dictionary get_first_available_block();
    static Dictionary get_genesis_hash();
    static Dictionary get_health();
    static Dictionary get_highest_snapshot_slot();
    static Dictionary get_identity();
    static Dictionary get_inflation_governor();
    static Dictionary get_inflation_rate();
    static Dictionary get_inflation_reward(const PackedStringArray accounts, const Variant& epoch = Variant::NIL);
    static Dictionary get_largest_accounts(const String& filter = "");
    static Dictionary get_leader_schedule(const Variant& slot = Variant::NIL);
    static Dictionary get_max_retransmit_slot();
    static Dictionary get_max_shred_insert_slot();
    static Dictionary get_minimum_balance_for_rent_extemption(uint64_t data_size);
    static Dictionary get_multiple_accounts(const PackedStringArray accounts);
    static Dictionary get_program_accounts(const String& program_address, bool with_context = false);
    static Dictionary get_recent_performance_samples();
    static Dictionary get_recent_prioritization_fees(PackedStringArray account_addresses);
    static Dictionary get_signature_for_address(const String& address, const String& before = "", const String& until = "");
    static Dictionary get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history = false);
    static Dictionary get_slot();
    static Dictionary get_slot_leader();
    static Dictionary get_slot_leaders(const Variant& start_slot = Variant(), const Variant& slot_limit = Variant());
    static Dictionary get_stake_activation(const String& account);
    static Dictionary get_stake_minimum_delegation();
    static Dictionary get_supply(bool exclude_non_circulating = false);
    static Dictionary get_token_account_balance(const String& token_account);
    static Dictionary get_token_accounts_by_delegate(const String& account_delegate, const String &mint = "", const String& program_id = "");
    static Dictionary get_token_accounts_by_owner(const String& owner, const String &mint = "", const String& program_id = "");
    static Dictionary get_token_largest_account(const String& token_mint);
    static Dictionary get_token_supply(const String& token_mint);
    static Dictionary get_transaction(const String& signature);
    static Dictionary get_transaction_count();
    static Dictionary get_version();
    static Dictionary get_vote_accounts(const String& vote_pubkey = "", bool keep_unstaked_delinquents = false);
    static Dictionary is_blockhash_valid(const String& blockhash);
    static Dictionary minimum_ledger_slot();
    static Dictionary request_airdrop(const String& address, uint64_t lamports);
    static Dictionary send_transaction(const String& encoded_transaction, uint64_t max_retries = 10, bool skip_preflight = false);
    static Dictionary simulate_transaction(const String& encoded_transaction, bool sig_verify = false, bool replace_blockhash = false, Array account_addresses = Array(), const String& account_encoding = "base64");
    
    ~SolanaClient();
};
}

#endif