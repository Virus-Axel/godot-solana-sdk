#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATENEWRESOURCETREETRANSACTIONRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATECREATENEWRESOURCETREETRANSACTIONRESPONSE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateCreateNewResourceTreeTransactionResponse : public Resource{
GDCLASS(CreateCreateNewResourceTreeTransactionResponse, Resource)
private:
Variant tx;
PackedByteArray tree;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_tx(const Variant& val);
Variant get_tx() const;
void set_tree(const PackedByteArray& val);
PackedByteArray get_tree() const;
};
} // honeycomb_resource
} // godot
#endif
