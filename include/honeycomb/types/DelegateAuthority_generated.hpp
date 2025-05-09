#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DELEGATEAUTHORITY
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DELEGATEAUTHORITY

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type DelegateAuthority.
 */
class DelegateAuthority : public Resource {
	GDCLASS(DelegateAuthority, Resource)

private:
	PackedByteArray address;
	int32_t bump;
	Variant project;
	Variant authority;
	Array delegations;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this DelegateAuthority.
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
	 * @brief Set the delegations property.
	 * @param val New value for delegations.
	 */
	void set_delegations(const Array &val);
	/**
	 * @brief Get the delegations property.
	 * @return Current value of delegations.
	 */
	Array get_delegations() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_DELEGATEAUTHORITY
