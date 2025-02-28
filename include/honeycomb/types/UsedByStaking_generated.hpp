#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYSTAKING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYSTAKING
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class UsedByStaking : public Resource{
GDCLASS(UsedByStaking, Resource)
private:
Variant pool;
Variant staker;
int32_t stakedAt;
int32_t claimedAt;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_pool(const Variant& val);
Variant get_pool() const;
void set_staker(const Variant& val);
Variant get_staker() const;
void set_stakedAt(const int32_t& val);
int32_t get_stakedAt() const;
void set_claimedAt(const int32_t& val);
int32_t get_claimedAt() const;
};
} // honeycomb_resource
} // godot
#endif
