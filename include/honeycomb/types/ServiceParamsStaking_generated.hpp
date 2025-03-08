#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSSTAKING
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSSTAKING
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceParamsStaking : public Resource {
	GDCLASS(ServiceParamsStaking, Resource)
private:
	Variant poolId;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_poolId(const Variant &val);
	Variant get_poolId() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
