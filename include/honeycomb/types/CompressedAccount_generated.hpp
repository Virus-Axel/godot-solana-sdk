#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_COMPRESSEDACCOUNT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_COMPRESSEDACCOUNT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CompressedAccount.
 */
class CompressedAccount : public Resource {
	GDCLASS(CompressedAccount, Resource)

private:
	PackedByteArray tree_id;
	int64_t leaf_idx;
	bool schema_validated;
	PackedByteArray raw_data;
	Dictionary parsed_data;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CompressedAccount.
	 */
	Dictionary to_dict();

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
	 * @brief Set the schema_validated property.
	 * @param val New value for schema_validated.
	 */
	void set_schema_validated(const bool &val);
	/**
	 * @brief Get the schema_validated property.
	 * @return Current value of schema_validated.
	 */
	bool get_schema_validated() const;

	/**
	 * @brief Set the raw_data property.
	 * @param val New value for raw_data.
	 */
	void set_raw_data(const PackedByteArray &val);
	/**
	 * @brief Get the raw_data property.
	 * @return Current value of raw_data.
	 */
	PackedByteArray get_raw_data() const;

	/**
	 * @brief Set the parsed_data property.
	 * @param val New value for parsed_data.
	 */
	void set_parsed_data(const Dictionary &val);
	/**
	 * @brief Get the parsed_data property.
	 * @return Current value of parsed_data.
	 */
	Dictionary get_parsed_data() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_COMPRESSEDACCOUNT
