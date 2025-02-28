#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERHISTORY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERHISTORY
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class CharacterHistory : public Resource{
GDCLASS(CharacterHistory, Resource)
private:
PackedByteArray address;
String event;
Dictionary event_data;
String time;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void set_address(const PackedByteArray& val);
PackedByteArray get_address() const;
void set_event(const String& val);
String get_event() const;
void set_event_data(const Dictionary& val);
Dictionary get_event_data() const;
void set_time(const String& val);
String get_time() const;
};
} // honeycomb_resource
} // godot
#endif
