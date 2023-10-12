#include "phantom.hpp"
#include <godot_cpp/classes/os.hpp>
#include <solana_sdk.hpp>

#ifdef SOLANA_SDK_WEBBUILD
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

  void godot::PhantomController::set_data(){
      godot::PhantomController::data = 100;
      std::cout << "DATA SET to 1000" << std::endl;
  }

#endif

const char *sign_and_send_script = "\
  async function foo() {\
    const { solana } = window;\
    const response = await solana.connect();\
  }\
";

const char* js_script = "\
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
        alert('Solana object not found! Get a Phantom Wallet 👻');\
        Module.phantom_status = -1;\
      }\
      }\
      catch (error){\
      console.error(error);\
      Module.phantom_status = -1;\
      }\
    }\
    Module.phantom_status = 0;\
    checkIfWalletIsConnected()\
    ";

namespace godot{

int PhantomController::data = 0;

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
      emit_signal("connection_established");
      break;
    }
    default:
      connected = false;
      emit_signal("connection_error");
      break;
  }

  phantom_state = State::IDLE;
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
  }

}

void PhantomController::_bind_methods(){
  ClassDB::add_signal("PhantomController", MethodInfo("connection_established"));
  ClassDB::add_signal("PhantomController", MethodInfo("connection_error"));
  ClassDB::bind_method(D_METHOD("connect_phantom"), &PhantomController::connect_phantom);
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
        std::cout << "Y from phantom" << std::endl;
        OS::get_singleton()->delay_msec(1000);
    }
    std::cout << "DATA IS: " << emscripten_run_script_int("Module.phantom_status") << std::endl;
 */   #endif
}

void PhantomController::sign_and_send_transaction(){
  #ifdef SOLANA_SDK_WEBBUILD

  emscripten_run_script(sign_and_send_script);

  #endif
}

PhantomController::~PhantomController(){
}
}