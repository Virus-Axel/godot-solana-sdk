#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ImportResourceInput.
 */
class ImportResourceInput : public Resource {
	GDCLASS(ImportResourceInput, Resource)

private:
	int32_t decimals;
	String project;
	String mint;
	String authority;
	String storage;
	PackedStringArray tags;
	Variant custody;
	String payer;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ImportResourceInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the decimals property.
	 * @param val New value for decimals.
	 */
	void set_decimals(const int32_t &val);
	/**
	 * @brief Get the decimals property.
	 * @return Current value of decimals.
	 */
	int32_t get_decimals() const;

	/**
	 * @brief Set the project property.
	 * @param val New value for project.
	 */
	void set_project(const String &val);
	/**
	 * @brief Get the project property.
	 * @return Current value of project.
	 */
	String get_project() const;

	/**
	 * @brief Set the mint property.
	 * @param val New value for mint.
	 */
	void set_mint(const String &val);
	/**
	 * @brief Get the mint property.
	 * @return Current value of mint.
	 */
	String get_mint() const;

	/**
	 * @brief Set the authority property.
	 * @param val New value for authority.
	 */
	void set_authority(const String &val);
	/**
	 * @brief Get the authority property.
	 * @return Current value of authority.
	 */
	String get_authority() const;

	/**
	 * @brief Set the storage property.
	 * @param val New value for storage.
	 */
	void set_storage(const String &val);
	/**
	 * @brief Get the storage property.
	 * @return Current value of storage.
	 */
	String get_storage() const;

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

	/**
	 * @brief Set the custody property.
	 * @param val New value for custody.
	 */
	void set_custody(const Variant &val);
	/**
	 * @brief Get the custody property.
	 * @return Current value of custody.
	 */
	Variant get_custody() const;

	/**
	 * @brief Set the payer property.
	 * @param val New value for payer.
	 */
	void set_payer(const String &val);
	/**
	 * @brief Get the payer property.
	 * @return Current value of payer.
	 */
	String get_payer() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_IMPORTRESOURCEINPUT
