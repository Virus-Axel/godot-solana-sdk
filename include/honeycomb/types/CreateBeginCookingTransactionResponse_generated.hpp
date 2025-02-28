#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBEGINCOOKINGTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBEGINCOOKINGTRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateBeginCookingTransactionResponse : public Resource{
GDCLASS(CreateBeginCookingTransactionResponse, Resource)
private:
Variant transaction;
PackedByteArray cooking;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_transaction(const Variant& val);
Variant get_transaction() const;
void set_cooking(const PackedByteArray& val);
PackedByteArray get_cooking() const;
};
} // honeycomb_resource
} // godot
#endif
