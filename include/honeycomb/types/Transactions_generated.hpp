#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONS
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Transactions : public Resource {
	GDCLASS(Transactions, Resource)
private:
	Array transactions;
	String blockhash;
	int32_t lastValidBlockHeight;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_transactions(const Array &val);
	Array get_transactions() const;
	void set_blockhash(const String &val);
	String get_blockhash() const;
	void set_lastValidBlockHeight(const int32_t &val);
	int32_t get_lastValidBlockHeight() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
