#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTION
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"

namespace godot{
namespace honeycomb_resource{

class Transaction : public Resource{
GDCLASS(Transaction, Resource)
private:
PackedByteArray transaction;
String blockhash;
int32_t lastValidBlockHeight;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_transaction(const PackedByteArray& val);
PackedByteArray& get_transaction();
void set_blockhash(const String& val);
String& get_blockhash();
void set_lastValidBlockHeight(const int32_t& val);
int32_t& get_lastValidBlockHeight();
};
} // honeycomb_resource
} // godot
#endif