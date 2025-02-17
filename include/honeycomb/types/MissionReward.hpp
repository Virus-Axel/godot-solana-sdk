#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONREWARD
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONREWARD
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums.hpp"

namespace godot{
namespace honeycomb_resource{

class MissionReward : public Resource{
GDCLASS(MissionReward, Resource)
private:
String kind;
int64_t min;
int64_t max;
String resource;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_kind(const String& val);
String get_kind() const;
void set_min(const int64_t& val);
int64_t get_min() const;
void set_max(const int64_t& val);
int64_t get_max() const;
void set_resource(const String& val);
String get_resource() const;
};
} // honeycomb_resource
} // godot
#endif
