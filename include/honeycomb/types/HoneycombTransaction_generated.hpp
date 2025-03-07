#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBTRANSACTION
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class HoneycombTransaction : public Resource {
	GDCLASS(HoneycombTransaction, Resource)
private:
	PackedByteArray transaction;
	String blockhash;
	int32_t lastValidBlockHeight;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_transaction(const PackedByteArray &val);
	PackedByteArray get_transaction() const;
	void set_blockhash(const String &val);
	String get_blockhash() const;
	void set_lastValidBlockHeight(const int32_t &val);
	int32_t get_lastValidBlockHeight() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
