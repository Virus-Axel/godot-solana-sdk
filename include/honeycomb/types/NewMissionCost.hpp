#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONCOST
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONCOST
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/resource.hpp"

namespace godot{
namespace honeycomb_resource{

class NewMissionCost : public Resource{
GDCLASS(NewMissionCost, Resource)
private:
String address;
int64_t amount;
protected:
static void _bind_methods();
public:
Dictionary to_dict();
void from_dict(const Dictionary& dict);
void set_address(const String& val);
String get_address();
void set_amount(const int64_t& val);
int64_t get_amount();
};
} // honeycomb_resource
} // godot
#endif