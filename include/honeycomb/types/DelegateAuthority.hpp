#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DELEGATEAUTHORITY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DELEGATEAUTHORITY
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
namespace godot{
namespace honeycomb_resource{
class DelegateAuthority : public Resource{
GDCLASS(DelegateAuthority, Resource)
private:
PackedByteArray address;
int32_t bump;
Variant project;
Variant authority;
Array delegations;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_address(const PackedByteArray& val);
PackedByteArray get_address();
void set_bump(const int32_t& val);
int32_t get_bump();
void set_project(const Variant& val);
Variant get_project();
void set_authority(const Variant& val);
Variant get_authority();
void set_delegations(const Array& val);
Array get_delegations();
};
}
}
#endif
