#include "Transaction.h"

namespace godot{

class Transaction : public Resource{
GDCLASS(Transaction, Resource)
protected:
static void _bind_methods();
public:
void set_transaction(const PackedByteArray& val);
PackedByteArray& get_transaction();
void set_blockhash(const String& val);
String& get_blockhash();
void set_lastValidBlockHeight(const int32_t& val);
int32_t& get_lastValidBlockHeight();
};
} // godot