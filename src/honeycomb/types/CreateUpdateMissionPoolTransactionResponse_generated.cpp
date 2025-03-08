#include "honeycomb/types/CreateUpdateMissionPoolTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateUpdateMissionPoolTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateUpdateMissionPoolTransactionResponse::get_tx() const {
return this->tx;
}

void CreateUpdateMissionPoolTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateUpdateMissionPoolTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateUpdateMissionPoolTransactionResponse::set_tx);
ClassDB::add_property("CreateUpdateMissionPoolTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateUpdateMissionPoolTransactionResponse::to_dict);
}
Dictionary CreateUpdateMissionPoolTransactionResponse::to_dict() {
Dictionary res;

          if (tx.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::HoneycombTransaction>(tx);
              if (ptr) {
                  res["tx"] = ptr->to_dict();
              }
          }
return res;
}
} // namespace honeycomb_resource
} // namespace godot