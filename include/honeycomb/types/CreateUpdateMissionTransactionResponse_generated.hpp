#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEUPDATEMISSIONTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEUPDATEMISSIONTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateUpdateMissionTransactionResponse : public Resource {
	GDCLASS(CreateUpdateMissionTransactionResponse, Resource)
private:
	Variant tx;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
