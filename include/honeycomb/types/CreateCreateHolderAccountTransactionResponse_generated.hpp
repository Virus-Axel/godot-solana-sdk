#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEHOLDERACCOUNTTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATEHOLDERACCOUNTTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateCreateHolderAccountTransactionResponse : public Resource {
	GDCLASS(CreateCreateHolderAccountTransactionResponse, Resource)
private:
	Variant tx;
	PackedByteArray holderAccount;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_holderAccount(const PackedByteArray &val);
	PackedByteArray get_holderAccount() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
