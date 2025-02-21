#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_EARNEDREWARDS
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class EarnedRewards : public Resource{
GDCLASS(EarnedRewards, Resource)
private:
int32_t delta;
int32_t rewardIdx;
bool collected;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_delta(const int32_t& val);
int32_t get_delta() const;
void set_rewardIdx(const int32_t& val);
int32_t get_rewardIdx() const;
void set_collected(const bool& val);
bool get_collected() const;
};
} // honeycomb_resource
} // godot
#endif
