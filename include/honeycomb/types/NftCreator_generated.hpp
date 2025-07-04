#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NFTCREATOR
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NFTCREATOR

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type NftCreator.
 */
class NftCreator : public Resource {
	GDCLASS(NftCreator, Resource)

private:
	Variant address;
	int32_t share;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this NftCreator.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const Variant &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	Variant get_address() const;

	/**
	 * @brief Set the share property.
	 * @param val New value for share.
	 */
	void set_share(const int32_t &val);
	/**
	 * @brief Get the share property.
	 * @return Current value of share.
	 */
	int32_t get_share() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_NFTCREATOR
