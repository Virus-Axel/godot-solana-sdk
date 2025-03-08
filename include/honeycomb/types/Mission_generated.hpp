#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSION
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MISSION
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Mission : public Resource {
	GDCLASS(Mission, Resource)
private:
	PackedByteArray address;
	PackedByteArray program_id;
	PackedByteArray discriminator;
	int32_t bump;
	Variant project;
	Variant missionPool;
	String name;
	int64_t minXp;
	Variant cost;
	Dictionary requirement;
	Array rewards;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_program_id(const PackedByteArray &val);
	PackedByteArray get_program_id() const;
	void set_discriminator(const PackedByteArray &val);
	PackedByteArray get_discriminator() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_project(const Variant &val);
	Variant get_project() const;
	void set_missionPool(const Variant &val);
	Variant get_missionPool() const;
	void set_name(const String &val);
	String get_name() const;
	void set_minXp(const int64_t &val);
	int64_t get_minXp() const;
	void set_cost(const Variant &val);
	Variant get_cost() const;
	void set_requirement(const Dictionary &val);
	Dictionary get_requirement() const;
	void set_rewards(const Array &val);
	Array get_rewards() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
