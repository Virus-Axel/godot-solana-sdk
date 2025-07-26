#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCESTORAGEPARAMS
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCESTORAGEPARAMS

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ResourceStorageParams.
 */
class ResourceStorageParams : public Resource {
	GDCLASS(ResourceStorageParams, Resource)

private:
	Variant merkle_trees;
	int64_t promiseSupply;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ResourceStorageParams.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the merkle_trees property.
	 * @param val New value for merkle_trees.
	 */
	void set_merkle_trees(const Variant &val);
	/**
	 * @brief Get the merkle_trees property.
	 * @return Current value of merkle_trees.
	 */
	Variant get_merkle_trees() const;

	/**
	 * @brief Set the promiseSupply property.
	 * @param val New value for promiseSupply.
	 */
	void set_promiseSupply(const int64_t &val);
	/**
	 * @brief Get the promiseSupply property.
	 * @return Current value of promiseSupply.
	 */
	int64_t get_promiseSupply() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_RESOURCESTORAGEPARAMS
