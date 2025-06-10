#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROJECT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROJECT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type Project.
 */
class Project : public Resource {
	GDCLASS(Project, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant authority;
	Variant key;
	Variant driver;
	String name;
	Array services;
	Array associatedPrograms;
	Variant profileDataConfig;
	Variant profileTrees;
	Array badgeCriteria;
	bool subsidyFees;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this Project.
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
	 * @brief Set the authority property.
	 * @param val New value for authority.
	 */
	void set_authority(const Variant &val);
	/**
	 * @brief Get the authority property.
	 * @return Current value of authority.
	 */
	Variant get_authority() const;

	/**
	 * @brief Set the key property.
	 * @param val New value for key.
	 */
	void set_key(const Variant &val);
	/**
	 * @brief Get the key property.
	 * @return Current value of key.
	 */
	Variant get_key() const;

	/**
	 * @brief Set the driver property.
	 * @param val New value for driver.
	 */
	void set_driver(const Variant &val);
	/**
	 * @brief Get the driver property.
	 * @return Current value of driver.
	 */
	Variant get_driver() const;

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
	 * @brief Set the services property.
	 * @param val New value for services.
	 */
	void set_services(const Array &val);
	/**
	 * @brief Get the services property.
	 * @return Current value of services.
	 */
	Array get_services() const;

	/**
	 * @brief Set the associatedPrograms property.
	 * @param val New value for associatedPrograms.
	 */
	void set_associatedPrograms(const Array &val);
	/**
	 * @brief Get the associatedPrograms property.
	 * @return Current value of associatedPrograms.
	 */
	Array get_associatedPrograms() const;

	/**
	 * @brief Set the profileDataConfig property.
	 * @param val New value for profileDataConfig.
	 */
	void set_profileDataConfig(const Variant &val);
	/**
	 * @brief Get the profileDataConfig property.
	 * @return Current value of profileDataConfig.
	 */
	Variant get_profileDataConfig() const;

	/**
	 * @brief Set the profileTrees property.
	 * @param val New value for profileTrees.
	 */
	void set_profileTrees(const Variant &val);
	/**
	 * @brief Get the profileTrees property.
	 * @return Current value of profileTrees.
	 */
	Variant get_profileTrees() const;

	/**
	 * @brief Set the badgeCriteria property.
	 * @param val New value for badgeCriteria.
	 */
	void set_badgeCriteria(const Array &val);
	/**
	 * @brief Get the badgeCriteria property.
	 * @return Current value of badgeCriteria.
	 */
	Array get_badgeCriteria() const;

	/**
	 * @brief Set the subsidyFees property.
	 * @param val New value for subsidyFees.
	 */
	void set_subsidyFees(const bool &val);
	/**
	 * @brief Get the subsidyFees property.
	 * @return Current value of subsidyFees.
	 */
	bool get_subsidyFees() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_PROJECT
