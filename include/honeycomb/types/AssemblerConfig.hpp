#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ASSEMBLERCONFIG
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class AssemblerConfig : public Resource{
GDCLASS(AssemblerConfig, Resource)
private:
PackedByteArray address;
PackedByteArray program_id;
PackedByteArray discriminator;
String ticker;
Variant project;
PackedStringArray order;
Variant merkle_trees;
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
void set_ticker(const String& val);
String get_ticker() const;
void set_project(const Variant& val);
Variant get_project() const;
void set_order(const PackedStringArray& val);
PackedStringArray get_order() const;
void set_merkle_trees(const Variant& val);
Variant get_merkle_trees() const;
};
} // honeycomb_resource
} // godot
#endif
