#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARD
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_REWARD
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class Reward : public Resource{
GDCLASS(Reward, Resource)
private:
int64_t min;
int64_t max;
Dictionary rewardType;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_min(const int64_t& val);
int64_t get_min() const;
void set_max(const int64_t& val);
int64_t get_max() const;
void set_rewardType(const Dictionary& val);
Dictionary get_rewardType() const;
};
} // honeycomb_resource
} // godot
#endif
