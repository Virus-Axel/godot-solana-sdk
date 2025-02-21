#include "honeycomb/types/CreateCreateStakingPoolTransactionResponse.hpp"

#include "honeycomb/types/Transactions.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateStakingPoolTransactionResponse::set_transactions(const Variant& val) {
this->transactions = val;
}

Variant CreateCreateStakingPoolTransactionResponse::get_transactions() const {
return this->transactions;
}

void CreateCreateStakingPoolTransactionResponse::set_stakingPoolAddress(const PackedByteArray& val) {
this->stakingPoolAddress = val;
}

PackedByteArray CreateCreateStakingPoolTransactionResponse::get_stakingPoolAddress() const {
return this->stakingPoolAddress;
}

void CreateCreateStakingPoolTransactionResponse::set_multipliersAddress(const PackedByteArray& val) {
this->multipliersAddress = val;
}

PackedByteArray CreateCreateStakingPoolTransactionResponse::get_multipliersAddress() const {
return this->multipliersAddress;
}

void CreateCreateStakingPoolTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_transactions"), &CreateCreateStakingPoolTransactionResponse::get_transactions);
ClassDB::bind_method(D_METHOD("set_transactions", "value"), &CreateCreateStakingPoolTransactionResponse::set_transactions);
ClassDB::add_property("CreateCreateStakingPoolTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "transactions"), "set_transactions", "get_transactions");
ClassDB::bind_method(D_METHOD("get_stakingPoolAddress"), &CreateCreateStakingPoolTransactionResponse::get_stakingPoolAddress);
ClassDB::bind_method(D_METHOD("set_stakingPoolAddress", "value"), &CreateCreateStakingPoolTransactionResponse::set_stakingPoolAddress);
ClassDB::add_property("CreateCreateStakingPoolTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "stakingPoolAddress"), "set_stakingPoolAddress", "get_stakingPoolAddress");
ClassDB::bind_method(D_METHOD("get_multipliersAddress"), &CreateCreateStakingPoolTransactionResponse::get_multipliersAddress);
ClassDB::bind_method(D_METHOD("set_multipliersAddress", "value"), &CreateCreateStakingPoolTransactionResponse::set_multipliersAddress);
ClassDB::add_property("CreateCreateStakingPoolTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "multipliersAddress"), "set_multipliersAddress", "get_multipliersAddress");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateStakingPoolTransactionResponse::to_dict);
}
Dictionary CreateCreateStakingPoolTransactionResponse::to_dict() {
Dictionary res;

          if (transactions.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Transactions>(transactions);
              if (ptr) {
                  res["transactions"] = ptr->to_dict();
              }
          }
res["stakingPoolAddress"] = stakingPoolAddress;
res["multipliersAddress"] = multipliersAddress;
return res;
}
} // namespace honeycomb_resource
} // namespace godot