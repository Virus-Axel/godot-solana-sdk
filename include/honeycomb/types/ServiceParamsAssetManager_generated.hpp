#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSETMANAGER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSETMANAGER
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

class ServiceParamsAssetManager : public Resource {
	GDCLASS(ServiceParamsAssetManager, Resource)
private:
	Variant assetManagerId;

protected:
	static void _bind_methods();

public:
	Dictionary to_dict();
	void set_assetManagerId(const Variant &val);
	Variant get_assetManagerId() const;
};
} //namespace honeycomb_resource
} //namespace godot
#endif
