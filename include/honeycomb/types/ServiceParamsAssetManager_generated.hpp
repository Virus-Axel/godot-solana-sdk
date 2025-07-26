#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSETMANAGER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSETMANAGER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ServiceParamsAssetManager.
 */
class ServiceParamsAssetManager : public Resource {
	GDCLASS(ServiceParamsAssetManager, Resource)

private:
	Variant assetManagerId;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ServiceParamsAssetManager.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the assetManagerId property.
	 * @param val New value for assetManagerId.
	 */
	void set_assetManagerId(const Variant &val);
	/**
	 * @brief Get the assetManagerId property.
	 * @return Current value of assetManagerId.
	 */
	Variant get_assetManagerId() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SERVICEPARAMSASSETMANAGER
