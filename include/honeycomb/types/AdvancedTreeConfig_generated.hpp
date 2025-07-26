#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADVANCEDTREECONFIG
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADVANCEDTREECONFIG

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type AdvancedTreeConfig.
 */
class AdvancedTreeConfig : public Resource {
	GDCLASS(AdvancedTreeConfig, Resource)

private:
	int32_t maxDepth;
	int32_t maxBufferSize;
	int32_t canopyDepth;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this AdvancedTreeConfig.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the maxDepth property.
	 * @param val New value for maxDepth.
	 */
	void set_maxDepth(const int32_t &val);
	/**
	 * @brief Get the maxDepth property.
	 * @return Current value of maxDepth.
	 */
	int32_t get_maxDepth() const;

	/**
	 * @brief Set the maxBufferSize property.
	 * @param val New value for maxBufferSize.
	 */
	void set_maxBufferSize(const int32_t &val);
	/**
	 * @brief Get the maxBufferSize property.
	 * @return Current value of maxBufferSize.
	 */
	int32_t get_maxBufferSize() const;

	/**
	 * @brief Set the canopyDepth property.
	 * @param val New value for canopyDepth.
	 */
	void set_canopyDepth(const int32_t &val);
	/**
	 * @brief Get the canopyDepth property.
	 * @return Current value of canopyDepth.
	 */
	int32_t get_canopyDepth() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_ADVANCEDTREECONFIG
