#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBACCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBACCOUNT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class HoneycombAccount : public Resource{
GDCLASS(HoneycombAccount, Resource)
private:
PackedByteArray address;
PackedByteArray program_id;
PackedByteArray discriminator;
Dictionary parsed_data;
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
void set_parsed_data(const Dictionary& val);
Dictionary get_parsed_data() const;
};
} // honeycomb_resource
} // godot
#endif
