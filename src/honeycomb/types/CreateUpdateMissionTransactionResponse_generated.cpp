#include "honeycomb/types/CreateUpdateMissionTransactionResponse_generated.hpp"

#include "honeycomb/types/HoneycombTransaction_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateUpdateMissionTransactionResponse::set_tx(const Variant& val) {
this->tx = val;
}

Variant CreateUpdateMissionTransactionResponse::get_tx() const {
return this->tx;
}

void CreateUpdateMissionTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_tx"), &CreateUpdateMissionTransactionResponse::get_tx);
ClassDB::bind_method(D_METHOD("set_tx", "value"), &CreateUpdateMissionTransactionResponse::set_tx);
ClassDB::add_property("CreateUpdateMissionTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "tx"), "set_tx", "get_tx");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateUpdateMissionTransactionResponse::to_dict);
}
Dictionary CreateUpdateMissionTransactionResponse::to_dict() {
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