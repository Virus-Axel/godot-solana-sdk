#ifndef SOLANA_SDK_ACCOUNT_HPP
#define SOLANA_SDK_ACCOUNT_HPP

#include <cstdint>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "solana_client.hpp"
#include "solana_utils.hpp"

namespace godot {

const double DEFALT_MAX_FETCH_INTERVAL = 10.0;
const double DEFALT_MIN_FETCH_INTERVAL = 5.0;

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
	String encoded_data = "";
	Variant key;
	PackedByteArray data;
	uint64_t lamports = 0;
	Variant owner;
	bool executable = false;
	bool sync_with_chain = true;

	double max_fetch_interval = DEFALT_MAX_FETCH_INTERVAL;
	double min_fetch_interval = DEFALT_MIN_FETCH_INTERVAL;
	double seconds_since_fetch = 0.0;

	Variant pre_simulate_transaction;

	void _get_property_list(List<PropertyInfo> *p_list) const;

protected:
	/**
	 * @brief Binds methods for Account Node.
	 */
	static void _bind_methods();

public:
	Account();

	/**
	 * @_set{Account}
	 */
	bool _set(const StringName &p_name, const Variant &p_value);

	/**
	 * @_get{Account}
	 */
	bool _get(const StringName &p_name, Variant &r_ret) const;

	/**
	 * @_process
	 */
	void _process(double delta) override;

	/**
	 * Check if a Variant is an Account object.
	 *
	 * @return bool indicating wether provided Variant is an Account.
	 */
	static bool is_account(const Variant &other);

	/**
	 * Check if the key property key is Pubkey of valid size.
	 *
	 * @return true if key is key is valid
	 * @return false if key is not set or invalid size.
	 */
	[[nodiscard]] bool is_valid() const;

	/**
	 * Get second since since last time this account data was synced with chain.
	 */
	[[nodiscard]] double get_seconds_since_last_sync() const;

	/**
	 * @setter{sync_with_chain}
	 */
	void set_sync_with_chain(bool sync_with_chain);

	/**
	 * @getter{sync_with_chain, bool}
	 */
	[[nodiscard]] bool get_sync_with_chain() const;

	/**
	 * @setter{data}
	 */
	void set_data(const PackedByteArray &data);

	/**
	 * @getter{data, PackedByteArray}
	 */
	[[nodiscard]] PackedByteArray get_data() const;

	/**
	 * @setter{executable}
	 */
	void set_executable(bool executable);

	/**
	 * @getter{executable, bool}
	 */
	[[nodiscard]] bool get_executable() const;

	/**
	 * @setter{lamports}
	 */
	void set_lamports(uint64_t lamports);

	/**
	 * @getter{lamports, uint64_t}
	 */
	[[nodiscard]] uint64_t get_lamports() const;

	/**
	 * @setter{key}
	 */
	void set_key(const Variant &key);

	/**
	 * @getter{key, Variant}
	 */
	[[nodiscard]] Variant get_key() const;

	/**
	 * @setter{min_fetch_interval}
	 */
	void set_min_fetch_interval(double min_fetch_interval);

	/**
	 * @getter{min_fetch_interval, double}
	 */
	[[nodiscard]] double get_min_fetch_interval() const;

	/**
	 * @setter{max_fetch_interval}
	 */
	void set_max_fetch_interval(double max_fetch_interval);

	/**
	 * @getter{max_fetch_interval, double}
	 */
	[[nodiscard]] double get_max_fetch_interval() const;

	/**
	 * @setter{pre_simulate_transaction}
	 */
	void set_pre_simulate_transaction(const Variant &pre_simulate_transaction);

	/**
	 * @getter{pre_simulate_transaction, Variant}
	 */
	[[nodiscard]] Variant get_pre_simulate_transaction() const;

	/**
	 * @brief Check if the pre_simulate_transaction property is assigned.
	 *
	 * @return true if the pre_simulate_transaction is assigned to a Transaction.
	 * @return false if pre_simulate_transaction is not assigned to a Transaction
	 */
	[[nodiscard]] bool has_pre_simulate_transaction() const;

	/**
	 * @brief Sync properties with response from a RPC server.
	 */
	void init_with_onchain_data(const Dictionary &onchain_data);

	~Account() override = default;
};

/**
 * @brief Accumulates accounts and bulk fetches them
 *
 * Keeps a local cache of accounts that are ready to be fetched. Syncs accounts and notifies the
 * individual Account nodes when fetched from chain. Requires Accounts to communicate their intents
 * through the class methods.
 */
class AccountFetcher : public SolanaClient {
	GDCLASS_CUSTOM(AccountFetcher, SolanaClient)
private:
	uint64_t last_processed_frame = 0;
	bool pending_fetch = false;
	bool pending_simulation = false;
	double time_since_last_fetch = 0;
	Dictionary fetch_accounts;
	Dictionary fetch_accounts_buffer;

	static void update_accounts(const Dictionary &account_dict, const Array &accounts_data);

	void fetch_callback(const Dictionary &params);

protected:
	/**
	 * @bindmethods{AccountFetcher, SolanaClient}
	 */
	static void _bind_methods();

public:
	AccountFetcher();

	/**
	 * @_ready
	 */
	void _ready() override;

	/**
	 * @brief Process base SolanaClient class if not already done this frame.
	 */
	void manual_process(double delta);

	/**
	 * @brief Trigger a bulk fetch.
	 */
	void fetch_all();

	/**
	 * @brief Check if provided Account is in fetch queue.
	 */
	[[nodiscard]] bool has_account(const Variant &account);

	/**
	 * @brief Store Account in fetch queue.
	 *
	 * @note Account will not be fetched until fetch_all is called.
	 */
	void add_account_to_fetch(const Variant &account);

	/**
	 * @brief Remove an account from fetch queue.
	 */
	void remove_account_to_fetch(const Variant &account);

	~AccountFetcher() override = default;
};

/**
 * @brief Accumulates accounts and bulk fetches them after a transaction simulation
 *
 * Keeps a local cache of accounts that are ready to be fetched. The accounts are stored per transaction-
 * Syncs accounts after a simulation and notifies the individual Account nodes when fetched from chain.
 * Requires Accounts to communicate their intents through the class methods.
 */
class AccountSimulator : public SolanaClient {
	GDCLASS_CUSTOM(AccountSimulator, SolanaClient)
private:
	uint64_t last_processed_frame = 0;
	bool pending_simulation = false;
	double time_since_last_fetch = 0;

	Dictionary sim_accounts;
	String pending_sim_key = "";

	static void update_accounts(const Dictionary &account_dict, const Array &accounts_data);
	void add_sim_account(const String &hash, const Variant &account);

	void fetch_callback(const Dictionary &params);

protected:
	/**
	 * @bindmethods{AccountSimulator, SolanaClient}
	 */
	static void _bind_methods();

public:
	AccountSimulator();

	/**
	 * @_ready
	 */
	void _ready() override;

	/**
	 * @brief Manually trigger a _process of base class.
	 *
	 * @param delta elapsed time since last process.
	 */
	void manual_process(double delta);

	/**
	 * @brief Simulate a Transaction and fetch all relevant Accounts in queue.
	 */
	void simulate_all(const Variant &transaction);

	/**
	 * @brief Add an account to the fetch queue of a certain Transaction.
	 */
	void add_account_to_fetch(const Variant &account);

	~AccountSimulator() override = default;
};

} //namespace godot

#endif
