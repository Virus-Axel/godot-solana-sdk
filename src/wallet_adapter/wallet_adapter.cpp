#include "wallet_adapter.hpp"

#include <cstdint>

#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/java_script_bridge.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/jni_singleton.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"

#include "keypair.hpp"
#include "pubkey.hpp"
#include "solana_utils.hpp"
#ifdef WEB_ENABLED
#include "wallet_adapter_generated.hpp"
#endif

namespace godot {

bool WalletAdapter::is_wallet_installed(WalletType wallet_type) {
#ifdef WEB_ENABLED
	const Array params = build_array(wallet_name_from_type(wallet_type));
	const String script = String("Module.bridge.isWalletInstalled('{0}')").format(params);
	const Variant ready_state = JavaScriptBridge::get_singleton()->eval(script);
	return static_cast<bool>(ready_state);
#endif
	return false;
}

Array WalletAdapter::get_all_wallets() {
	return build_array("Alpha", "Avana", "BitKeep", "Bitpie", "Clover", "Coin98", "Coinbase", "Coinhub", "Fractal", "Huobi", "HyperPay", "Keystone", "Krystal", "Ledger", "Math", "Neko", "Nightly", "Nufi", "Onto", "Particle", "Phantom", "SafePal", "Saifu", "Salmon", "Sky", "Solflare", "Solong", "Spot", "Tokenary", "TokenPocket", "Torus", "Trezor", "Trust", "UnsafeBurner", "WalletConnect", "Xdefi", "Backpack");
}

Variant WalletAdapter::get_android_plugin(){
	if(Engine::get_singleton()->has_singleton("WalletAdapterAndroid")){
		return Engine::get_singleton()->get_singleton("WalletAdapterAndroid");
	}
	else{
		return nullptr;
	}
}

void WalletAdapter::clear_state() {
	wallet_state = State::IDLE;

#ifdef WEB_ENABLED
	JavaScriptBridge::get_singleton()->eval("Module.bridge.clear_state()");
#endif

#ifdef ANDROID_ENABLED
	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() != Variant::NIL) {
		android_plugin.call("clearState");
	}
#endif
}

bool WalletAdapter::is_idle() {
	return wallet_state == State::IDLE;
}

bool WalletAdapter::did_transaction_change() const {
	return dirty_transaction;
}

bool WalletAdapter::has_multiple_wallets() {
	const Array available_wallets = get_available_wallets();
	return available_wallets.size() > 1;
}

String WalletAdapter::wallet_name_from_type(WalletType wallet_type) {
	const Array WALLET_NAMES = get_all_wallets();
	if (wallet_type >= WALLET_NAMES.size()) {
		return "";
	}

	return WALLET_NAMES[wallet_type];
}

String WalletAdapter::get_sign_transaction_script(uint32_t signer_index) {
#ifdef WEB_ENABLED
	const Array params = build_array(signer_index);
	const String result = String::utf8("Module.bridge.signTransactionProcedure({0})");
	return result.format(params);
#endif
	return "";
}

String WalletAdapter::get_sign_message_script(const String &message) {
#ifdef WEB_ENABLED
	const String result = String::utf8("Module.bridge.signMessageProcedure('{0}')");
	return result.format(build_array(message));
#endif
	return "";
}

String WalletAdapter::get_connect_script() const {
#ifdef WEB_ENABLED
	const String result = String::utf8("Module.bridge.connectWalletProcedure('{0}')");

	const String wallet_name = wallet_name_from_type(wallet_type);
	return result.format(build_array(wallet_name));
#endif
	return "";
}

void WalletAdapter::store_serialized_message(const PackedByteArray &serialized_message) {
#ifdef WEB_ENABLED

	String script = "Module.serialized_message = [";
	for (const uint8_t &value : serialized_message) {
		script += String::num_int64(value);
		script += ',';
	}
	script += "];";

	JavaScriptBridge::get_singleton()->eval(script);

#endif
}

PackedByteArray WalletAdapter::get_message_signature() {
	PackedByteArray message_signature;

#ifdef WEB_ENABLED
	message_signature.resize(SIGNATURE_LENGTH);

	for (unsigned int i = 0; i < SIGNATURE_LENGTH; i++) {
		String script = "Module.message_signature[";
		script += String::num_uint64(i);
		script += "]";
		message_signature[i] = JavaScriptBridge::get_singleton()->eval(script);
	}

#endif

#ifdef ANDROID_ENABLED
	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() == Variant::NIL) {
		WARN_PRINT_ONCE_ED_CUSTOM("No android plugin installed");
		return message_signature;
	}

	message_signature = android_plugin.call("getMessageSignature");
#endif
	return message_signature;
}

PackedByteArray WalletAdapter::get_modified_transaction() {
	return JavaScriptBridge::get_singleton()->eval("Module.tampered_serialized_message");
}

Array WalletAdapter::get_available_wallets() {
	Array available_wallets;

#ifdef WEB_ENABLED

	for (int i = 0; i < WalletType::MAX_TYPES; i++) {
		Array params;
		params.append(wallet_name_from_type(static_cast<WalletType>(i)));

		if (is_wallet_installed(static_cast<WalletType>(i))) {
			available_wallets.append(i);
		}
	}

#endif

	return available_wallets;
}

void WalletAdapter::poll_connection() {
#ifdef WEB_ENABLED
	const int wallet_connect_status = JavaScriptBridge::get_singleton()->eval("Module.wallet_status");
	switch (wallet_connect_status) {
		case 0:
			return;
			break;

		case 1: {
			connected = true;
			const String connected_pubkey(JavaScriptBridge::get_singleton()->eval("Module.connect_key"));

			const PackedByteArray decoded_bytes = SolanaUtils::bs58_decode(connected_pubkey);
			connected_key = decoded_bytes;
			// Axel, here we have a problem, it processes when in signing, so we get a reset.
			clear_state();
			emit_signal("connection_established");
			break;
		}
		default:
			connected = false;
			clear_state();
			emit_signal("connection_error");
			break;
	}
#endif

#ifdef ANDROID_ENABLED
	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() == Variant::NIL){
		WARN_PRINT_ONCE_ED_CUSTOM("No android plugin installed");
		connected = false;
		clear_state();
		emit_signal("connection_error");
		return;
	}

	const int wallet_connect_status = android_plugin.call("getConnectionStatus");

	switch (wallet_connect_status) {
		case 0:
			return;
			break;

		case 1: {
			connected = true;
			const PackedByteArray decoded_bytes = android_plugin.call("getConnectedKey");
			connected_key = decoded_bytes;
			clear_state();
			emit_signal("connection_established");
			break;
		}
		default:
			connected = false;
			clear_state();
			emit_signal("connection_error");
			break;
	}
#endif
}

bool WalletAdapter::is_message_tampered(const PackedByteArray &original_serialization, const PackedByteArray &new_serialization) {
	const PackedByteArray original_message = strip_signatures(original_serialization);
	const PackedByteArray new_message = strip_signatures(new_serialization);

	if (original_message.size() != new_message.size()) {
		return true;
	}
	for (unsigned int i = 0; i < original_message.size(); i++) {
		if (original_message[i] != new_message[i]) {
			return true;
		}
	}
	return false;
}

PackedByteArray WalletAdapter::strip_signatures(const PackedByteArray &serialization) {
	const uint8_t signature_size = serialization[0];
	const uint32_t SIGNATURE_OFFSET = 1 + (signature_size * SIGNATURE_LENGTH);
	return serialization.slice(SIGNATURE_OFFSET);
}

void WalletAdapter::poll_message_signing() {
#ifdef WEB_ENABLED
	const int wallet_signing_status = JavaScriptBridge::get_singleton()->eval("Module.wallet_status");
	switch (wallet_signing_status) {
		case 0:
			return;
			break;

		case 1: {
			const PackedByteArray old_serialized_message = JavaScriptBridge::get_singleton()->eval("Module.old_serialized_message");
			const PackedByteArray new_serialized_message = JavaScriptBridge::get_singleton()->eval("Module.tampered_serialized_message");

			dirty_transaction = is_message_tampered(old_serialized_message, new_serialized_message);
			clear_state();

			emit_signal("message_signed", get_message_signature());
			break;
		}
		default:
			clear_state();
			emit_signal("signing_failed");
			break;
	}
#endif

#ifdef ANDROID_ENABLED
	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() == Variant::NIL) {
		WARN_PRINT_ONCE_ED_CUSTOM("No android plugin installed");
		clear_state();
		emit_signal("signing_failed");
		return;
	}

	const int wallet_signing_status = android_plugin.call("getSigningStatus");
	switch (wallet_signing_status) {
		case 0:
			return;
			break;

		case 1: {
			dirty_transaction = false;
			clear_state();
			const PackedByteArray signed_transaction = get_message_signature();
			const uint32_t SIGNATURE_OFFSET = 1 + SIGNATURE_LENGTH * active_signer_index;

			if (signed_transaction.size() < SIGNATURE_OFFSET + SIGNATURE_LENGTH) {
				emit_signal("signing_failed");
				return;
			}

			const PackedByteArray message_signature = signed_transaction.slice(SIGNATURE_OFFSET, SIGNATURE_OFFSET + SIGNATURE_LENGTH);
			emit_signal("message_signed", message_signature);
			break;
		}
		default:
			clear_state();
			emit_signal("signing_failed");
			break;
	}
#endif
}

void WalletAdapter::_process(double delta) { // NOLINT(misc-unused-parameters)
	switch (wallet_state) {
		case State::IDLE:
			return;
			break;
		case State::CONNECTING:
			poll_connection();
			break;
		case State::SIGNING:
			poll_message_signing();
			break;
	}
}

void WalletAdapter::_bind_methods() {
	ClassDB::add_signal("WalletAdapter", MethodInfo("connection_established"));
	ClassDB::add_signal("WalletAdapter", MethodInfo("connection_failed"));
	ClassDB::add_signal("WalletAdapter", MethodInfo("message_signed", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "signature")));
	ClassDB::add_signal("WalletAdapter", MethodInfo("signing_failed"));
	ClassDB::bind_method(D_METHOD("connect_wallet"), &WalletAdapter::connect_wallet);
	ClassDB::bind_method(D_METHOD("sign_message", "serialized_message", "signer_index"), &WalletAdapter::sign_message);
	ClassDB::bind_method(D_METHOD("sign_text_message", "text_message"), &WalletAdapter::sign_text_message);
	ClassDB::bind_method(D_METHOD("get_connected_key"), &WalletAdapter::get_connected_key);
	ClassDB::bind_method(D_METHOD("set_wallet_type", "wallet_type"), &WalletAdapter::set_wallet_type);
	ClassDB::bind_method(D_METHOD("get_wallet_type"), &WalletAdapter::get_wallet_type);
	ClassDB::bind_method(D_METHOD("set_mobile_identity_uri", "mobile_identity_uri"), &WalletAdapter::set_mobile_identity_uri);
	ClassDB::bind_method(D_METHOD("get_mobile_identity_uri"), &WalletAdapter::get_mobile_identity_uri);
	ClassDB::bind_method(D_METHOD("set_mobile_identity_name", "mobile_identity_name"), &WalletAdapter::set_mobile_identity_name);
	ClassDB::bind_method(D_METHOD("get_mobile_identity_name"), &WalletAdapter::get_mobile_identity_name);
	ClassDB::bind_method(D_METHOD("set_mobile_icon_path", "mobile_icon_path"), &WalletAdapter::set_mobile_icon_path);
	ClassDB::bind_method(D_METHOD("get_mobile_icon_path"), &WalletAdapter::get_mobile_icon_path);
	ClassDB::bind_method(D_METHOD("set_mobile_blockchain", "mobile_blockchain"), &WalletAdapter::set_mobile_blockchain);
	ClassDB::bind_method(D_METHOD("get_mobile_blockchain"), &WalletAdapter::get_mobile_blockchain);
	ClassDB::bind_static_method("WalletAdapter", D_METHOD("get_available_wallets"), &WalletAdapter::get_available_wallets);

	ClassDB::add_property_group("WalletAdapter", "Web", "");
	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::INT, "wallet_type", PROPERTY_HINT_ENUM, String(",").join(get_all_wallets()), PROPERTY_USAGE_DEFAULT), "set_wallet_type", "get_wallet_type");

	ClassDB::add_property_group("WalletAdapter", "Mobile", "mobile_");
	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::INT, "mobile_blockchain", PROPERTY_HINT_ENUM, "DEVNET, MAINNET, TESTNET", PROPERTY_USAGE_DEFAULT), "set_mobile_blockchain", "get_mobile_blockchain");
	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::STRING, "mobile_identity_uri"), "set_mobile_identity_uri", "get_mobile_identity_uri");
	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::STRING, "mobile_identity_name"), "set_mobile_identity_name", "get_mobile_identity_name");
	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::STRING, "mobile_icon_path"), "set_mobile_icon_path", "get_mobile_icon_path");
}

WalletAdapter::WalletAdapter() {
#ifdef WEB_ENABLED
	JavaScriptBridge::get_singleton()->eval(String::utf8(static_cast<const char *>(WALLET_ADAPTER_BUNDLED)));
#endif
}

bool WalletAdapter::is_wallet_adapter(const Variant &other) {
	return (static_cast<Object *>(other))->is_class("WalletAdapter");
}

void WalletAdapter::connect_wallet() {
	wallet_state = State::CONNECTING;

#ifdef WEB_ENABLED
	JavaScriptBridge::get_singleton()->eval(get_connect_script());
#endif

#ifdef ANDROID_ENABLED
	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() == Variant::NIL){
		WARN_PRINT_ONCE_ED_CUSTOM("No android plugin installed");
		return;
	}

	android_plugin.call("connectWallet", mobile_blockchain, mobile_identity_uri, mobile_icon_path, mobile_identity_name);
#endif
}

bool WalletAdapter::is_connected() const {
	return connected;
}

Variant WalletAdapter::get_connected_key() {
	return Pubkey::new_from_bytes(connected_key);
}

void WalletAdapter::set_mobile_blockchain(int mobile_blockchain) {
	this->mobile_blockchain = static_cast<Blockchain>(mobile_blockchain);
}

int WalletAdapter::get_mobile_blockchain() {
	return mobile_blockchain;
}

void WalletAdapter::set_wallet_type(int wallet_type) {
	this->wallet_type = static_cast<WalletType>(wallet_type);
}

int WalletAdapter::get_wallet_type() {
	return wallet_type;
}

void WalletAdapter::set_mobile_identity_name(const String& mobile_identity_name) {
	this->mobile_identity_name = mobile_identity_name;
}

String WalletAdapter::get_mobile_identity_name() {
	return mobile_identity_name;
}

void WalletAdapter::set_mobile_identity_uri(const String& mobile_identity_uri) {
	this->mobile_identity_uri = mobile_identity_uri;
}

String WalletAdapter::get_mobile_identity_uri() {
	return mobile_identity_uri;
}

void WalletAdapter::set_mobile_icon_path(const String& mobile_icon_path) {
	this->mobile_icon_path = mobile_icon_path;
}

String WalletAdapter::get_mobile_icon_path() {
	return mobile_icon_path;
}

void WalletAdapter::sign_message(const PackedByteArray &serialized_message, const uint32_t index) {
#ifdef WEB_ENABLED
	active_signer_index = index;
	dirty_transaction = false;

	wallet_state = State::SIGNING;
	store_serialized_message(serialized_message);

	JavaScriptBridge::get_singleton()->eval(get_sign_transaction_script(active_signer_index));

#endif

#ifdef ANDROID_ENABLED

	Variant android_plugin = get_android_plugin();
	if (android_plugin.get_type() == Variant::NIL){
		WARN_PRINT_ONCE_ED_CUSTOM("No android plugin installed");
		return;
	}

	wallet_state = State::SIGNING;

	android_plugin.call("signTransaction", serialized_message);

#endif
}

void WalletAdapter::sign_text_message(const String &message) {
#ifdef WEB_ENABLED
	dirty_transaction = false;
	active_signer_index = 0;

	wallet_state = State::SIGNING;
	JavaScriptBridge::get_singleton()->eval(get_sign_message_script(message));

#endif
}

uint32_t WalletAdapter::get_active_signer_index() const {
	return active_signer_index;
}

} //namespace godot