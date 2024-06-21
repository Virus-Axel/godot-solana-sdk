#ifndef SOLANA_SDK_SOLANA_CLIENT_HPP
#define SOLANA_SDK_SOLANA_CLIENT_HPP

#include <queue>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/web_socket_peer.hpp>
#include <godot_cpp/classes/http_client.hpp>

namespace godot {

class SolanaClient : public Node {
    GDCLASS(SolanaClient, Node)

private:
    static unsigned int global_rpc_id;

    unsigned int local_rpc_id = 0;
    
    float timeout = 10.0;
    float elapsed_time = 0;

    const int DEFAULT_PORT = 443;
    const std::string DEFAULT_URL = "https://api.devnet.solana.com";
    const std::string DEFAULT_WS_URL = "wss://api.devnet.solana.com";

    String url_override = "";
    String ws_url = "";
    std::string http_request_body;
    int port = DEFAULT_PORT;
    bool use_tls = true;
    bool async_override = false;
    bool pending_request = false;
    HTTPClient http_handler;
    
    Callable http_callback;

    std::vector<std::pair<int, Callable>> callbacks;
    std::queue<String> ws_request_queue;
    std::vector<String> method_names;
    WebSocketPeer *ws = nullptr;

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

    String ws_from_http(const String& http_url);
    String get_real_url();
    String get_real_ws_url();

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

    Dictionary make_rpc_dict(const String& method, const Array& params);
    Dictionary make_rpc_param(const Variant& key, const Variant& value);
    Dictionary synchronous_request(const String& request_body);
    void asynchronous_request(const String& request_body);
    Dictionary quick_http_request(const String& request_body, const Callable& callback = Callable());
    Dictionary parse_url(const String& url);
    String assemble_url(const Dictionary& url_components);

    void poll_http_request(const float delta);

    void process_package(const PackedByteArray& packet_data);
    void connect_ws();
    void response_callback(const Dictionary &params);

protected:
    static void _bind_methods();

public:

    void _process(double delta) override;
    void _ready() override;

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
    void set_http_callback(const Callable& callback);

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

    Dictionary get_account_info(const String& account);
    Dictionary get_balance(const String& account);
    Dictionary get_block(uint64_t slot, const String& detail);
    Dictionary get_block_height();
    Dictionary get_latest_blockhash();
    Dictionary get_block_production();
    Dictionary get_block_commitment(uint64_t slot_number);
    Dictionary get_blocks(uint64_t start_slot, const Variant& end_slot = Variant::NIL);
    Dictionary get_blocks_with_limit(uint64_t start_slot, uint64_t end_slot);
    Dictionary get_block_time(uint64_t slot);
    Dictionary get_cluster_nodes();
    Dictionary get_epoch_info();
    Dictionary get_epoch_schedule();
    Dictionary get_fee_for_message(const String& encoded_message);
    Dictionary get_first_available_block();
    Dictionary get_genesis_hash();
    Dictionary get_health();
    Dictionary get_highest_snapshot_slot();
    Dictionary get_identity();
    Dictionary get_inflation_governor();
    Dictionary get_inflation_rate();
    Dictionary get_inflation_reward(const PackedStringArray accounts, const Variant& epoch = Variant::NIL);
    Dictionary get_largest_accounts(const String& filter = "");
    Dictionary get_leader_schedule(const Variant& slot = Variant::NIL);
    Dictionary get_max_retransmit_slot();
    Dictionary get_max_shred_insert_slot();
    Dictionary get_minimum_balance_for_rent_extemption(uint64_t data_size);
    Dictionary get_multiple_accounts(const PackedStringArray accounts);
    Dictionary get_program_accounts(const String& program_address, bool with_context = false);
    Dictionary get_recent_performance_samples();
    Dictionary get_recent_prioritization_fees(PackedStringArray account_addresses);
    Dictionary get_signature_for_address(const String& address, const String& before = "", const String& until = "");
    Dictionary get_signature_statuses(const PackedStringArray signatures, bool search_transaction_history = false);
    Dictionary get_slot();
    Dictionary get_slot_leader();
    Dictionary get_slot_leaders(const Variant& start_slot = Variant(), const Variant& slot_limit = Variant());
    Dictionary get_stake_activation(const String& account);
    Dictionary get_stake_minimum_delegation();
    Dictionary get_supply(bool exclude_non_circulating = false);
    Dictionary get_token_account_balance(const String& token_account);
    Dictionary get_token_accounts_by_delegate(const String& account_delegate, const String &mint = "", const String& program_id = "");
    Dictionary get_token_accounts_by_owner(const String& owner, const String &mint = "", const String& program_id = "");
    Dictionary get_token_largest_account(const String& token_mint);
    Dictionary get_token_supply(const String& token_mint);
    Dictionary get_transaction(const String& signature);
    Dictionary get_transaction_count();
    Dictionary get_version();
    Dictionary get_vote_accounts(const String& vote_pubkey = "", bool keep_unstaked_delinquents = false);
    Dictionary is_blockhash_valid(const String& blockhash);
    Dictionary minimum_ledger_slot();
    Dictionary request_airdrop(const String& address, uint64_t lamports);
    Dictionary send_transaction(const String& encoded_transaction, uint64_t max_retries = 10, bool skip_preflight = false);
    Dictionary simulate_transaction(const String& encoded_transaction, bool sig_verify = false, bool replace_blockhash = false, Array account_addresses = Array(), const String& account_encoding = "base64");

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