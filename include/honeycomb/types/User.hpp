#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class User : public Resource{
GDCLASS(User, Resource)
private:
int32_t id;
PackedByteArray address;
PackedByteArray tree_id;
int64_t leaf_idx;
Variant proof;
Variant info;
Variant socialInfo;
Variant wallets;
Array profiles;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_id(const int32_t& val);
int32_t get_id() const;
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_tree_id(const PackedByteArray& val);
PackedByteArray get_tree_id() const;
void set_leaf_idx(const int64_t& val);
int64_t get_leaf_idx() const;
void set_proof(const Variant& val);
Variant get_proof() const;
void set_info(const Variant& val);
Variant get_info() const;
void set_socialInfo(const Variant& val);
Variant get_socialInfo() const;
void set_wallets(const Variant& val);
Variant get_wallets() const;
void set_profiles(const Array& val);
Array get_profiles() const;
};
} // honeycomb_resource
} // godot
#endif
