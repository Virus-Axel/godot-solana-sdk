#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROFILE
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Profile : public Resource{
GDCLASS(Profile, Resource)
private:
PackedByteArray address;
PackedByteArray tree_id;
int64_t leaf_idx;
Variant proof;
Variant project;
int32_t userId;
String identity;
Variant info;
Variant platformData;
Array customData;
Variant user;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_tree_id(const PackedByteArray& val);
PackedByteArray get_tree_id() const;
void set_leaf_idx(const int64_t& val);
int64_t get_leaf_idx() const;
void set_proof(const Variant& val);
Variant get_proof() const;
void set_project(const Variant& val);
Variant get_project() const;
void set_userId(const int32_t& val);
int32_t get_userId() const;
void set_identity(const String& val);
String get_identity() const;
void set_info(const Variant& val);
Variant get_info() const;
void set_platformData(const Variant& val);
Variant get_platformData() const;
void set_customData(const Array& val);
Array get_customData() const;
void set_user(const Variant& val);
Variant get_user() const;
};
} // honeycomb_resource
} // godot
#endif
