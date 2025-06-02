#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBADGECRITERIAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type CreateBadgeCriteriaInput.
 */
class CreateBadgeCriteriaInput : public Resource {
	GDCLASS(CreateBadgeCriteriaInput, Resource)

private:
	Variant projectAddress;
	Variant authority;
	Variant payer;
	int32_t startTime;
	int32_t endTime;
	int32_t badgeIndex;
	String condition;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this CreateBadgeCriteriaInput.
	 */
	Dictionary to_dict();

	/**
	 * @brief Set the projectAddress property.
	 * @param val New value for projectAddress.
	 */
	void set_projectAddress(const Variant &val);
	/**
	 * @brief Get the projectAddress property.
	 * @return Current value of projectAddress.
	 */
	Variant get_projectAddress() const;

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
	 * @brief Set the payer property.
	 * @param val New value for payer.
	 */
	void set_payer(const Variant &val);
	/**
	 * @brief Get the payer property.
	 * @return Current value of payer.
	 */
	Variant get_payer() const;

	/**
	 * @brief Set the startTime property.
	 * @param val New value for startTime.
	 */
	void set_startTime(const int32_t &val);
	/**
	 * @brief Get the startTime property.
	 * @return Current value of startTime.
	 */
	int32_t get_startTime() const;

	/**
	 * @brief Set the endTime property.
	 * @param val New value for endTime.
	 */
	void set_endTime(const int32_t &val);
	/**
	 * @brief Get the endTime property.
	 * @return Current value of endTime.
	 */
	int32_t get_endTime() const;

	/**
	 * @brief Set the badgeIndex property.
	 * @param val New value for badgeIndex.
	 */
	void set_badgeIndex(const int32_t &val);
	/**
	 * @brief Get the badgeIndex property.
	 * @return Current value of badgeIndex.
	 */
	int32_t get_badgeIndex() const;

	/**
	 * @brief Set the condition property.
	 * @param val New value for condition.
	 */
	void set_condition(const String &val);
	/**
	 * @brief Get the condition property.
	 * @return Current value of condition.
	 */
	String get_condition() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CREATEBADGECRITERIAINPUT
