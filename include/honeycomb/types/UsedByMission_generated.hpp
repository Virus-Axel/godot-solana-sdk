#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYMISSION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYMISSION
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class UsedByMission : public Resource {
	GDCLASS(UsedByMission, Resource)
private:
	Variant missionId;
	String participationId;
	Array rewards;
	int32_t endTime;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_missionId(const Variant &val);
	Variant get_missionId() const;
	void set_participationId(const String &val);
	String get_participationId() const;
	void set_rewards(const Array &val);
	Array get_rewards() const;
	void set_endTime(const int32_t &val);
	int32_t get_endTime() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
