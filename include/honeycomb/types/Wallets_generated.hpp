#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WALLETS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WALLETS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Wallets.
 */
class Wallets : public Resource {
	GDCLASS(Wallets, Resource)

private:
	Variant shadow;
	Array wallets;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Wallets.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the shadow property.
	 * @param val New value for shadow.
	 */
	void set_shadow(const Variant &val);
	/**
	 * @brief Get the shadow property.
	 * @return Current value of shadow.
	 */
	Variant get_shadow() const;

	/**
	 * @brief Set the wallets property.
	 * @param val New value for wallets.
	 */
	void set_wallets(const Array &val);
	/**
	 * @brief Get the wallets property.
	 * @return Current value of wallets.
	 */
	Array get_wallets() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_WALLETS
