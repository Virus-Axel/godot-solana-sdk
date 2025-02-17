#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_FAUCET
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_FAUCET
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Faucet : public Resource{
GDCLASS(Faucet, Resource)
private:
PackedByteArray address;
Variant project;
Variant resource;
int64_t amount;
int64_t repeatInterval;
int64_t lastClaimed;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_project(const Variant& val);
Variant get_project() const;
void set_resource(const Variant& val);
Variant get_resource() const;
void set_amount(const int64_t& val);
int64_t get_amount() const;
void set_repeatInterval(const int64_t& val);
int64_t get_repeatInterval() const;
void set_lastClaimed(const int64_t& val);
int64_t get_lastClaimed() const;
};
} // honeycomb_resource
} // godot
#endif
