#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PARTIALUSERINFOINPUT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class PartialUserInfoInput : public Resource {
	GDCLASS(PartialUserInfoInput, Resource)
private:
	String name;
	String bio;
	String pfp;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_name(const String &val);
	String get_name() const;
	void set_bio(const String &val);
	String get_bio() const;
	void set_pfp(const String &val);
	String get_pfp() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
