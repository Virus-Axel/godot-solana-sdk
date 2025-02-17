#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEWALLETINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEWALLETINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class UpdateWalletInput : public Resource{
GDCLASS(UpdateWalletInput, Resource)
private:
PackedStringArray add;
PackedStringArray remove;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_add(const PackedStringArray& val);
PackedStringArray get_add() const;
void set_remove(const PackedStringArray& val);
PackedStringArray get_remove() const;
};
} // honeycomb_resource
} // godot
#endif
