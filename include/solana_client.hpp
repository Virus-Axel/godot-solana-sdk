#ifndef SOLANA_SDK_SOLANA_CLIENT_HPP
#define SOLANA_SDK_SOLANA_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

#include "rpc_multi_http_request_client.hpp"
#include "rpc_single_ws_request_client.hpp"

namespace godot {

class SolanaClient : public Node {
    GDCLASS(SolanaClient, Node)

private:
    float timeout = 20.0;

    uint32_t http_port_override = 0;
    uint32_t ws_port_override = 0;

    const std::string DEFAULT_URL = "https://api.devnet.solana.com";
    const std::string DEFAULT_WS_URL = "wss://api.devnet.solana.com";

    String url_override = "";
    String ws_url = "";
    
    bool use_tls = true;
    bool async_override = false;
    bool pending_request = false;

    String commitment = "confirmed";
    String encoding = "base64";
    std::string transaction_detail;
    std::string identity;
    uint64_t min_context_slot = 0;
    uint64_t filter_offset = 0;
    uint64_t filter_length = 0;
    uint64_t max_transaction_version = 0;
    uint64_t first_slot = 0;
    uint64_t last_slot = 0;
    bool account_filter_enabled = false;
    bool min_constext_slot_enabled = false;
    bool max_transaction_version_enabled = false;
    bool rewards = false;
    bool slot_range_enabled = false;

    Callable ws_callback;

    String ws_from_http(const String& http_url);
    String get_real_url();
    uint32_t get_real_http_port();
    uint32_t get_real_ws_port();
    String get_real_ws_url();

    WsRpcCall *ws_client();
    RpcMultiHttpRequestClient *http_client();

    void append_commitment(Array& options);
    void append_min_context_slot(Array& options);
    void append_encoding(Array& options);
    void append_account_filter(Array& options);
    void append_data_filter(Array& options);
    void append_transaction_detail(Array& options);
    void append_max_transaction_version(Array& options);
    void append_rewards(Array& options);
    void append_identity(Array& options);
    void append_slot_range(Array& options);
    void append_limit(Array& options);

    void add_to_param_dict(Array &options, const String& key, const Variant& value);

    Dictionary make_rpc_param(const Variant& key, const Variant& value);
    void quick_http_request(const Dictionary& request_body, const Callable& callback = Callable());
    Dictionary parse_url(const String& url);

    void response_callback(const Dictionary &params);
    void ws_response_callback(const Dictionary &params);

protected:
    static void _bind_methods();

public:
    static String assemble_url(const Dictionary& url_components);

    static unsigned int global_rpc_id;
    static unsigned int get_next_request_identifier();

    void _process(double delta) override;
    void _ready() override;

    static Dictionary make_rpc_dict(const String& method, const Array& params);

    SolanaClient();

    void set_url_override(const String& url);
    String get_url_override();

    void set_timeout(float timeout);
    float get_timeout();
    
    void set_ws_url(const String& url);
    String get_ws_url();

    void set_commitment(const String& commitment);
    String get_commitment();

    void set_encoding(const String& encoding);
    String get_encoding();

    void set_transaction_detail(const String& transaction_detail);

    void enable_min_context_slot(int slot);
    void disable_min_context_slot();

    void enable_account_filter(uint64_t offset, uint64_t length);
    void disable_account_filter();
    
    void enable_max_transaction_version(uint64_t version);
    void disable_max_transaction_version();

    void enable_rewards();
    void disable_rewards();

    void enable_identity(const String& identity);
    void disable_identity();

    void enable_slot_range(uint64_t first, uint64_t last);
    void disable_slot_range();

    void set_async_override(bool use_async);
    bool get_async_override();
    bool is_ready();

    void get_account_info(const String& account);
    void get_balance(const String& account);
    void get_block(uint64_t slot, const String& detail);
    void get_block_height();
    void get_latest_blockhash();
    void get_block_production();
    void get_block_commitment(uint64_t slot_number);
    void get_blocks(uint64_t start_slot, const Variant& end_slot = Variant::NIL);
    void get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot);
    void get_block_time(uint64_t slot);
    void get_cluster_nodes();
    void get_epoch_info();
    void get_epoch_schedule();
    void get_fee_for_message(const String& encoded_message);
    void get_first_available_block();
    void get_genesis_hash();
    void get_health();
    void get_highest_snapshot_slot();
    void get_identity();
    void get_inflation_governor();
    void get_inflation_rate();
    void get_inflation_reward(const PackedStringArray accounts, const Variant& epoch = Variant::NIL);
    void get_largest_accounts(const String& filter = "");
    void get_leader_schedule(const Variant& slot = Variant::NIL);
    void get_max_retransmit_slot();
    void get_max_shred_insert_slot();
    void get_minimum_balance_for_rent_extemption(uint64_t data_size);
    void get_multiple_accounts(const PackedStringArray accounts);
    void get_program_accounts(const String& program_address, bool with_context = false);
    void get_recent_performance_samples();
    void get_recent_prioritization_fees(PackedStringArray account_addresses);
    void get_signature_for_address(const String& address, const String& before = "", const String& until = "");
    void get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history = false);
    void get_slot();
    void get_slot_leader();
    void get_slot_leaders(const Variant& start_slot = Variant(), const Variant& slot_limit = Variant());
    void get_stake_activation(const String& account);
    void get_stake_minimum_delegation();
    void get_supply(bool exclude_non_circulating = false);
    void get_token_account_balance(const String& token_account);
    void get_token_accounts_by_delegate(const String& account_delegate, const String &mint = "", const String& program_id = "");
    void get_token_accounts_by_owner(const String& owner, const String &mint = "", const String& program_id = "");
    void get_token_largest_account(const String& token_mint);
    void get_token_supply(const String& token_mint);
    void get_transaction(const String& signature);
    void get_transaction_count();
    void get_version();
    void get_vote_accounts(const String& vote_pubkey = "", bool keep_unstaked_delinquents = false);
    void is_blockhash_valid(const String& blockhash);
    void minimum_ledger_slot();
    void request_airdrop(const String& address, uint64_t lamports);
    void send_transaction(const String& encoded_transaction, uint64_t max_retries = 10, bool skip_preflight = false);
    void simulate_transaction(const String& encoded_transaction, bool sig_verify = false, bool replace_blockhash = false, Array account_addresses = Array(), const String& account_encoding = "base64");

    void account_subscribe(const Variant &account_key, const Callable &callback);
    void signature_subscribe(const String &signature, const Callable &callback, const String &commitment);
    void program_subscribe(const String &program_id, const Callable &callback);
    void root_subscribe(const Callable &callback);
    void slot_subscribe(const Callable &callback);

    void unsubscribe_all(const Callable &callback);

    ~SolanaClient();
};
}

#endif