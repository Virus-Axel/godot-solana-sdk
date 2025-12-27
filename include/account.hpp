#ifndef SOLANA_SDK_ACCOUNT_HPP
#define SOLANA_SDK_ACCOUNT_HPP

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_utils.hpp"
#include "solana_client.hpp"

namespace godot {

/**
 * @brief Account mirror of a Solana account.
 *
 * Attempts to synchronize local data with on-chain account data.
 * Abstracts the fetching of account data from the Solana blockchain by
 * using web sockets or HTTP requests.
 */
class Account : public Node {
	GDCLASS_CUSTOM(Account, Node)

private:
	Variant key;
	PackedByteArray data;
	uint64_t lamports = 0;
	Variant owner;
	bool executable = false;
	bool sync_with_chain = true;

	double max_fetch_interval = 10.0;
	double min_fetch_interval = 5.0;
	double seconds_since_fetch = 0.0;

	void _get_property_list(List<PropertyInfo> *p_list) const;
	
protected:
	/**
	 * @brief Binds methods for Account Node.
	 */
	static void _bind_methods();

public:
	Account() = default;
	
	/**
	 * @_set{Transaction}
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @_get{Transaction}
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _process(double delta) override;

	static bool is_account(const Variant& other);
	
	bool is_valid() const;
	double get_seconds_since_last_sync() const;
	void synchronize();
	

	void set_sync_with_chain(bool value);
    bool get_sync_with_chain() const;
	void set_data(const PackedByteArray& value);
	PackedByteArray get_data() const;
	void set_executable(bool value);
	bool get_executable() const;
	void set_lamports(uint64_t value);
	uint64_t get_lamports() const;
	void set_key(const Variant& value);
	Variant get_key() const;
	void set_min_fetch_interval(double p_value);
	[[nodiscard]] double get_min_fetch_interval() const;
	void set_max_fetch_interval(double p_value);
	[[nodiscard]] double get_max_fetch_interval() const;
	
	void init_with_onchain_data(const Dictionary& onchain_data);

	~Account() override = default;
};

class AccountFetcher : public SolanaClient {
    GDCLASS_CUSTOM(AccountFetcher, SolanaClient)
private:
    uint64_t last_processed_frame = 0;
    bool pending_fetch = false;
    double time_since_last_fetch = 0;
    Dictionary fetch_accounts;
    Dictionary fetch_accounts_buffer;
    
    void fetch_callback(const Dictionary &params);
protected:
    static void _bind_methods();

public:
	AccountFetcher();

	void _ready() override;
	void process(double delta);
	
	void fetch_all();
	bool has_account(const Variant& account);
	void add_account_to_fetch(const Variant &account);
	void remove_account_to_fetch(const Variant &account);

	~AccountFetcher() = default;
};

} //namespace godot

#endif