#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERMODEL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERMODEL
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterModel : public Resource{
GDCLASS(CharacterModel, Resource)
private:
PackedByteArray address;
PackedByteArray program_id;
PackedByteArray discriminator;
int32_t bump;
Variant key;
Variant project;
Dictionary config;
Array attributes;
Variant merkle_trees;
Variant cooldown;
PackedStringArray equipableCriteria;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_program_id(const PackedByteArray& val);
PackedByteArray get_program_id() const;
void set_discriminator(const PackedByteArray& val);
PackedByteArray get_discriminator() const;
void set_bump(const int32_t& val);
int32_t get_bump() const;
void set_key(const Variant& val);
Variant get_key() const;
void set_project(const Variant& val);
Variant get_project() const;
void set_config(const Dictionary& val);
Dictionary get_config() const;
void set_attributes(const Array& val);
Array get_attributes() const;
void set_merkle_trees(const Variant& val);
Variant get_merkle_trees() const;
void set_cooldown(const Variant& val);
Variant get_cooldown() const;
void set_equipableCriteria(const PackedStringArray& val);
PackedStringArray get_equipableCriteria() const;
};
} // honeycomb_resource
} // godot
#endif
