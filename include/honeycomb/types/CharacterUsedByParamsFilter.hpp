#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERUSEDBYPARAMSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERUSEDBYPARAMSFILTER
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterUsedByParamsFilter : public Resource{
GDCLASS(CharacterUsedByParamsFilter, Resource)
private:
Variant pool;
Variant staker;
int32_t stakedAt;
int32_t claimedAt;
Variant id;
Array rewards;
int32_t endTime;
bool rewardsCollected;
Variant role;
int32_t order;
Variant mint;
Variant user;
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
void set_id(const Variant& val);
Variant get_id() const;
void set_rewards(const Array& val);
Array get_rewards() const;
void set_endTime(const int32_t& val);
int32_t get_endTime() const;
void set_rewardsCollected(const bool& val);
bool get_rewardsCollected() const;
void set_role(const Variant& val);
Variant get_role() const;
void set_order(const int32_t& val);
int32_t get_order() const;
void set_mint(const Variant& val);
Variant get_mint() const;
void set_user(const Variant& val);
Variant get_user() const;
};
} // honeycomb_resource
} // godot
#endif
