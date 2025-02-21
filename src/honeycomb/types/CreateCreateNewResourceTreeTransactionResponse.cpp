#include "honeycomb/types/CreateCreateNewResourceTreeTransactionResponse.hpp"

#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateCreateNewResourceTreeTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateCreateNewResourceTreeTransactionResponse::get_tx() const {
return this->tx;
}

void CreateCreateNewResourceTreeTransactionResponse::set_tree(const PackedByteArray& val) {
this->tree = val;
}

PackedByteArray CreateCreateNewResourceTreeTransactionResponse::get_tree() const {
return this->tree;
}

void CreateCreateNewResourceTreeTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateCreateNewResourceTreeTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateCreateNewResourceTreeTransactionResponse::set_tx);
ClassDB::add_property("CreateCreateNewResourceTreeTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_tree"), &CreateCreateNewResourceTreeTransactionResponse::get_tree);
ClassDB::bind_method(D_METHOD("set_tree", "value"), &CreateCreateNewResourceTreeTransactionResponse::set_tree);
ClassDB::add_property("CreateCreateNewResourceTreeTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "tree"), "set_tree", "get_tree");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateCreateNewResourceTreeTransactionResponse::to_dict);
}
Dictionary CreateCreateNewResourceTreeTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["tree"] = tree;
return res;
}
} // namespace honeycomb_resource
} // namespace godot