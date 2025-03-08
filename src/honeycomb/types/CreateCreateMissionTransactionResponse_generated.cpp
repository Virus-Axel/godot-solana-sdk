#include "honeycomb/types/CreateCreateMissionTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateMissionTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateCreateMissionTransactionResponse::get_tx() const {
return this->tx;
}

void CreateCreateMissionTransactionResponse::set_missionAddress(const PackedByteArray& val) {
this->missionAddress = val;
}

PackedByteArray CreateCreateMissionTransactionResponse::get_missionAddress() const {
return this->missionAddress;
}

void CreateCreateMissionTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateMissionTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateMissionTransactionResponse::set_tx);
ClassDB::add_property("CreateCreateMissionTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_missionAddress"), &CreateCreateMissionTransactionResponse::get_missionAddress);
ClassDB::bind_method(D_METHOD("set_missionAddress", "value"), &CreateCreateMissionTransactionResponse::set_missionAddress);
ClassDB::add_property("CreateCreateMissionTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "missionAddress"), "set_missionAddress", "get_missionAddress");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateMissionTransactionResponse::to_dict);
}
Dictionary CreateCreateMissionTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["missionAddress"] = missionAddress;
return res;
}
} // namespace honeycomb_resource
} // namespace godot