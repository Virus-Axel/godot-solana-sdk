#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROJECT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROJECT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class Project : public Resource {
	GDCLASS(Project, Resource)
private:
	PackedByteArray address;
	int32_t bump;
	Variant authority;
	Variant key;
	Variant driver;
	String name;
	Array services;
	Array associatedPrograms;
	Variant profileDataConfig;
	Variant profileTrees;
	Array badgeCriteria;
	bool subsidyFees;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_address(const PackedByteArray &val);
	PackedByteArray get_address() const;
	void set_bump(const int32_t &val);
	int32_t get_bump() const;
	void set_authority(const Variant &val);
	Variant get_authority() const;
	void set_key(const Variant &val);
	Variant get_key() const;
	void set_driver(const Variant &val);
	Variant get_driver() const;
	void set_name(const String &val);
	String get_name() const;
	void set_services(const Array &val);
	Array get_services() const;
	void set_associatedPrograms(const Array &val);
	Array get_associatedPrograms() const;
	void set_profileDataConfig(const Variant &val);
	Variant get_profileDataConfig() const;
	void set_profileTrees(const Variant &val);
	Variant get_profileTrees() const;
	void set_badgeCriteria(const Array &val);
	Array get_badgeCriteria() const;
	void set_subsidyFees(const bool &val);
	bool get_subsidyFees() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
