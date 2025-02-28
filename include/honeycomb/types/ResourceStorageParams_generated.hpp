#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCESTORAGEPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCESTORAGEPARAMS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class ResourceStorageParams : public Resource{
GDCLASS(ResourceStorageParams, Resource)
private:
Variant merkle_trees;
int64_t promiseSupply;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_merkle_trees(const Variant& val);
Variant get_merkle_trees() const;
void set_promiseSupply(const int64_t& val);
int64_t get_promiseSupply() const;
};
} // honeycomb_resource
} // godot
#endif
