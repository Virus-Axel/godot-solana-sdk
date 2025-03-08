#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECHARACTERMODELTRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECHARACTERMODELTRANSACTIONRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class CreateCharacterModelTransactionResponse : public Resource {
	GDCLASS(CreateCharacterModelTransactionResponse, Resource)
private:
	Variant tx;
	PackedByteArray characterModel;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_tx(const Variant &val);
	Variant get_tx() const;
	void set_characterModel(const PackedByteArray &val);
	PackedByteArray get_characterModel() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
