#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREELEAF
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_TREELEAF
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class TreeLeaf : public Resource{
GDCLASS(TreeLeaf, Resource)
private:
PackedByteArray tree;
int64_t index;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_tree(const PackedByteArray& val);
PackedByteArray get_tree() const;
void set_index(const int64_t& val);
int64_t get_index() const;
};
} // honeycomb_resource
} // godot
#endif
