#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CONTROLLEDMERKLETREES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CONTROLLEDMERKLETREES
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ControlledMerkleTrees : public Resource{
GDCLASS(ControlledMerkleTrees, Resource)
private:
int32_t active;
Dictionary schema;
Array merkle_trees;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_active(const int32_t& val);
int32_t get_active() const;
void set_schema(const Dictionary& val);
Dictionary get_schema() const;
void set_merkle_trees(const Array& val);
Array get_merkle_trees() const;
};
} // honeycomb_resource
} // godot
#endif
