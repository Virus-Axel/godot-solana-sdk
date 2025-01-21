#ifndef SOLANA_SDK_SCRIPT_BUILDER_JS_HPP
#define SOLANA_SDK_SCRIPT_BUILDER_JS_HPP
#ifdef WEB_ENABLED

namespace script_builder {

const char *SIGN_MESSAGE_SCRIPT = "\
  Module.wallet_status = 0;\
  async function storeMessageSignature() {\
    try{\
      buffer_message = new TextEncoder().encode('{0}');\
      Module.message_signature = (await Module.wallet_handler.signMessage(buffer_message, 'utf8')).signature;\
      if(Module.message_signature == null){\
        Module.wallet_status = -1;\
        return;\
      }\
    }\
    catch (err){\
      Module.wallet_status = -1;\
      return;\
    }\
    Module.wallet_status = 1;\
  }\
  \
  storeMessageSignature();\
";

const char *SIGN_TRANSACTION_SCRIPT = "\
  Module.wallet_status = 0;\
  async function storeTransactionSignature() {\
    \
    try{\
      Module.old_serialized_message = new Uint8Array(Module.serialized_message);\
      var tx = Module.Transaction.from(Module.serialized_message);\
      var response = (await Module.wallet_handler.signTransaction(tx));\
      Module.tampered_serialized_message = response.serialize({ requireAllSignatures: false, verifySignatures: false });\
      console.log(response);\
      Module.message_signature = response.signatures[{0}].signature;\
      if(Module.message_signature == null){\
        Module.wallet_status = -1;\
        return;\
      }\
    }\
    catch (err){\
      Module.wallet_status = -1;\
      return;\
    }\
    Module.wallet_status = 1;\
  }\
  \
  storeTransactionSignature();\
";

const char *CONNECT_SCRIPT = "\
    console.log(Module.solanaWeb3);\
    console.log(Module.wallets);\
    Module.wallet_status = 0;\
    async function connectWallet() {\
    try{\
      Module.wallet_handler = new Module.wallets.{0}WalletAdapter;\
      await Module.wallet_handler.connect();\
      Module.connect_key = Module.wallet_handler.publicKey.toString();\
      Module.wallet_status = 1;\
      }\
      catch (error){\
        console.error(error);\
        Module.wallet_status = -1;\
      }\
    }\
    connectWallet()\
    ";

inline String get_connect_script(const String &wallet_name) {
	String result(CONNECT_SCRIPT);

	return result.format(build_array(wallet_name));
}

inline String get_sign_transaction_script() {
	String result(SIGN_TRANSACTION_SCRIPT);

	return result;
}

inline String get_sign_message_script() {
	String result(SIGN_MESSAGE_SCRIPT);

	return result;
}

} //namespace script_builder

#endif
#endif