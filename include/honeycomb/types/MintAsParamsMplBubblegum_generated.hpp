#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASPARAMSMPLBUBBLEGUM
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASPARAMSMPLBUBBLEGUM

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type MintAsParamsMplBubblegum.
 */
class MintAsParamsMplBubblegum : public Resource {
	GDCLASS(MintAsParamsMplBubblegum, Resource)

private:
	Variant merkleTree;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this MintAsParamsMplBubblegum.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the merkleTree property.
	 * @param val New value for merkleTree.
	 */
	void set_merkleTree(const Variant &val);
	/**
	 * @brief Get the merkleTree property.
	 * @return Current value of merkleTree.
	 */
	Variant get_merkleTree() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_MINTASPARAMSMPLBUBBLEGUM
