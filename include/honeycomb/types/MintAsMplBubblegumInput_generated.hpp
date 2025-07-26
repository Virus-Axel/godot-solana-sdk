#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASMPLBUBBLEGUMINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASMPLBUBBLEGUMINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MintAsMplBubblegumInput.
 */
class MintAsMplBubblegumInput : public Resource {
	GDCLASS(MintAsMplBubblegumInput, Resource)

private:
	int32_t maxDepth;
	int32_t maxBufferSize;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MintAsMplBubblegumInput.
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
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASMPLBUBBLEGUMINPUT
