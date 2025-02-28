#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONPOOL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONPOOL
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class MissionPool : public Resource{
GDCLASS(MissionPool, Resource)
private:
PackedByteArray address;
PackedByteArray program_id;
PackedByteArray discriminator;
int32_t bump;
Variant project;
String name;
PackedByteArray factionsMerkleRoot;
int32_t randomizerRound;
Array characterModels;
PackedInt32Array guildKits;
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
void set_project(const Variant& val);
Variant get_project() const;
void set_name(const String& val);
String get_name() const;
void set_factionsMerkleRoot(const PackedByteArray& val);
PackedByteArray get_factionsMerkleRoot() const;
void set_randomizerRound(const int32_t& val);
int32_t get_randomizerRound() const;
void set_characterModels(const Array& val);
Array get_characterModels() const;
void set_guildKits(const PackedInt32Array& val);
PackedInt32Array get_guildKits() const;
};
} // honeycomb_resource
} // godot
#endif
