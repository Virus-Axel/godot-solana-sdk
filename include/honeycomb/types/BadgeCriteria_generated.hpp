#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BADGECRITERIA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BADGECRITERIA
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot{
namespace honeycomb_resource{

class BadgeCriteria : public Resource{
GDCLASS(BadgeCriteria, Resource)
private:
int64_t startTime;
int64_t endTime;
int32_t index;
String condition;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_startTime(const int64_t& val);
int64_t get_startTime() const;
void set_endTime(const int64_t& val);
int64_t get_endTime() const;
void set_index(const int32_t& val);
int32_t get_index() const;
void set_condition(const String& val);
String get_condition() const;
};
} // honeycomb_resource
} // godot
#endif
