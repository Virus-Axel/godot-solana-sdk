#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITRESOURCETRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEINITRESOURCETRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateInitResourceTransactionResponse : public Resource {
	GDCLASS(CreateInitResourceTransactionResponse, Resource)
private:
	Variant tx;
	PackedByteArray resource;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_resource(const PackedByteArray &val);
	PackedByteArray get_resource() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
