#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class TransactionResponse : public Resource {
	GDCLASS(TransactionResponse, Resource)
private:
	String signature;
	Dictionary error;
	String status;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_signature(const String &val);
	String get_signature() const;
	void set_error(const Dictionary &val);
	Dictionary get_error() const;
	void set_status(const String &val);
	String get_status() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
