#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONSOPTIONS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SENDTRANSACTIONSOPTIONS
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class SendTransactionsOptions : public Resource {
	GDCLASS(SendTransactionsOptions, Resource)
private:
	String commitment;
	bool skipPreflight;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_commitment(const String &val);
	String get_commitment() const;
	void set_skipPreflight(const bool &val);
	bool get_skipPreflight() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
