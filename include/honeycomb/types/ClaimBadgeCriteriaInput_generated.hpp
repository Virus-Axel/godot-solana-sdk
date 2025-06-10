#ifndef GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
#define GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "honeycomb/enums_generated.hpp"

namespace godot {
namespace honeycomb_resource {

/**
 * @brief Resource wrapper for honeycomb type ClaimBadgeCriteriaInput.
 */
class ClaimBadgeCriteriaInput : public Resource {
	GDCLASS(ClaimBadgeCriteriaInput, Resource)

private:
	Variant projectAddress;
	Variant profileAddress;
	Variant payer;
	int32_t criteriaIndex;
	String proof;

protected:
	/**
	 * @brief Bind methods and properties to the Godot engine.
	 */
	static void _bind_methods();

public:
	/**
	 * @brief Convert this resource to a Dictionary.
	 * @return Dictionary representation of this ClaimBadgeCriteriaInput.
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
	 * @brief Set the profileAddress property.
	 * @param val New value for profileAddress.
	 */
	void set_profileAddress(const Variant &val);
	/**
	 * @brief Get the profileAddress property.
	 * @return Current value of profileAddress.
	 */
	Variant get_profileAddress() const;

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
	 * @brief Set the criteriaIndex property.
	 * @param val New value for criteriaIndex.
	 */
	void set_criteriaIndex(const int32_t &val);
	/**
	 * @brief Get the criteriaIndex property.
	 * @return Current value of criteriaIndex.
	 */
	int32_t get_criteriaIndex() const;

	/**
	 * @brief Set the proof property.
	 * @param val New value for proof.
	 */
	void set_proof(const String &val);
	/**
	 * @brief Get the proof property.
	 * @return Current value of proof.
	 */
	String get_proof() const;
};

} // namespace honeycomb_resource
} // namespace godot

#endif // GODOT_SOLANA_SDK_HONEYCOMB_TYPE_CLAIMBADGECRITERIAINPUT
