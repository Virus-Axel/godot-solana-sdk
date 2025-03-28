#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITMULTIPLIERSTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITMULTIPLIERSTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateInitMultipliersTransactionResponse : public Resource {
	GDCLASS(CreateInitMultipliersTransactionResponse, Resource)
private:
	Variant tx;
	PackedByteArray multipliersAddress;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_multipliersAddress(const PackedByteArray &val);
	PackedByteArray get_multipliersAddress() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
