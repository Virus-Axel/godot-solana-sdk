#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAIT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAIT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CharacterTrait.
 */
class CharacterTrait : public Resource {
	GDCLASS(CharacterTrait, Resource)

private:
	PackedByteArray address;
	PackedByteArray tree_id;
	int64_t leaf_idx;
	String layer;
	String name;
	String uri;
	Variant proof;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CharacterTrait.
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
	 * @brief Set the tree_id property.
	 * @param val New value for tree_id.
	 */
	void set_tree_id(const PackedByteArray &val);
	/**
	 * @brief Get the tree_id property.
	 * @return Current value of tree_id.
	 */
	PackedByteArray get_tree_id() const;

	/**
	 * @brief Set the leaf_idx property.
	 * @param val New value for leaf_idx.
	 */
	void set_leaf_idx(const int64_t &val);
	/**
	 * @brief Get the leaf_idx property.
	 * @return Current value of leaf_idx.
	 */
	int64_t get_leaf_idx() const;

	/**
	 * @brief Set the layer property.
	 * @param val New value for layer.
	 */
	void set_layer(const String &val);
	/**
	 * @brief Get the layer property.
	 * @return Current value of layer.
	 */
	String get_layer() const;

	/**
	 * @brief Set the name property.
	 * @param val New value for name.
	 */
	void set_name(const String &val);
	/**
	 * @brief Get the name property.
	 * @return Current value of name.
	 */
	String get_name() const;

	/**
	 * @brief Set the uri property.
	 * @param val New value for uri.
	 */
	void set_uri(const String &val);
	/**
	 * @brief Get the uri property.
	 * @return Current value of uri.
	 */
	String get_uri() const;

	/**
	 * @brief Set the proof property.
	 * @param val New value for proof.
	 */
	void set_proof(const Variant &val);
	/**
	 * @brief Get the proof property.
	 * @return Current value of proof.
	 */
	Variant get_proof() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CHARACTERTRAIT
