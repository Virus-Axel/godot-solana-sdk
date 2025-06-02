#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBRESOURCE
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBRESOURCE

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type HoneycombResource.
 */
class HoneycombResource : public Resource {
	GDCLASS(HoneycombResource, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant mint;
	Variant storage;
	Variant kind;
	PackedStringArray tags;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this HoneycombResource.
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
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const Variant &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	Variant get_project() const;

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
	 * @brief Set the storage property.
	 * @param val New value for storage.
	 */
	void set_storage(const Variant &val);
	/**
	 * @brief Get the storage property.
	 * @return Current value of storage.
	 */
	Variant get_storage() const;

	/**
	 * @brief Set the kind property.
	 * @param val New value for kind.
	 */
	void set_kind(const Variant &val);
	/**
	 * @brief Get the kind property.
	 * @return Current value of kind.
	 */
	Variant get_kind() const;

	/**
	 * @brief Set the tags property.
	 * @param val New value for tags.
	 */
	void set_tags(const PackedStringArray &val);
	/**
	 * @brief Get the tags property.
	 * @return Current value of tags.
	 */
	PackedStringArray get_tags() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_HONEYCOMBRESOURCE
