#ifndef GODOT_SOLANA_SDK_SHDW_DRIVE_HPP
#define GODOT_SOLANA_SDK_SHDW_DRIVE_HPP

#include "solana_client.hpp"
#include "transaction.hpp"
#include <godot_cpp/classes/http_request.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/resource.hpp>

#include "solana_utils.hpp"

namespace godot {

/**
 * @brief Parameters used when uploading a file to a ShdwDrive.
 */
typedef struct {
	const Variant &account_key; ///< Account key to upload to.
	const String &filename_hash; ///< Hash of the filename. Can be obtained with get_filename_hash.
} UploadParams;

/**
 * @brief Parameters used when initializing a shadow drive.
 */
typedef struct {
	const Variant &owner_keypair; ///< Owner Keypair of the storage.
	const String &name; ///< Name of the storage.
	uint64_t storage; ///< Storage size in bytes.
} InitParams;

/**
 * @brief Storage account data object used in ShdwDrive transactions.
 */
class StorageAccountV2 : public Resource {
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
	/**
	 * @bindmethods{StorageAccountV2, Resource}
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Reconstruct Storage account structure from a byte array.
	 * 
	 * @param bytes Byte array to reconstruct from.
	 */
	void from_bytes(const PackedByteArray &bytes);

	/**
	 * @getter{identifier, String}
	*/
	String get_identifier();

	/**
	 * @brief Assembles resource into a Dictionary object.
	 * 
	 * @return Dictionary Storage account data as Dictionary.
	 */
	Dictionary to_dict();
};

/**
 * @brief Contains information about a user needed by ShdwDrive program.
 */
class UserInfo : public Resource {
	GDCLASS(UserInfo, Resource)
private:
	uint32_t account_counter = 0;
	uint32_t delete_counter = 0;
	bool agreed_to_terms = false;
	bool had_bad_scam_scan = false;

	static PackedByteArray discriminator();

protected:
	/**
	 * @bindmethods{UserInfo, Resource}
	 */
	static void _bind_methods();

public:
	/**
	 * @getter{account_counter, uint32_t}
	 */
	uint32_t get_account_counter() const;

	/**
	 * @brief Constructs UserInfo from a byte array.
	 * 
	 * @param bytes Bytes to construct UserInfo from.
	 */
	void from_bytes(const PackedByteArray &bytes);
};

/**
 * @class ShdwDrive
 * @brief Manager of ShdwDrive API service and on chain program.
 * 
 * This class can be used to interface the ShdwDrive service to create permanent storage
 * on the blockchain. This class can be used to get an URI to a permanent file storage for NFTs.
 */
class ShdwDrive : public Node {
	GDCLASS(ShdwDrive, Node)
private:
	static const std::string PID;

	bool new_user = false;
	bool simulate_only = false;
	Variant owner_keypair;
	String storage_name;
	uint64_t storage_size = 0;
	UserInfo *user_info = memnew_custom(UserInfo);
	StorageAccountV2 *storage_account = nullptr;
	RpcSingleHttpRequestClient *api_request = memnew_custom(RpcSingleHttpRequestClient);
	HTTPRequest *upload_file_request = memnew_custom(HTTPRequest);

	Array storage_list;
	PackedStringArray storage_name_list;
	PackedStringArray storage_key_list;

	SolanaClient *create_storage_account_client = memnew_custom(SolanaClient);
	SolanaClient *fetch_user_info_client = memnew_custom(SolanaClient);
	SolanaClient *fetch_storage_account_client = memnew_custom(SolanaClient);
	SolanaClient *fetch_all_storage_accounts_client = memnew_custom(SolanaClient);
	Transaction *create_storage_account_transaction = memnew_custom(Transaction);

	static PackedByteArray initialize_accountv2_discriminator();
	static PackedByteArray create_form_line(const String &line);
	static PackedByteArray create_form_line(const PackedByteArray &content);
	static String get_upload_message(const UploadParams &upload_params);
	static String get_filename_hash(const String &filename);

	void get_all_storage_accounts(const Variant &owner_key);
	void send_fetch_account_infos();

	void fetch_userinfo_callback(const Dictionary &params);
	void fetch_storage_account_callback(const Dictionary &params);
	void upload_file_callback(int result, int response_code, const PackedStringArray &headers, const PackedByteArray &body);

	void emit_simulation_response(const Dictionary &response);

	static uint64_t human_size_to_bytes(const String &human_size);

	static Variant get_uploader();

protected:
	/**
	 * @bindmethods{ShdwDrive, Node}
	 */
	static void _bind_methods();

public:
	ShdwDrive();

	/**
	 * @_process
	 */
	void _process(double delta) override;

	void set_simulate_only(const bool simulate_only);
	bool get_simulate_only() const;

	/**
	 * @brief Get an Array of cached storage accounts.
	 * 
	 * @return Array Storage accounts.
	 */
	Array get_cached_storage_accounts();

	/**
	 * @brief Fetch information about a user from an account.
	 * 
	 * @param address Address of account to fetch info from.
	 * @return Variant Status of the request.
	 */
	Variant fetch_user_info(const Variant &address);

	/**
	 * @brief Fetch information about a storage account.
	 * 
	 * @param address Address of storage account to fetch info from.
	 * @return Variant Status of the request.
	 */
	Variant fetch_storage_account(const Variant &address);

	/**
	 * @brief Initiate a request to create a storage account.
	 * 
	 * Starts a fetch user info request and a fetch storage account request to get required parameters.
	 * Once requests are finalized the transaction to ShdwDrive program is created and sent.
	 * 
	 * @param owner_keypair Owner of the storage account.
	 * @param name Name of storage account.
	 * @param size Size in bytes integer or in human size String, Example input 10000, 10KB, 300mb, 1GB.
	 * @return Variant nullptr
	 */
	Variant create_storage_account(const Variant &owner_keypair, const String &name, const Variant &size);

	/**
	 * @brief Called when storage by name is fetched.
	 * 
	 * @param storage_name Name of storage account.
	 * @return Variant Storage account Pubkey.
	 * @return Variant nullptr on failure.
	 */
	Variant fetch_storage_key_by_name_callback(const String &storage_name);

	/**
	 * @brief Fetch a storage account key by storage name.
	 * 
	 * Searches cached storage accounts and gets key directly if found. Otherwise all storage accounts
	 * are cached and the storage account key is fetched for storage with specified name.
	 * 
	 * @param owner_keypair Owner of the storage.
	 * @param storage_name Name of storage to fetch key of.
	 * @return Variant Storage account key if account was cached.
	 * @return Variant nullptr if storage accounts are being fetched first.
	 */
	Variant fetch_storage_key_by_name(const Variant &owner_keypair, const String &storage_name);

	/**
	 * @brief Partially sign and prepare a create storage Transaction.
	 */
	void send_create_storage_tx();

	/**
	 * @brief Send transaction to ShdwDrive endpoint for master key signing.
	 * 
	 * A request will be initiated to send the transaction for signing. When response is received
	 * create_store_api_response will be called. If successfull ShdwDrive will completely sign and
	 * send the transaction.
	 */
	void send_create_storage_tx_signed();

	/**
	 * @brief Called when API responded to a create store request.
	 * 
	 * A signal storage_account_response will be emitted when called.
	 * 
	 * @param response Response from ShdwDrive endpoint.
	 */
	void create_store_api_response(const Dictionary &response);

	/**
	 * @brief Called when all storage accounts are being fetched.
	 * 
	 * @param response Get account info request response from RPC node.
	 */
	void get_multiple_accounts_callback(const Dictionary &response);

	/**
	 * @brief Create a new user info PDA Pubkey.
	 * 
	 * @pda{"user-info", base_keypair}
	 * 
	 * @param base_keypair Base keypair.
	 * @return Variant New user info Pubkey.
	 */
	static Variant new_user_info_pubkey(const Variant &base_keypair);

	/**
	 * @brief Create a new stake account Pubkey.
	 * 
	 * @pda{"stake-account", storage_account}
	 * 
	 * @param storage_account Storage account to get stake account Pubkey for.
	 * @return Variant New stake account Pubkey.
	 */
	static Variant new_stake_account_pubkey(const Variant &storage_account);

	/**
	 * @brief Create new storage config Pubkey.
	 * 
	 * @pda{"storage-config"}
	 * 
	 * @return Variant New storage config Pubkey.
	 */
	static Variant new_storage_config_pubkey();

	/**
	 * @brief Create new storage account Pubkey.
	 * 
	 * @pda{"storage-account", owner_key, account_seed}
	 * 
	 * @param owner_key Owner key.
	 * @param account_seed Account seed.
	 * @return Variant New storage account Pubkey.
	 */
	static Variant new_storage_account_pubkey(const Variant &owner_key, uint64_t account_seed);

	/**
	 * @brief Create a new Instruction that initializes an account.
	 * 
	 * @param initialize_params Initialize parameters. 
	 * @return Variant New initialize Instruction.
	 */
	Variant initialize_account(const InitParams &initialize_params);

	/**
	 * @brief Sends a upload file request to ShdwDrive endpoint.
	 * 
	 * @param filename Name of file.
	 * @param storage_owner_keypair Storage owner keypair. @signer
	 * @param storage_account Storage account key.
	 */
	void upload_file_to_storage(const String &filename, const Variant &storage_owner_keypair, const Variant &storage_account);

	void create_folder(const String &bucket, const String &folder_name, const String &signer, const String &signature);
	void upload_small_file(String bucket, String file_path, const Variant &signer, String directory);

	/**
	 * @brief Get the ShdwDrive program ID.
	 * 
	 * @return Variant ShdwDrive program ID.
	 */
	static Variant get_pid();

	~ShdwDrive();
};

} //namespace godot

#endif