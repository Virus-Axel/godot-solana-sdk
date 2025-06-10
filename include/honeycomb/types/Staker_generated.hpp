#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Staker.
 */
class Staker : public Resource {
	GDCLASS(Staker, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant stakingPool;
	Variant wallet;
	int64_t totalStaked;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Staker.
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
	 * @brief Set the bump property.
	 * @param val New value for bump.
	 */
	void set_bump(const int32_t &val);
	/**
	 * @brief Get the bump property.
	 * @return Current value of bump.
	 */
	int32_t get_bump() const;

	/**
	 * @brief Set the stakingPool property.
	 * @param val New value for stakingPool.
	 */
	void set_stakingPool(const Variant &val);
	/**
	 * @brief Get the stakingPool property.
	 * @return Current value of stakingPool.
	 */
	Variant get_stakingPool() const;

	/**
	 * @brief Set the wallet property.
	 * @param val New value for wallet.
	 */
	void set_wallet(const Variant &val);
	/**
	 * @brief Get the wallet property.
	 * @return Current value of wallet.
	 */
	Variant get_wallet() const;

	/**
	 * @brief Set the totalStaked property.
	 * @param val New value for totalStaked.
	 */
	void set_totalStaked(const int64_t &val);
	/**
	 * @brief Get the totalStaked property.
	 * @return Current value of totalStaked.
	 */
	int64_t get_totalStaked() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_STAKER
