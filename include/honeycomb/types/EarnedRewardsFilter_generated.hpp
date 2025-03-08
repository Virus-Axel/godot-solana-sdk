#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDSFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDSFILTER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class EarnedRewardsFilter : public Resource {
	GDCLASS(EarnedRewardsFilter, Resource)
private:
	int32_t delta;
	int32_t rewardIdx;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_delta(const int32_t &val);
	int32_t get_delta() const;
	void set_rewardIdx(const int32_t &val);
	int32_t get_rewardIdx() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
