#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATESTAKINGPOOLTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATESTAKINGPOOLTRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateCreateStakingPoolTransactionResponse : public Resource{
GDCLASS(CreateCreateStakingPoolTransactionResponse, Resource)
private:
Variant transactions;
PackedByteArray stakingPoolAddress;
PackedByteArray multipliersAddress;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_transactions(const Variant& val);
Variant get_transactions() const;
void set_stakingPoolAddress(const PackedByteArray& val);
PackedByteArray get_stakingPoolAddress() const;
void set_multipliersAddress(const PackedByteArray& val);
PackedByteArray get_multipliersAddress() const;
};
} // honeycomb_resource
} // godot
#endif
