#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_UPDATEMISSIONDATA
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class UpdateMissionData : public Resource {
	GDCLASS(UpdateMissionData, Resource)
private:
	String mission;
	String authority;
	String delegateAuthority;
	String payer;
	String name;
	int64_t minXp;
	Variant cost;
	int64_t duration;
	bool removeAllRewards;
	Array addRewards;
	PackedInt32Array removeRewardIndices;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_mission(const String &val);
	String get_mission() const;
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
	void set_removeAllRewards(const bool &val);
	bool get_removeAllRewards() const;
	void set_addRewards(const Array &val);
	Array get_addRewards() const;
	void set_removeRewardIndices(const PackedInt32Array &val);
	PackedInt32Array get_removeRewardIndices() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
