#include "honeycomb/types/Holding.hpp"

#include "pubkey.hpp"
#include "honeycomb/types/Proof.hpp"

namespace godot {
namespace honeycomb_resource {

void Holding::set_address(const PackedByteArray& val) {
this->address = val;
}

PackedByteArray Holding::get_address() const {
return this->address;
}

void Holding::set_tree_id(const PackedByteArray& val) {
this->tree_id = val;
}

PackedByteArray Holding::get_tree_id() const {
return this->tree_id;
}

void Holding::set_leaf_idx(const int64_t& val) {
this->leaf_idx = val;
}

int64_t Holding::get_leaf_idx() const {
return this->leaf_idx;
}

void Holding::set_holder(const Variant& val) {
this->holder = val;
}

Variant Holding::get_holder() const {
return this->holder;
}

void Holding::set_balance(const int64_t& val) {
this->balance = val;
}

int64_t Holding::get_balance() const {
return this->balance;
}

void Holding::set_characteristics(const Array& val) {
this->characteristics = val;
}

Array Holding::get_characteristics() const {
return this->characteristics;
}

void Holding::set_proof(const Variant& val) {
this->proof = val;
}

Variant Holding::get_proof() const {
return this->proof;
}

void Holding::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_address"), &Holding::get_address);
ClassDB::bind_method(D_METHOD("set_address", "value"), &Holding::set_address);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "address"), "set_address", "get_address");
ClassDB::bind_method(D_METHOD("get_tree_id"), &Holding::get_tree_id);
ClassDB::bind_method(D_METHOD("set_tree_id", "value"), &Holding::set_tree_id);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree_id"), "set_tree_id", "get_tree_id");
ClassDB::bind_method(D_METHOD("get_leaf_idx"), &Holding::get_leaf_idx);
ClassDB::bind_method(D_METHOD("set_leaf_idx", "value"), &Holding::set_leaf_idx);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::INT, "leaf_idx"), "set_leaf_idx", "get_leaf_idx");
ClassDB::bind_method(D_METHOD("get_holder"), &Holding::get_holder);
ClassDB::bind_method(D_METHOD("set_holder", "value"), &Holding::set_holder);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::OBJECT, "holder"), "set_holder", "get_holder");
ClassDB::bind_method(D_METHOD("get_balance"), &Holding::get_balance);
ClassDB::bind_method(D_METHOD("set_balance", "value"), &Holding::set_balance);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::INT, "balance"), "set_balance", "get_balance");
ClassDB::bind_method(D_METHOD("get_characteristics"), &Holding::get_characteristics);
ClassDB::bind_method(D_METHOD("set_characteristics", "value"), &Holding::set_characteristics);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::ARRAY, "characteristics"), "set_characteristics", "get_characteristics");
ClassDB::bind_method(D_METHOD("get_proof"), &Holding::get_proof);
ClassDB::bind_method(D_METHOD("set_proof", "value"), &Holding::set_proof);
ClassDB::add_property("Holding", PropertyInfo(Variant::Type::OBJECT, "proof"), "set_proof", "get_proof");
ClassDB::bind_method(D_METHOD("to_dict"), &Holding::to_dict);
}
Dictionary Holding::to_dict() {
Dictionary res;
res["address"] = address;
res["tree_id"] = tree_id;
res["leaf_idx"] = leaf_idx;
if (holder.has_method("to_string")) {
res["holder"] = holder.call("to_string");
} else {
res["holder"] = holder;
}
res["balance"] = balance;
res["characteristics"] = characteristics;

          if (proof.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Proof>(proof);
              if (ptr) {
                  res["proof"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot