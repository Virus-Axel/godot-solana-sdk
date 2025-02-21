#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEASSEMBLERCONFIGTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEASSEMBLERCONFIGTRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateAssemblerConfigTransactionResponse : public Resource{
GDCLASS(CreateAssemblerConfigTransactionResponse, Resource)
private:
Variant tx;
PackedByteArray assemblerConfig;
PackedByteArray treeAddress;
int32_t proofBytes;
int32_t space;
float cost;
int32_t maxTreeCapacity;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_tx(const Variant& val);
Variant get_tx() const;
void set_assemblerConfig(const PackedByteArray& val);
PackedByteArray get_assemblerConfig() const;
void set_treeAddress(const PackedByteArray& val);
PackedByteArray get_treeAddress() const;
void set_proofBytes(const int32_t& val);
int32_t get_proofBytes() const;
void set_space(const int32_t& val);
int32_t get_space() const;
void set_cost(const float& val);
float get_cost() const;
void set_maxTreeCapacity(const int32_t& val);
int32_t get_maxTreeCapacity() const;
};
} // honeycomb_resource
} // godot
#endif
