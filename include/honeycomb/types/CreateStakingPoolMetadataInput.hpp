#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESTAKINGPOOLMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESTAKINGPOOLMETADATAINPUT
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CreateStakingPoolMetadataInput : public Resource{
GDCLASS(CreateStakingPoolMetadataInput, Resource)
private:
String name;
int64_t rewardsPerDuration;
int64_t rewardsDuration;
int64_t maxRewardsDuration;
int64_t minStakeDuration;
int64_t cooldownDuration;
bool resetStakeDuration;
int64_t startTime;
int32_t lockType;
int64_t endTime;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_name(const String& val);
String get_name();
void set_rewardsPerDuration(const int64_t& val);
int64_t get_rewardsPerDuration();
void set_rewardsDuration(const int64_t& val);
int64_t get_rewardsDuration();
void set_maxRewardsDuration(const int64_t& val);
int64_t get_maxRewardsDuration();
void set_minStakeDuration(const int64_t& val);
int64_t get_minStakeDuration();
void set_cooldownDuration(const int64_t& val);
int64_t get_cooldownDuration();
void set_resetStakeDuration(const bool& val);
bool get_resetStakeDuration();
void set_startTime(const int64_t& val);
int64_t get_startTime();
void set_lockType(const int32_t& val);
int32_t get_lockType();
void set_endTime(const int64_t& val);
int64_t get_endTime();
};
} // honeycomb_resource
} // godot
#endif