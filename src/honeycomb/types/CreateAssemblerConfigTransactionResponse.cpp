#include "honeycomb/types/CreateAssemblerConfigTransactionResponse.hpp"

#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateAssemblerConfigTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateAssemblerConfigTransactionResponse::get_tx() const {
return this->tx;
}

void CreateAssemblerConfigTransactionResponse::set_assemblerConfig(const PackedByteArray& val) {
this->assemblerConfig = val;
}

PackedByteArray CreateAssemblerConfigTransactionResponse::get_assemblerConfig() const {
return this->assemblerConfig;
}

void CreateAssemblerConfigTransactionResponse::set_treeAddress(const PackedByteArray& val) {
this->treeAddress = val;
}

PackedByteArray CreateAssemblerConfigTransactionResponse::get_treeAddress() const {
return this->treeAddress;
}

void CreateAssemblerConfigTransactionResponse::set_proofBytes(const int32_t& val) {
this->proofBytes = val;
}

int32_t CreateAssemblerConfigTransactionResponse::get_proofBytes() const {
return this->proofBytes;
}

void CreateAssemblerConfigTransactionResponse::set_space(const int32_t& val) {
this->space = val;
}

int32_t CreateAssemblerConfigTransactionResponse::get_space() const {
return this->space;
}

void CreateAssemblerConfigTransactionResponse::set_cost(const float& val) {
this->cost = val;
}

float CreateAssemblerConfigTransactionResponse::get_cost() const {
return this->cost;
}

void CreateAssemblerConfigTransactionResponse::set_maxTreeCapacity(const int32_t& val) {
this->maxTreeCapacity = val;
}

int32_t CreateAssemblerConfigTransactionResponse::get_maxTreeCapacity() const {
return this->maxTreeCapacity;
}

void CreateAssemblerConfigTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateAssemblerConfigTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateAssemblerConfigTransactionResponse::set_tx);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_assemblerConfig"), &CreateAssemblerConfigTransactionResponse::get_assemblerConfig);
ClassDB::bind_method(D_METHOD("set_assemblerConfig", "value"), &CreateAssemblerConfigTransactionResponse::set_assemblerConfig);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "assemblerConfig"), "set_assemblerConfig", "get_assemblerConfig");
ClassDB::bind_method(D_METHOD("get_treeAddress"), &CreateAssemblerConfigTransactionResponse::get_treeAddress);
ClassDB::bind_method(D_METHOD("set_treeAddress", "value"), &CreateAssemblerConfigTransactionResponse::set_treeAddress);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "treeAddress"), "set_treeAddress", "get_treeAddress");
ClassDB::bind_method(D_METHOD("get_proofBytes"), &CreateAssemblerConfigTransactionResponse::get_proofBytes);
ClassDB::bind_method(D_METHOD("set_proofBytes", "value"), &CreateAssemblerConfigTransactionResponse::set_proofBytes);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::INT, "proofBytes"), "set_proofBytes", "get_proofBytes");
ClassDB::bind_method(D_METHOD("get_space"), &CreateAssemblerConfigTransactionResponse::get_space);
ClassDB::bind_method(D_METHOD("set_space", "value"), &CreateAssemblerConfigTransactionResponse::set_space);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::INT, "space"), "set_space", "get_space");
ClassDB::bind_method(D_METHOD("get_cost"), &CreateAssemblerConfigTransactionResponse::get_cost);
ClassDB::bind_method(D_METHOD("set_cost", "value"), &CreateAssemblerConfigTransactionResponse::set_cost);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::FLOAT, "cost"), "set_cost", "get_cost");
ClassDB::bind_method(D_METHOD("get_maxTreeCapacity"), &CreateAssemblerConfigTransactionResponse::get_maxTreeCapacity);
ClassDB::bind_method(D_METHOD("set_maxTreeCapacity", "value"), &CreateAssemblerConfigTransactionResponse::set_maxTreeCapacity);
ClassDB::add_property("CreateAssemblerConfigTransactionResponse", PropertyInfo(Variant::Type::INT, "maxTreeCapacity"), "set_maxTreeCapacity", "get_maxTreeCapacity");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateAssemblerConfigTransactionResponse::to_dict);
}
Dictionary CreateAssemblerConfigTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["assemblerConfig"] = assemblerConfig;
res["treeAddress"] = treeAddress;
res["proofBytes"] = proofBytes;
res["space"] = space;
res["cost"] = cost;
res["maxTreeCapacity"] = maxTreeCapacity;
return res;
}
} // namespace honeycomb_resource
} // namespace godot