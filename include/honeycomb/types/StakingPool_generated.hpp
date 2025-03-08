#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGPOOL
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKINGPOOL
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class StakingPool : public Resource {
	GDCLASS(StakingPool, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant key;
	Variant resource;
	Variant lockType;
	String name;
	int64_t rewardsPerDuration;
	int64_t rewardsDuration;
	int64_t maxRewardsDuration;
	int64_t minStakeDuration;
	int64_t cooldownDuration;
	bool resetStakeDuration;
	bool allowedMints;
	int64_t totalStaked;
	int64_t startTime;
	int64_t endTime;
	Array characterModels;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_project(const Variant &val);
	Variant get_project() const;
	void set_key(const Variant &val);
	Variant get_key() const;
	void set_resource(const Variant &val);
	Variant get_resource() const;
	void set_lockType(const Variant &val);
	Variant get_lockType() const;
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
	void set_allowedMints(const bool &val);
	bool get_allowedMints() const;
	void set_totalStaked(const int64_t &val);
	int64_t get_totalStaked() const;
	void set_startTime(const int64_t &val);
	int64_t get_startTime() const;
	void set_endTime(const int64_t &val);
	int64_t get_endTime() const;
	void set_characterModels(const Array &val);
	Array get_characterModels() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
