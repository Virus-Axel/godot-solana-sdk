#include "honeycomb/types/CreateInitializeFaucetTransactionResponse.hpp"

#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateInitializeFaucetTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateInitializeFaucetTransactionResponse::get_tx() const {
return this->tx;
}

void CreateInitializeFaucetTransactionResponse::set_faucet(const PackedByteArray& val) {
this->faucet = val;
}

PackedByteArray CreateInitializeFaucetTransactionResponse::get_faucet() const {
return this->faucet;
}

void CreateInitializeFaucetTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateInitializeFaucetTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateInitializeFaucetTransactionResponse::set_tx);
ClassDB::add_property("CreateInitializeFaucetTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_faucet"), &CreateInitializeFaucetTransactionResponse::get_faucet);
ClassDB::bind_method(D_METHOD("set_faucet", "value"), &CreateInitializeFaucetTransactionResponse::set_faucet);
ClassDB::add_property("CreateInitializeFaucetTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "faucet"), "set_faucet", "get_faucet");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateInitializeFaucetTransactionResponse::to_dict);
}
Dictionary CreateInitializeFaucetTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["faucet"] = faucet;
return res;
}
} // namespace honeycomb_resource
} // namespace godot