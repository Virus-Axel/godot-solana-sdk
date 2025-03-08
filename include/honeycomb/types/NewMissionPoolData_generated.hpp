#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONPOOLDATA
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NEWMISSIONPOOLDATA
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class NewMissionPoolData : public Resource {
	GDCLASS(NewMissionPoolData, Resource)
private:
	String project;
	String authority;
	String delegateAuthority;
	String payer;
	String characterModel;
	String name;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_project(const String &val);
	String get_project() const;
	void set_authority(const String &val);
	String get_authority() const;
	void set_delegateAuthority(const String &val);
	String get_delegateAuthority() const;
	void set_payer(const String &val);
	String get_payer() const;
	void set_characterModel(const String &val);
	String get_characterModel() const;
	void set_name(const String &val);
	String get_name() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
