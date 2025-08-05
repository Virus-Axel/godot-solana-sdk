#ifndef SOLANA_SDK_WALLET_ADAPTER_HPP
#define SOLANA_SDK_WALLET_ADAPTER_HPP

#include "solana_utils.hpp"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/jni_singleton.hpp>

namespace godot {

enum Blockchain {
	MAINNET = 0,
	DEVNET = 1,
	TESTNET = 2,
};

enum WalletType {
	ALPHA,
	AVANA,
	BITKEEP,
	BITPIE,
	CLOVER,
	COIN98,
	COINBASE,
	COINHUB,
	FRACTAL,
	HUOBI,
	HYPERPAY,
	KEYSTONE,
	KRYSTAL,
	LEDGER,
	MATH,
	NEKO,
	NIGHTLY,
	NUFI,
	ONTO,
	PARTICLE,
	PHANTOM,
	SAFEPAL,
	SAIFU,
	SALMON,
	SKY,
	SOLFLARE,
	SOLONG,
	SPOT,
	TOKENARY,
	TOKENPOCKET,
	TORUS,
	TREZOR,
	TRUST,
	UNSAFEBURNER,
	WALLETCONNECT,
	XDEFI,
	BACKPACK,
	MAX_TYPES,
};

/**
 * @brief Handles interaction with external wallet applications.
 * 
 * The WalletAdapter can be used as any other signer type like a Keypair. When using a WalletAdapter for
 * instructions or as payer, the wallet app will be requested to sign when needed. The class provices 
 * methods to connect and disconnect the selected wallet application.
 * 
 * @note This class is mainly intended for web platform. It has been proven to work on desktop and mobile
 * as well with some tweaking, but is not actively tested yet.
 */
class WalletAdapter : public Node {
	GDCLASS(WalletAdapter, Node)
private:
	bool connected = false;
	bool dirty_transaction = false;
	uint32_t active_signer_index = 0;
	String mobile_identity_uri = "https://solana.com";
	String mobile_icon_path = "favicon.ico";
	String mobile_identity_name = "Solana";
	enum State {
		IDLE = 0,
		CONNECTING = 1,
		SIGNING = 2,
	};

	PackedByteArray connected_key;
	State wallet_state = State::IDLE;
	WalletType wallet_type = PHANTOM;
	Blockchain mobile_blockchain = Blockchain::MAINNET;

	static Variant get_android_plugin();
	void clear_state();
	static void store_serialized_message(const PackedByteArray &serialized_message);
	static String wallet_name_from_type(WalletType wallet_type);

	static String get_sign_transaction_script(uint32_t signer_index);
	static String get_sign_message_script(const String &message);
	String get_connect_script() const;
	static bool is_wallet_installed(WalletType wallet_type);
	static bool is_message_tampered(const PackedByteArray &original_serialization, const PackedByteArray &new_serialization);
	static PackedByteArray strip_signatures(const PackedByteArray &serialization);
	static Array get_all_wallets();

protected:
	/**
	 * @bindmethods{WalletAdapter, Node}
	 */
	static void _bind_methods();

public:
	/**
	 * @_process
	 */
	void _process(double delta) override;
	WalletAdapter();

	/**
	 * @brief Check if Variant is of type WalletAdapter Object.
	 * 
	 * @param other Variatn to type check.
	 * @return true if Variant is a WalletAdapter.
	 * @return false Otherwise.
	 */
	static bool is_wallet_adapter(const Variant &other);

	/**
	 * @brief Attempt to connect wallet.
	 * 
	 * Opens a browser extension popup to ask user to connect. If already connected no popup will appear
	 * and the WalletAdapter will instantly get the connected Pubkey. 
	 */
	void connect_wallet();

	/**
	 * @brief Check if connected.
	 * 
	 * @return true If wallet is connected
	 * @return false if wallet is not connected.
	 */
	bool is_connected() const;

	/**
	 * @brief Check if no operation is ongoing.
	 * 
	 * @return true If wallet is idle.
	 * @return false If wallet is busy.
	 */
	bool is_idle();

	/**
	 * @brief Check if transaction changed after a wallet operation.
	 * 
	 * @return true If transaction was modified.
	 * @return false If transaction is the same.
	 */
	bool did_transaction_change() const;

	/**
	 * @brief Check if there are several available wallets.
	 * 
	 * @return true If several wallets are available.
	 * @return false If one or zero wallets are available.
	 */
	static bool has_multiple_wallets();

	/**
	 * @brief Poll connection request.
	 */
	void poll_connection();

	/**
	 * @brief Poll message signing request.
	 */
	void poll_message_signing();

	/**
	 * @brief Get the connected Pubkey.
	 * 
	 * @return Variant Connected Pubkey.
	 */
	Variant get_connected_key();

	/**
	 * @brief Get the message signature of latest signing operation.
	 */
	static PackedByteArray get_message_signature();

	/**
	 * @brief Get the modified serialized Transaction.
	 * 
	 * This can be used when wallet modifies the Transaction to get the modified version.
	 */
	static PackedByteArray get_modified_transaction();

	/**
	 * @brief Get the available wallets.
	 * 
	 * @return Array Array of wallet type IDs.
	 */
	static Array get_available_wallets();

	/**
	 * @setter{mobile_blockchain}
	 */
	void set_mobile_blockchain(int mobile_blockchain);

	/**
	 * @getter{mobile_blockchain, int}
	 */
	int get_mobile_blockchain();

	/**
	 * @setter{wallet_type}
	 */
	void set_wallet_type(int wallet_type);

	/**
	 * @getter{wallet_type, int}
	 */
	int get_wallet_type();

	/**
	 * @setter{mobile_identity_name}
	 */
	void set_mobile_identity_name(const String &mobile_identity_name);

	/**
	 * @getter{mobile_identity_name, String}
	 */
	String get_mobile_identity_name();

	/**
	 * @setter{mobile_identity_uri}
	 */
	void set_mobile_identity_uri(const String &mobile_identity_uri);

	/**
	 * @getter{mobile_identity_uri, String}
	 */
	String get_mobile_identity_uri();

	/**
	 * @setter{mobile_icon_path}
	 */
	void set_mobile_icon_path(const String &mobile_icon_path);

	/**
	 * @getter{mobile_icon_path, String}
	 */
	String get_mobile_icon_path();

	/**
	 * @brief Sign a Transaction message.
	 * 
	 * @param serialized_message Serialized Transaction message.
	 * @param index Index of the active signer.
	 */
	void sign_message(const PackedByteArray &serialized_message, const uint32_t index);

	/**
	 * @brief Sign a text message.
	 * 
	 * @param message Text message to sign.
	 */
	void sign_text_message(const String &message);

	/**
	 * @getter{active_signer_index, uint32_t}
	 */
	uint32_t get_active_signer_index() const;
	~WalletAdapter() = default;
};
} //namespace godot

#endif