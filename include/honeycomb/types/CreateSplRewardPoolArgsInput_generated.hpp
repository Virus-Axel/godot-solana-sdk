#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLREWARDPOOLARGSINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLREWARDPOOLARGSINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateSplRewardPoolArgsInput.
 */
class CreateSplRewardPoolArgsInput : public Resource {
	GDCLASS(CreateSplRewardPoolArgsInput, Resource)

private:
	Variant ApyConfig;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateSplRewardPoolArgsInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the ApyConfig property.
	 * @param val New value for ApyConfig.
	 */
	void set_ApyConfig(const Variant &val);
	/**
	 * @brief Get the ApyConfig property.
	 * @return Current value of ApyConfig.
	 */
	Variant get_ApyConfig() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATESPLREWARDPOOLARGSINPUT
