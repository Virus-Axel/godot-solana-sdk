#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONDATA
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class NewMissionData : public Resource {
	GDCLASS(NewMissionData, Resource)
private:
	String project;
	String missionPool;
	String authority;
	String delegateAuthority;
	String payer;
	String name;
	int64_t minXp;
	Variant cost;
	int64_t duration;
	Array rewards;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_project(const String &val);
	String get_project() const;
	void set_missionPool(const String &val);
	String get_missionPool() const;
	void set_authority(const String &val);
	String get_authority() const;
	void set_delegateAuthority(const String &val);
	String get_delegateAuthority() const;
	void set_payer(const String &val);
	String get_payer() const;
	void set_name(const String &val);
	String get_name() const;
	void set_minXp(const int64_t &val);
	int64_t get_minXp() const;
	void set_cost(const Variant &val);
	Variant get_cost() const;
	void set_duration(const int64_t &val);
	int64_t get_duration() const;
	void set_rewards(const Array &val);
	Array get_rewards() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
