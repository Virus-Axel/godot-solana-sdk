#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class HoneycombTransaction : public Resource{
GDCLASS(HoneycombTransaction, Resource)
private:
PackedByteArray transaction;
String blockhash;
int32_t lastValidBlockHeight;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_transaction(const PackedByteArray& val);
PackedByteArray get_transaction();
void set_blockhash(const String& val);
String get_blockhash();
void set_lastValidBlockHeight(const int32_t& val);
int32_t get_lastValidBlockHeight();
};
} // honeycomb_resource
} // godot
#endif