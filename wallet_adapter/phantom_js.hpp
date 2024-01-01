#ifndef SOLANA_SDK_PHANTOM_JS_HPP
#define SOLANA_SDK_PHANTOM_JS_HPP
#ifdef WEB_ENABLED

#include <iostream>

namespace phantom{

const char* SIGN_MESSAGE_SCRIPT = "\
  Module.wallet_status = 0;\
  async function foo() {\
    const { solana } = window;\
    try{\
      buffer_message = new TextEncoder().encode('{0}');\
      Module.message_signature = (await solana.signMessage(buffer_message, 'utf8')).signature;\
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
  foo();\
";

const char* SIGN_TRANSACTION_SCRIPT = "\
  Module.wallet_status = 0;\
  async function foo() {\
    \
    const { solana } = window;\
    try{\
      console.log('signing from js');\
      var tx = Module.solanaWeb3.Transaction.from(Module.serialized_message);\
      var response = (await solana.signTransaction(tx));\
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
  foo();\
";

const char* CONNECT_SCRIPT = "\
    console.log(Module.solanaWeb3);\
    Module.wallet_status = 0;\
    async function checkIfWalletIsConnected() {\
    try{\
      const { solana } = window;\
      if (solana) {\
        if (solana.isPhantom) {\
          const response = await solana.connect();\
          Module.connect_key = response.publicKey.toString();\
          Module.wallet_status = 1;\
        }\
      } else {\
        Module.wallet_status = -1;\
      }\
      }\
      catch (error){\
        console.error(error);\
        Module.wallet_status = -1;\
      }\
    }\
    checkIfWalletIsConnected()\
    ";

} // namespace phantom

#endif
#endif