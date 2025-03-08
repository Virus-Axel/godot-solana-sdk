#include "honeycomb/types/CreateInitializeRecipeTransactionResponse_generated.hpp"

#include "honeycomb/types/Transactions_generated.hpp"

namespace godot {
namespace honeycomb_resource {

void CreateInitializeRecipeTransactionResponse::set_transactions(const Variant& val) {
this->transactions = val;
}

Variant CreateInitializeRecipeTransactionResponse::get_transactions() const {
return this->transactions;
}

void CreateInitializeRecipeTransactionResponse::set_recipe(const PackedByteArray& val) {
this->recipe = val;
}

PackedByteArray CreateInitializeRecipeTransactionResponse::get_recipe() const {
return this->recipe;
}

void CreateInitializeRecipeTransactionResponse::_bind_methods() {
ClassDB::bind_method(D_METHOD("get_transactions"), &CreateInitializeRecipeTransactionResponse::get_transactions);
ClassDB::bind_method(D_METHOD("set_transactions", "value"), &CreateInitializeRecipeTransactionResponse::set_transactions);
ClassDB::add_property("CreateInitializeRecipeTransactionResponse", PropertyInfo(Variant::Type::OBJECT, "transactions"), "set_transactions", "get_transactions");
ClassDB::bind_method(D_METHOD("get_recipe"), &CreateInitializeRecipeTransactionResponse::get_recipe);
ClassDB::bind_method(D_METHOD("set_recipe", "value"), &CreateInitializeRecipeTransactionResponse::set_recipe);
ClassDB::add_property("CreateInitializeRecipeTransactionResponse", PropertyInfo(Variant::Type::PACKED_BYTE_ARRAY, "recipe"), "set_recipe", "get_recipe");
ClassDB::bind_method(D_METHOD("to_dict"), &CreateInitializeRecipeTransactionResponse::to_dict);
}
Dictionary CreateInitializeRecipeTransactionResponse::to_dict() {
Dictionary res;

          if (transactions.get_type() != Variant::NIL) {
              auto* ptr = Object::cast_to<godot::honeycomb_resource::Transactions>(transactions);
              if (ptr) {
                  res["transactions"] = ptr->to_dict();
              }
          }
res["recipe"] = recipe;
return res;
}
} // namespace honeycomb_resource
} // namespace godot