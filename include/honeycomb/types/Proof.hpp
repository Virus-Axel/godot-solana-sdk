#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROOF
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROOF
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Proof : public Resource{
GDCLASS(Proof, Resource)
private:
PackedByteArray root;
Array proof;
int64_t node_index;
PackedByteArray leaf;
int32_t maxDepth;
int64_t leaf_index;
PackedByteArray tree_id;
int32_t canopy_depth;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_root(const PackedByteArray& val);
PackedByteArray get_root() const;
void set_proof(const Array& val);
Array get_proof() const;
void set_node_index(const int64_t& val);
int64_t get_node_index() const;
void set_leaf(const PackedByteArray& val);
PackedByteArray get_leaf() const;
void set_maxDepth(const int32_t& val);
int32_t get_maxDepth() const;
void set_leaf_index(const int64_t& val);
int64_t get_leaf_index() const;
void set_tree_id(const PackedByteArray& val);
PackedByteArray get_tree_id() const;
void set_canopy_depth(const int32_t& val);
int32_t get_canopy_depth() const;
};
} // honeycomb_resource
} // godot
#endif
