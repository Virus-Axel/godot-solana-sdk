#include "honeycomb/types/CreateInitMultipliersTransactionResponse.hpp"

#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateInitMultipliersTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateInitMultipliersTransactionResponse::get_tx() const {
return this->tx;
}

void CreateInitMultipliersTransactionResponse::set_multipliersAddress(const PackedByteArray& val) {
this->multipliersAddress = val;
}

PackedByteArray CreateInitMultipliersTransactionResponse::get_multipliersAddress() const {
return this->multipliersAddress;
}

void CreateInitMultipliersTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateInitMultipliersTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateInitMultipliersTransactionResponse::set_tx);
ClassDB::add_property("CreateInitMultipliersTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_multipliersAddress"), &CreateInitMultipliersTransactionResponse::get_multipliersAddress);
ClassDB::bind_method(D_METHOD("set_multipliersAddress", "value"), &CreateInitMultipliersTransactionResponse::set_multipliersAddress);
ClassDB::add_property("CreateInitMultipliersTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "multipliersAddress"), "set_multipliersAddress", "get_multipliersAddress");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateInitMultipliersTransactionResponse::to_dict);
}
Dictionary CreateInitMultipliersTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["multipliersAddress"] = multipliersAddress;
return res;
}
} // namespace honeycomb_resource
} // namespace godot