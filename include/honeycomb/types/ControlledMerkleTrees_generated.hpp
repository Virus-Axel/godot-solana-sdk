#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CONTROLLEDMERKLETREES
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CONTROLLEDMERKLETREES

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ControlledMerkleTrees.
 */
class ControlledMerkleTrees : public Resource {
	GDCLASS(ControlledMerkleTrees, Resource)

private:
	int32_t active;
	Dictionary schema;
	Array merkle_trees;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ControlledMerkleTrees.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the active property.
	 * @param val New value for active.
	 */
	void set_active(const int32_t &val);
	/**
	 * @brief Get the active property.
	 * @return Current value of active.
	 */
	int32_t get_active() const;

	/**
	 * @brief Set the schema property.
	 * @param val New value for schema.
	 */
	void set_schema(const Dictionary &val);
	/**
	 * @brief Get the schema property.
	 * @return Current value of schema.
	 */
	Dictionary get_schema() const;

	/**
	 * @brief Set the merkle_trees property.
	 * @param val New value for merkle_trees.
	 */
	void set_merkle_trees(const Array &val);
	/**
	 * @brief Get the merkle_trees property.
	 * @return Current value of merkle_trees.
	 */
	Array get_merkle_trees() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CONTROLLEDMERKLETREES
