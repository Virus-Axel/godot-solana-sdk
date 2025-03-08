#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASPARAMSMPLBUBBLEGUM
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASPARAMSMPLBUBBLEGUM
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class MintAsParamsMplBubblegum : public Resource{
GDCLASS(MintAsParamsMplBubblegum, Resource)
private:
Variant merkleTree;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_merkleTree(const Variant& val);
Variant get_merkleTree() const;
};
} // honeycomb_resource
} // godot
#endif
