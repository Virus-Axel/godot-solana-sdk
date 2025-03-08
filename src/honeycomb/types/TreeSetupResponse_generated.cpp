#include "honeycomb/types/TreeSetupResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void TreeSetupResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant TreeSetupResponse::get_tx() const {
return this->tx;
}

void TreeSetupResponse::set_treeAddress(const PackedByteArray& val) {
this->treeAddress = val;
}

PackedByteArray TreeSetupResponse::get_treeAddress() const {
return this->treeAddress;
}

void TreeSetupResponse::set_proofBytes(const int32_t& val) {
this->proofBytes = val;
}

int32_t TreeSetupResponse::get_proofBytes() const {
return this->proofBytes;
}

void TreeSetupResponse::set_space(const int32_t& val) {
this->space = val;
}

int32_t TreeSetupResponse::get_space() const {
return this->space;
}

void TreeSetupResponse::set_cost(const float& val) {
this->cost = val;
}

float TreeSetupResponse::get_cost() const {
return this->cost;
}

void TreeSetupResponse::set_maxTreeCapacity(const int32_t& val) {
this->maxTreeCapacity = val;
}

int32_t TreeSetupResponse::get_maxTreeCapacity() const {
return this->maxTreeCapacity;
}

void TreeSetupResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &TreeSetupResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &TreeSetupResponse::set_tx);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_treeAddress"), &TreeSetupResponse::get_treeAddress);
ClassDB::bind_method(D_METHOD("set_treeAddress", "value"), &TreeSetupResponse::set_treeAddress);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "treeAddress"), "set_treeAddress", "get_treeAddress");
ClassDB::bind_method(D_METHOD("get_proofBytes"), &TreeSetupResponse::get_proofBytes);
ClassDB::bind_method(D_METHOD("set_proofBytes", "value"), &TreeSetupResponse::set_proofBytes);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::INT, "proofBytes"), "set_proofBytes", "get_proofBytes");
ClassDB::bind_method(D_METHOD("get_space"), &TreeSetupResponse::get_space);
ClassDB::bind_method(D_METHOD("set_space", "value"), &TreeSetupResponse::set_space);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::INT, "space"), "set_space", "get_space");
ClassDB::bind_method(D_METHOD("get_cost"), &TreeSetupResponse::get_cost);
ClassDB::bind_method(D_METHOD("set_cost", "value"), &TreeSetupResponse::set_cost);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::FLOAT, "cost"), "set_cost", "get_cost");
ClassDB::bind_method(D_METHOD("get_maxTreeCapacity"), &TreeSetupResponse::get_maxTreeCapacity);
ClassDB::bind_method(D_METHOD("set_maxTreeCapacity", "value"), &TreeSetupResponse::set_maxTreeCapacity);
ClassDB::add_property("TreeSetupResponse", PropertyInfo(Variant::Type::INT, "maxTreeCapacity"), "set_maxTreeCapacity", "get_maxTreeCapacity");
ClassDB::bind_method(D_METHOD("to_dict"), &TreeSetupResponse::to_dict);
}
Dictionary TreeSetupResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["treeAddress"] = treeAddress;
res["proofBytes"] = proofBytes;
res["space"] = space;
res["cost"] = cost;
res["maxTreeCapacity"] = maxTreeCapacity;
return res;
}
} // namespace honeycomb_resource
} // namespace godot