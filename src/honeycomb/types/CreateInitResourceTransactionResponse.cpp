#include "honeycomb/types/CreateInitResourceTransactionResponse.hpp"

#include "honeycomb/types/HoneycombTransaction.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateInitResourceTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateInitResourceTransactionResponse::get_tx() const {
return this->tx;
}

void CreateInitResourceTransactionResponse::set_resource(const PackedByteArray& val) {
this->resource = val;
}

PackedByteArray CreateInitResourceTransactionResponse::get_resource() const {
return this->resource;
}

void CreateInitResourceTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateInitResourceTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateInitResourceTransactionResponse::set_tx);
ClassDB::add_property("CreateInitResourceTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("get_resource"), &CreateInitResourceTransactionResponse::get_resource);
ClassDB::bind_method(D_METHOD("set_resource", "value"), &CreateInitResourceTransactionResponse::set_resource);
ClassDB::add_property("CreateInitResourceTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "resource"), "set_resource", "get_resource");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateInitResourceTransactionResponse::to_dict);
}
Dictionary CreateInitResourceTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
res["resource"] = resource;
return res;
}
} // namespace honeycomb_resource
} // namespace godot