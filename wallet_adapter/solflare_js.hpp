#ifndef SOLANA_SDK_SOLFLARE_JS_HPP
#define SOLANA_SDK_SOLFLARE_JS_HPP
#ifdef WEB_ENABLED

namespace solflare{

const char* SIGN_MESSAGE_SCRIPT = "\
  Module.wallet_status = 0;\
  async function foo() {\
    const { solflare } = window;\
    try{\
        buffer_message = new TextEncoder().encode('{0}');\
        Module.message_signature = (await solflare.signMessage(buffer_message, 'utf8')).signature;\
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
    const { solflare } = window;\
    try{\
      var tx = Module.solanaWeb3.Transaction.from(Module.serialized_message);\
      await solflare.signTransaction(tx);\
      Module.message_signature = tx.signatures[{0}].signature;\
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
      const { solflare } = window;\
      if (solflare) {\
        if (solflare.isSolflare) {\
          const response = await solflare.connect();\
          Module.connect_key = solflare.publicKey.toString();\
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

} // namespace solflare

#endif
#endif