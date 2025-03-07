#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSEMBLER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSEMBLER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceParamsAssembler : public Resource {
	GDCLASS(ServiceParamsAssembler, Resource)
private:
	Variant assemblerId;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_assemblerId(const Variant &val);
	Variant get_assemblerId() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
