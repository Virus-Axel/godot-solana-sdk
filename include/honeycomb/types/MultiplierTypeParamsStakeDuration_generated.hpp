#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSSTAKEDURATION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSSTAKEDURATION
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class MultiplierTypeParamsStakeDuration : public Resource{
GDCLASS(MultiplierTypeParamsStakeDuration, Resource)
private:
int64_t minDuration;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_minDuration(const int64_t& val);
int64_t get_minDuration() const;
};
} // honeycomb_resource
} // godot
#endif
