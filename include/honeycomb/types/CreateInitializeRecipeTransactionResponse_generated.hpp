#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZERECIPETRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZERECIPETRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateInitializeRecipeTransactionResponse : public Resource{
GDCLASS(CreateInitializeRecipeTransactionResponse, Resource)
private:
Variant transactions;
PackedByteArray recipe;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_transactions(const Variant& val);
Variant get_transactions() const;
void set_recipe(const PackedByteArray& val);
PackedByteArray get_recipe() const;
};
} // honeycomb_resource
} // godot
#endif
