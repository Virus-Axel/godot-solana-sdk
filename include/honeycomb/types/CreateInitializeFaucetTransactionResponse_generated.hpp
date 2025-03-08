#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZEFAUCETTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITIALIZEFAUCETTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateInitializeFaucetTransactionResponse : public Resource {
	GDCLASS(CreateInitializeFaucetTransactionResponse, Resource)
private:
	Variant tx;
	PackedByteArray faucet;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_faucet(const PackedByteArray &val);
	PackedByteArray get_faucet() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
