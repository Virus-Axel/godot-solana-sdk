#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYGUILDFILTER
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYGUILDFILTER

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type UsedByGuildFilter.
 */
class UsedByGuildFilter : public Resource {
	GDCLASS(UsedByGuildFilter, Resource)

private:
	Variant id;
	Variant role;
	int32_t order;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this UsedByGuildFilter.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the id property.
	 * @param val New value for id.
	 */
	void set_id(const Variant &val);
	/**
	 * @brief Get the id property.
	 * @return Current value of id.
	 */
	Variant get_id() const;

	/**
	 * @brief Set the role property.
	 * @param val New value for role.
	 */
	void set_role(const Variant &val);
	/**
	 * @brief Get the role property.
	 * @return Current value of role.
	 */
	Variant get_role() const;

	/**
	 * @brief Set the order property.
	 * @param val New value for order.
	 */
	void set_order(const int32_t &val);
	/**
	 * @brief Get the order property.
	 * @return Current value of order.
	 */
	int32_t get_order() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_USEDBYGUILDFILTER
