#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHRESPONSE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_AUTHRESPONSE
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class AuthResponse : public Resource {
	GDCLASS(AuthResponse, Resource)
private:
	String message;
	PackedByteArray tx;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_message(const String &val);
	String get_message() const;
	void set_tx(const PackedByteArray &val);
	PackedByteArray get_tx() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
