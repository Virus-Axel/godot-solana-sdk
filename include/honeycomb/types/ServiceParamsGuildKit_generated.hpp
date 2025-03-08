#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSGUILDKIT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSGUILDKIT
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceParamsGuildKit : public Resource {
	GDCLASS(ServiceParamsGuildKit, Resource)
private:
	Variant kitId;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_kitId(const Variant &val);
	Variant get_kitId() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
