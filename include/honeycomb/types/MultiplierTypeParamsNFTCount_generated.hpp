#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSNFTCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSNFTCOUNT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MultiplierTypeParamsNFTCount.
 */
class MultiplierTypeParamsNFTCount : public Resource {
	GDCLASS(MultiplierTypeParamsNFTCount, Resource)

private:
	int64_t minCount;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MultiplierTypeParamsNFTCount.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the minCount property.
	 * @param val New value for minCount.
	 */
	void set_minCount(const int64_t &val);
	/**
	 * @brief Get the minCount property.
	 * @return Current value of minCount.
	 */
	int64_t get_minCount() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MULTIPLIERTYPEPARAMSNFTCOUNT
