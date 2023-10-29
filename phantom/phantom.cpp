#include "phantom.hpp"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <solana_sdk.hpp>
#include <godot_cpp/classes/thread.hpp>

#ifdef WEB_ENABLED
#include <emscripten.h>


const std::string sign_and_send_script = "\
  Module.phantom_status = 0;\
  async function foo() {\
    \
    const { solana } = window;\
    Module.message_signature = (await solana.request({\
      method: 'signTransaction',\
      params: {\
        message: Module.encoded_message,\
      }\
    })).signatures[0];\
    Module.phantom_status = 1;\
  }\
  \
  foo();\
";

const char* js_script = "\
    Module.phantom_status = 0;\
    async function checkIfWalletIsConnected() {\
    try{\
      const { solana } = window;\
      if (solana) {\
        if (solana.isPhantom) {\
          const response = await solana.connect();\
          Module.phantom_key = response.publicKey.toString();\
          Module.phantom_status = 1;\
        }\
      } else {\
        Module.phantom_status = -1;\
      }\
      }\
      catch (error){\
      console.error(error);\
      Module.phantom_status = -1;\
      }\
    }\
    checkIfWalletIsConnected()\
    ";

#endif

namespace godot{

void PhantomController::clear_state(){
  phantom_state = State::IDLE;
  #ifdef WEB_ENABLED
  emscripten_run_script("Module.phantom_status = 0;");
  #endif
}

bool PhantomController::is_idle(){
  return phantom_state == State::IDLE;
  
}

void PhantomController::store_encoded_message(const PackedByteArray &serialized_message){
  #ifdef WEB_ENABLED

  String script = "Module.encoded_message = '";
  script += SolanaSDK::bs58_encode(serialized_message);
  script += "';";

  emscripten_run_script(script.utf8());
  
  #endif
}

PackedByteArray PhantomController::get_message_signature(){
  PackedByteArray message_signature;

  #ifdef WEB_ENABLED

  // TODO: replace with named constant.
  message_signature.resize(64);

  for(unsigned int i = 0; i < 64; i++){
    String script = "Module.message_signature[";
    script += String::num_uint64(i);
    script += "]";
    message_signature[i] = emscripten_run_script_int(script.utf8());
  }

  #endif
  return message_signature;
}

void PhantomController::poll_connection(){
  #ifdef WEB_ENABLED
  int phantom_connect_status = emscripten_run_script_int("Module.phantom_status");
  switch(phantom_connect_status){
    case 0:
      return;
      break;

    case 1:{
      connected = true;
      String connected_pubkey(emscripten_run_script_string("Module.phantom_key"));

      PackedByteArray decoded_bytes = SolanaSDK::bs58_decode(connected_pubkey);
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


void PhantomController::poll_message_signing(){
  #ifdef WEB_ENABLED
  int phantom_signing_status = emscripten_run_script_int("Module.phantom_status");
  switch(phantom_signing_status){
    case 0:
      return;
      break;

    case 1:{
      clear_state();
      emit_signal("message_signed", get_message_signature());
      break;
    }
    default:
      clear_state();
      emit_signal("signing_error");
      break;
  }
  #endif
}

void PhantomController::_process(double delta){
  switch(phantom_state){
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

void PhantomController::_bind_methods(){
  ClassDB::add_signal("PhantomController", MethodInfo("connection_established"));
  ClassDB::add_signal("PhantomController", MethodInfo("connection_error"));
  ClassDB::add_signal("PhantomController", MethodInfo("message_signed", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "signature")));
  ClassDB::add_signal("PhantomController", MethodInfo("signing_error"));
  ClassDB::bind_method(D_METHOD("connect_phantom"), &PhantomController::connect_phantom);
  ClassDB::bind_method(D_METHOD("sign_message", "serialized_message"), &PhantomController::sign_message);
  ClassDB::bind_method(D_METHOD("get_connected_key"), &PhantomController::get_connected_key);
}

PhantomController::PhantomController(){
}

void PhantomController::connect_phantom(){
  phantom_state = State::CONNECTING;
    #ifdef WEB_ENABLED
    emscripten_run_script(js_script);
    #endif
}

bool PhantomController::is_connected(){
  return connected;
}

PackedByteArray PhantomController::get_connected_key(){
  return connected_key;
}

void PhantomController::sign_message(const PackedByteArray &serialized_message){
  #ifdef WEB_ENABLED

  phantom_state = State::SIGNING;
  store_encoded_message(serialized_message);

  JavaScriptBridge::get_singleton()->eval(String(sign_and_send_script.c_str()));

  #endif
}

PhantomController::~PhantomController(){
}
}