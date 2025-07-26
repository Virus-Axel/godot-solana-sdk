#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSNOTSET
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSNOTSET

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type SplRewardConfigParamsNotSet.
 */
class SplRewardConfigParamsNotSet : public Resource {
	GDCLASS(SplRewardConfigParamsNotSet, Resource)

private:
	bool notSet;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this SplRewardConfigParamsNotSet.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the notSet property.
	 * @param val New value for notSet.
	 */
	void set_notSet(const bool &val);
	/**
	 * @brief Get the notSet property.
	 * @return Current value of notSet.
	 */
	bool get_notSet() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_SPLREWARDCONFIGPARAMSNOTSET
