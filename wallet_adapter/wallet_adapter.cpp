#include "wallet_adapter.hpp"

#include <stdint.h>
#include <backpack_js.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <keypair.hpp>
#include <phantom_js.hpp>
#include <pubkey.hpp>
#include <solana_utils.hpp>
#include <solflare_js.hpp>
#include <web3_js.hpp>

#ifdef WEB_ENABLED
#include <emscripten.h>
#endif

namespace godot {

void WalletAdapter::clear_state() {
	wallet_state = State::IDLE;

#ifdef WEB_ENABLED
	emscripten_run_script("Module.wallet_status = 0;");
#endif
}

bool WalletAdapter::is_idle() {
	return wallet_state == State::IDLE;
}

bool WalletAdapter::has_multiple_wallets() {
	const Array available_wallets = get_available_wallets();
	return available_wallets.size() > 1;
}

void WalletAdapter::store_encoded_message(const PackedByteArray &serialized_message) {
#ifdef WEB_ENABLED

	String script = "Module.encoded_message = '";
	script += SolanaUtils::bs58_encode(serialized_message);
	script += "';";

	emscripten_run_script(script.utf8());

#endif
}

String WalletAdapter::wallet_name_from_type(WalletType wallet_type) {
	if (wallet_type >= WalletType::MAX_TYPES) {
		return "";
	}

	const char *WALLET_NAMES[] = {
		"solana",
		"solflare",
		"backpack",
	};

	return String(WALLET_NAMES[wallet_type]);
}

String WalletAdapter::wallet_check_name_from_type(WalletType wallet_type) {
	if (wallet_type >= WalletType::MAX_TYPES) {
		return "";
	}

	const Array WALLET_CHECK_NAMES = build_array(
			"isPhantom",
			"isSolflare",
			"isBackpack");

	return String(WALLET_CHECK_NAMES[wallet_type]);
}

String WalletAdapter::get_sign_transaction_script() const {
#ifdef WEB_ENABLED
	switch (wallet_type) {
		case WalletType::PHANTOM:
			return { phantom::SIGN_TRANSACTION_SCRIPT };
			break;

		case WalletType::SOLFLARE:
			return { solflare::SIGN_TRANSACTION_SCRIPT };
			break;

		case WalletType::BACKPACK:
			return { backpack::SIGN_TRANSACTION_SCRIPT };
			break;

		default:
			break;
	}
#endif
	return "";
}

String WalletAdapter::get_sign_message_script() const {
#ifdef WEB_ENABLED
	switch (wallet_type) {
		case WalletType::PHANTOM:
			return { phantom::SIGN_MESSAGE_SCRIPT };
			break;

		case WalletType::SOLFLARE:
			return { solflare::SIGN_MESSAGE_SCRIPT };
			break;

		case WalletType::BACKPACK:
			return { backpack::SIGN_MESSAGE_SCRIPT };
			break;

		default:
			break;
	}
#endif
	return "";
}

String WalletAdapter::get_connect_script() const {
#ifdef WEB_ENABLED
	switch (wallet_type) {
		case WalletType::PHANTOM:
			return { phantom::CONNECT_SCRIPT };
			break;

		case WalletType::SOLFLARE:
			return { solflare::CONNECT_SCRIPT };
			break;

		case WalletType::BACKPACK:
			return { backpack::CONNECT_SCRIPT };
			break;

		default:
			break;
	}
#endif
	return "";
}

void WalletAdapter::store_serialized_message(const PackedByteArray &serialized_message) {
#ifdef WEB_ENABLED

	String script = "Module.serialized_message = [";
	for (const char &value : serialized_message) {
		script += String::num_int64(value);
		script += ',';
	}
	script += "];";

	emscripten_run_script(script.utf8());

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
		message_signature[i] = emscripten_run_script_int(script.utf8());
	}

#endif
	return message_signature;
}

Array WalletAdapter::get_available_wallets() {
	Array available_wallets;

#ifdef WEB_ENABLED

	for (int i = 0; i < WalletType::MAX_TYPES; i++) {
		Array params;
		params.append(wallet_name_from_type(static_cast<WalletType>(i)));
		params.append(wallet_check_name_from_type(static_cast<WalletType>(i)));

		const String CHECK_SCRIPT = "\
        const {{0}} = window;\
        if({0}){\
            if({0}.{1}){\
                1\
            }else{\
              0\
            }\
        }\
        else{\
          0\
        }";

		if (static_cast<int>(JavaScriptBridge::get_singleton()->eval(CHECK_SCRIPT.format(params))) == 1) {
			available_wallets.append(i);
		}
	}

#endif

	return available_wallets;
}

void WalletAdapter::poll_connection() {
#ifdef WEB_ENABLED
	int wallet_connect_status = emscripten_run_script_int("Module.wallet_status");
	switch (wallet_connect_status) {
		case 0:
			return;
			break;

		case 1: {
			connected = true;
			String connected_pubkey(emscripten_run_script_string("Module.connect_key"));

			const PackedByteArray decoded_bytes = SolanaUtils::bs58_decode(connected_pubkey);
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

void WalletAdapter::poll_message_signing() {
#ifdef WEB_ENABLED
	int wallet_signing_status = emscripten_run_script_int("Module.wallet_status");
	switch (wallet_signing_status) {
		case 0:
			return;
			break;

		case 1: {
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
	ClassDB::bind_static_method("WalletAdapter", D_METHOD("get_available_wallets"), &WalletAdapter::get_available_wallets);

	ClassDB::add_property("WalletAdapter", PropertyInfo(Variant::INT, "wallet_type", PROPERTY_HINT_ENUM, "Phantom,Solflare,Backpack", PROPERTY_USAGE_DEFAULT), "set_wallet_type", "get_wallet_type");
}

WalletAdapter::WalletAdapter() {
#ifdef WEB_ENABLED
	emscripten_run_script(WEB3_JS_STR);
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
}

bool WalletAdapter::is_connected() const {
	return connected;
}

Variant WalletAdapter::get_connected_key() {
	return Pubkey::new_from_bytes(connected_key);
}

void WalletAdapter::set_wallet_type(int wallet_type) {
	this->wallet_type = static_cast<WalletType>(wallet_type);
}

int WalletAdapter::get_wallet_type() {
	return wallet_type;
}

void WalletAdapter::sign_message(const PackedByteArray &serialized_message, const uint32_t index) {
#ifdef WEB_ENABLED

	active_signer_index = index;

	wallet_state = State::SIGNING;
	store_serialized_message(serialized_message);
	store_encoded_message(serialized_message);

	Array params;
	params.append(active_signer_index);
	JavaScriptBridge::get_singleton()->eval(get_sign_transaction_script().format(params));

#endif
}

void WalletAdapter::sign_text_message(const String &message) {
#ifdef WEB_ENABLED

	active_signer_index = 0;

	wallet_state = State::SIGNING;
	Array params;
	params.append(message);
	JavaScriptBridge::get_singleton()->eval(get_sign_message_script().format(params));

#endif
}

uint32_t WalletAdapter::get_active_signer_index() const {
	return active_signer_index;
}

} //namespace godot