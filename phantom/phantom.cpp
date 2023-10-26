#include "phantom.hpp"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/java_script_bridge.hpp>
#include <solana_sdk.hpp>
#include <godot_cpp/classes/thread.hpp>

#ifdef SOLANA_SDK_WEBBUILD
#include <emscripten.h>
#include <emscripten/val.h>


const std::string sign_and_send_script = "\
  const url = 'https://unpkg.com/@solana/web3.js@1.87.2/lib/index.iife.min.js';\
  fetch(url)\
    .then(response => response.text())\
    .then(script => {\
      eval(script);\
    })\
    .catch(error => {\
      console.error('Error loading script:', error);\
    });\
  Module.phantom_status = 0;\
  async function foo() {\
    \
    const { solana } = window;\
    console.log(Module.serialized_message);\
    Module.message_signature = (await solana.request({\
      method: 'signTransaction',\
      params: {\
        message: Module.encoded_message,\
      }\
    }));\
    console.log(Module.message_signature);\
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
          console.log('Phantom wallet found!');\
          const response = await solana.connect();\
          Module.phantom_status = 1;\
          Module.phantom_key = response.publicKey.toString();\
        }\
      } else {\
        alert('Solana object not found!');\
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
  #ifdef SOLANA_SDK_WEBBUILD
  emscripten_run_script("Module.phantom_status = 0;");
  #endif
}

bool PhantomController::is_idle(){
  return phantom_state == State::IDLE;
  
}

void PhantomController::store_serialized_message(const PackedByteArray &serialized_message){
  #ifdef SOLANA_SDK_WEBBUILD
  String script = "Module.serialized_message = new Uint8Array(";
  script += String::num_uint64(serialized_message.size());
  script += ");";
  std::cout << "running " << script.utf8() << std::endl;
  emscripten_run_script(script.utf8());

  for(unsigned int i = 0; i < serialized_message.size(); i++){
    script = "Module.serialized_message[";
    script += String::num_uint64(i);
    script += "] = ";
    script += String::num_uint64(serialized_message[i]);
    script += ';';
    std::cout << "running " << script.utf8() << std::endl;
    emscripten_run_script(script.utf8());
  }

  script = "Module.encoded_message = '";
  script += SolanaSDK::bs58_encode(serialized_message);
  script += "';";
  std::cout << "running " << script.utf8() << std::endl;
  emscripten_run_script(script.utf8());
  
  #endif
}

PackedByteArray PhantomController::get_message_signature(){
  PackedByteArray message_signature;

  #ifdef SOLANA_SDK_WEBBUILD

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
  #ifdef SOLANA_SDK_WEBBUILD
  int phantom_connect_status = emscripten_run_script_int("Module.phantom_status");
  switch(phantom_connect_status){
    case 0:
      return;
      break;

    case 1:{
      connected = true;
      String connected_pubkey(emscripten_run_script_string("Module.phantom_key"));
      std::cout << "waaaaaaaaaa " << connected_pubkey.utf8() << std::endl;
      PackedByteArray decoded_bytes = SolanaSDK::bs58_decode(connected_pubkey);
      if(decoded_bytes.size() != 32){
        std::cout << "ERRORRR" << std::endl;
      }
      connected_key = decoded_bytes;
      std::cout << connected_key.size() << std::endl;
      std::cout << decoded_bytes.size() << std::endl;
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
  #ifdef SOLANA_SDK_WEBBUILD
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
  //connect();
}

void PhantomController::connect_phantom(){
  phantom_state = State::CONNECTING;
    #ifdef SOLANA_SDK_WEBBUILD
    //emscripten::val sol = emscripten::val::global("solana");
    emscripten_run_script(js_script);

    /*
    OS::get_singleton()->delay_msec(1000);

    //checkIfWalletIsConnected();
    while(emscripten_run_script_int("Module.phantom_status") == 0){
        std::cout << "Y from phantom " << emscripten_run_script_int("Module.phantom_status") << std::endl;
        OS::get_singleton()->delay_msec(1000);
    }
    std::cout << "DATA IS: " << emscripten_run_script_int("Module.phantom_status") << std::endl;
    */
    #endif
}

bool PhantomController::is_connected(){
  return connected;
}

PackedByteArray PhantomController::get_connected_key(){
  return connected_key;
}

void PhantomController::sign_message(const PackedByteArray &serialized_message){
  #ifdef SOLANA_SDK_WEBBUILD

  phantom_state = State::SIGNING;
  store_serialized_message(serialized_message);

  JavaScriptBridge::get_singleton()->eval(String(sign_and_send_script.c_str()));

  #endif
}

PhantomController::~PhantomController(){
}
}