#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BASICTREECONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_BASICTREECONFIG
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class BasicTreeConfig : public Resource {
	GDCLASS(BasicTreeConfig, Resource)
private:
	int32_t numAssets;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_numAssets(const int32_t &val);
	int32_t get_numAssets() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
