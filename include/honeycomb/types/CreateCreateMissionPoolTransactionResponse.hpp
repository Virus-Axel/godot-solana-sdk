#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONPOOLTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEMISSIONPOOLTRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateCreateMissionPoolTransactionResponse : public Resource{
GDCLASS(CreateCreateMissionPoolTransactionResponse, Resource)
private:
Variant tx;
PackedByteArray missionPoolAddress;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_tx(const Variant& val);
Variant get_tx() const;
void set_missionPoolAddress(const PackedByteArray& val);
PackedByteArray get_missionPoolAddress() const;
};
} // honeycomb_resource
} // godot
#endif
