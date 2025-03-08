#include "honeycomb/types/CreateCreateMissionPoolTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateMissionPoolTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateCreateMissionPoolTransactionResponse::get_tx() const {
return this->tx;
}

void CreateCreateMissionPoolTransactionResponse::set_missionPoolAddress(const PackedByteArray& val) {
this->missionPoolAddress = val;
}

PackedByteArray CreateCreateMissionPoolTransactionResponse::get_missionPoolAddress() const {
return this->missionPoolAddress;
}

void CreateCreateMissionPoolTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateMissionPoolTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateMissionPoolTransactionResponse::set_tx);
ClassDB::add_property("CreateCreateMissionPoolTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_missionPoolAddress"), &CreateCreateMissionPoolTransactionResponse::get_missionPoolAddress);
ClassDB::bind_method(D_METHOD("set_missionPoolAddress", "value"), &CreateCreateMissionPoolTransactionResponse::set_missionPoolAddress);
ClassDB::add_property("CreateCreateMissionPoolTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "missionPoolAddress"), "set_missionPoolAddress", "get_missionPoolAddress");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateMissionPoolTransactionResponse::to_dict);
}
Dictionary CreateCreateMissionPoolTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["missionPoolAddress"] = missionPoolAddress;
return res;
}
} // namespace honeycomb_resource
} // namespace godot