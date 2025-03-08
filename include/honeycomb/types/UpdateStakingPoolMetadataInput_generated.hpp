#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATESTAKINGPOOLMETADATAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATESTAKINGPOOLMETADATAINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class UpdateStakingPoolMetadataInput : public Resource {
	GDCLASS(UpdateStakingPoolMetadataInput, Resource)
private:
	String name;
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;
	int64_t maxRewardsDuration;
	int64_t minStakeDuration;
	int64_t cooldownDuration;
	bool resetStakeDuration;
	int64_t startTime;
	int64_t endTime;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_name(const String &val);
	String get_name() const;
	void set_rewardsPerDuration(const int64_t &val);
	int64_t get_rewardsPerDuration() const;
	void set_rewardsDuration(const int64_t &val);
	int64_t get_rewardsDuration() const;
	void set_maxRewardsDuration(const int64_t &val);
	int64_t get_maxRewardsDuration() const;
	void set_minStakeDuration(const int64_t &val);
	int64_t get_minStakeDuration() const;
	void set_cooldownDuration(const int64_t &val);
	int64_t get_cooldownDuration() const;
	void set_resetStakeDuration(const bool &val);
	bool get_resetStakeDuration() const;
	void set_startTime(const int64_t &val);
	int64_t get_startTime() const;
	void set_endTime(const int64_t &val);
	int64_t get_endTime() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
