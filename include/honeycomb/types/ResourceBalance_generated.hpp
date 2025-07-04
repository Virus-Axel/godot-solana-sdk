#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEBALANCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEBALANCE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceBalance.
 */
class ResourceBalance : public Resource {
	GDCLASS(ResourceBalance, Resource)

private:
	PackedByteArray address;
	String amount;
	Variant mint;
	String wallet;
	String resourceStorage;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceBalance.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the address property.
	 * @param val New value for address.
	 */
	void set_address(const PackedByteArray &val);
	/**
	 * @brief Get the address property.
	 * @return Current value of address.
	 */
	PackedByteArray get_address() const;

	/**
	 * @brief Set the amount property.
	 * @param val New value for amount.
	 */
	void set_amount(const String &val);
	/**
	 * @brief Get the amount property.
	 * @return Current value of amount.
	 */
	String get_amount() const;

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const Variant &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	Variant get_mint() const;

	/**
	 * @brief Set the wallet property.
	 * @param val New value for wallet.
	 */
	void set_wallet(const String &val);
	/**
	 * @brief Get the wallet property.
	 * @return Current value of wallet.
	 */
	String get_wallet() const;

	/**
	 * @brief Set the resourceStorage property.
	 * @param val New value for resourceStorage.
	 */
	void set_resourceStorage(const String &val);
	/**
	 * @brief Get the resourceStorage property.
	 * @return Current value of resourceStorage.
	 */
	String get_resourceStorage() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCEBALANCE
