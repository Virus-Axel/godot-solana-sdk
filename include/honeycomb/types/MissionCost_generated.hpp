#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONCOST
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSIONCOST
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class MissionCost : public Resource {
	GDCLASS(MissionCost, Resource)
private:
	int64_t amount;
	Variant resource_address;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_amount(const int64_t &val);
	int64_t get_amount() const;
	void set_resource_address(const Variant &val);
	Variant get_resource_address() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
